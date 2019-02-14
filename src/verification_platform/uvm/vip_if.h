// Copyright 2019 Victor Hugo Schulz
// Copyright 2018 Jeferson Santos Barros

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef VIP_IF_H
#define VIP_IF_H

#include <systemc>
#include "tlm_socket.h"
#include "tlm_target.h"

class vip_if {

public:

    TlmSocket tlm_socket;
    TlmTarget tlm_target;

    vip_if(){
        // constructor
        tlm_socket.tlm_socket_port(tlm_target.tlm_target_port);
    }
};
#endif /* VIP_IF_H_ */
