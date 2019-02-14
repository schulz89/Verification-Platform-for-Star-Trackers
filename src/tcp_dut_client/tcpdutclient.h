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

#ifndef TCPDUTCLIENT_H
#define TCPDUTCLIENT_H

#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <unistd.h>
#include <netdb.h>
#include "standard_structures.h"

#define PORT "9964"
#define BACKLOG 64
#define HEADER_SIZE 64

class TcpDutClient
{
public:
    TcpDutClient(std::string server_address);
    void clientReceiveSend(std_str::Sky transform(std_str::Sky));

private:
    int sendall(int s, char *buf, int *len);
    int recvall(int s, char *buf, int *len);
    std::string address;
};

#endif // TCPDUTCLIENT_H
