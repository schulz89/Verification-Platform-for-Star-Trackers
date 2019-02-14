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

#include "tcpdutclient.h"

using namespace std;
using namespace std_str;
using namespace cv;

TcpDutClient::TcpDutClient(std::string server_address)
{
    address = server_address;
}

void TcpDutClient::clientReceiveSend(Sky transform(Sky))
{
    Sky sky;
    struct addrinfo hints, *res;
    char* data[4];
    char* ptr;
    char header[HEADER_SIZE];
    int sockfd, len;
    int rows, cols, cvtype;
    int val = 0;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(address.data(), PORT, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    // Receive: Sky Image
    val = recv(sockfd, header, HEADER_SIZE, 0);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    size_t element_size;
    sscanf(header,"%d,%d,%lu,%d",&rows,&cols,&element_size,&cvtype);
    len = rows*cols*element_size;
    data[0] = (char*) malloc(len);
    val = recvall(sockfd, data[0], &len);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    Mat image(rows,cols,cvtype,data[0]);
    sky.image = image.clone();
    // Receive: Star Array
    val = recv(sockfd, header, HEADER_SIZE, 0);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    sscanf(header,"%d",&len);
    data[1] = (char*) malloc(len);
    val = recvall(sockfd, data[1], &len);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    vector<Star> star_array((Star*) data[1], (Star*) data[1] + len/sizeof(Star));
    sky.stars = star_array;
    // Receive: Time (double) array
    val = recv(sockfd, header, HEADER_SIZE, 0);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    sscanf(header,"%d",&len);
    data[2] = (char*) malloc(len);
    val = recvall(sockfd, data[2], &len);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    vector<double> time_array((double*) data[2], (double*) data[2] + len/sizeof(double));
    sky.time = time_array;
    // Receive: Quaternion
    len = sizeof(Quaternion);
    data[3] = (char*) malloc(len);
    val = recvall(sockfd, data[3], &len);
    if(val < 0){
        cerr << "Disconnected by server" << endl;
        exit(0xFF);
    }
    Quaternion* q = (Quaternion*)data[3];
    sky.q = *q;
    // This will call the function provided as an argument to transform the input data
    sky = transform(sky);
    // Send: Sky image
    ptr = (char*)sky.image.data;
    len = sky.image.rows * sky.image.cols * sky.image.elemSize();
    sprintf(header,"%d,%d,%lu,%d",sky.image.rows,sky.image.cols,sky.image.elemSize(),sky.image.type());
    send(sockfd, header, HEADER_SIZE, 0);
    sendall(sockfd, (char*)ptr, &len);
    // Send: Star Array
    ptr = (char*) sky.stars.data();
    len = sky.stars.size() * sizeof(Star);
    sprintf(header,"%d",len);
    send(sockfd, header, HEADER_SIZE, 0);
    sendall(sockfd, (char*)ptr, &len);
    // Send: Time (double) array
    ptr = (char*) sky.time.data();
    len = sky.time.size() * sizeof(double);
    sprintf(header,"%d",len);
    send(sockfd, header, HEADER_SIZE, 0);
    sendall(sockfd, (char*)ptr, &len);
    // Send: Quaternion
    ptr = (char*) &sky.q;
    len = sizeof(Quaternion);
    sendall(sockfd, (char*)ptr, &len);
    // Closing
    free(data[0]);
    free(data[1]);
    free(data[2]);
    free(data[3]);
    close(sockfd);
}

int TcpDutClient::sendall(int s, char *buf, int *len)
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

int TcpDutClient::recvall(int s, char *buf, int *len)
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
