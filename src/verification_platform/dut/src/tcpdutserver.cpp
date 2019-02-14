// Copyright 2019 Victor Hugo Schulz

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tcpdutserver.h"

using namespace std;
using namespace cv;
using namespace std_str;

TcpDutServer::TcpDutServer()
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;//AF_UNSPEC;     // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    getaddrinfo(NULL, PORT, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);
}

TcpDutServer::~TcpDutServer()
{
    close(sockfd);
}

int TcpDutServer::sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int TcpDutServer::recvall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've received
    int bytesleft = *len; // how many we have left to receive
    int n;
    while(total < *len) {
        n = recv(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    *len = total; // return number actually received here
    return n==-1?-1:0; // return -1 on failure, 0 on success
}

std_str::Sky TcpDutServer::ServerSendReceive(std_str::Sky sky)
{
    Sky output;
    int len;
    char* data[4];
    char* ptr;
    char header [HEADER_SIZE];
    int rows, cols, cvtype;
    socklen_t addr_size = sizeof their_addr;
    int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    // Send: Sky image
    ptr = (char*)sky.image.data;
    len = sky.image.rows * sky.image.cols * sky.image.elemSize();
    sprintf(header,"%d,%d,%lu,%d",sky.image.rows,sky.image.cols,sky.image.elemSize(),sky.image.type());
    send(new_fd, header, HEADER_SIZE, 0);
    sendall(new_fd, (char*)ptr, &len);
    // Send: Star Array
    ptr = (char*) sky.stars.data();
    len = sky.stars.size() * sizeof(Star);
    sprintf(header,"%d",len);
    send(new_fd, header, HEADER_SIZE, 0);
    sendall(new_fd, (char*)ptr, &len);
    // Send: Time (double) array
    ptr = (char*) sky.time.data();
    len = sky.time.size() * sizeof(double);
    sprintf(header,"%d",len);
    send(new_fd, header, HEADER_SIZE, 0);
    sendall(new_fd, (char*)ptr, &len);
    // Send: Quaternion
    ptr = (char*) &sky.q;
    len = sizeof(Quaternion);
    sendall(new_fd, (char*)ptr, &len);
    // Receive: Sky Image
    recv(new_fd, header, HEADER_SIZE, 0);
    size_t element_size;
    sscanf(header,"%d,%d,%lu,%d",&rows,&cols,&element_size,&cvtype);
    len = rows*cols*element_size;
    data[0] = (char*) malloc(len);
    recvall(new_fd, data[0], &len);
    Mat image(rows,cols,cvtype,data[0]);
    output.image = image.clone(); // <<<<<<<<<<<<<<<<<<<<<<< O_O
    // Receive: Star Array
    recv(new_fd, header, HEADER_SIZE, 0);
    sscanf(header,"%d",&len);
    data[1]  = (char*) malloc(len);
    recvall(new_fd, data[1] , &len);
    vector<Star> star_array((Star*) data[1] , (Star*) data[1]  + len/sizeof(Star));
    output.stars = star_array;
    // Receive: Time (double) array
    recv(new_fd, header, HEADER_SIZE, 0);
    sscanf(header,"%d",&len);
    data[2]  = (char*) malloc(len);
    recvall(new_fd, data[2] , &len);
    vector<double> time_array((double*) data[2] , (double*) data[2]  + len/sizeof(double));
    output.time = time_array;
    // Receive: Quaternion
    len = sizeof(Quaternion);
    data[3]  = (char*) malloc(len);
    recvall(new_fd, data[3] , &len);
    Quaternion* q = (Quaternion*)data[3] ;
    output.q = *q;
    // Closing
    free(data[0]);
    free(data[1]);
    free(data[2]);
    free(data[3]);
    close(new_fd);
    return output;
}
