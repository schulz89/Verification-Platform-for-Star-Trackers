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

#ifndef TLM_SOCKET_H
#define TLM_SOCKET_H

#include <systemc>
#include <uvm>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "standard_packet.h"

class TlmSocket : public sc_core::sc_module
{
public:
    TlmSocket();
    TlmSocket(sc_core::sc_module_name);
    ~TlmSocket();
    StandardPacket read_register(int address);
    void write_register(int address, StandardPacket writeData);
    tlm_utils::simple_initiator_socket<TlmSocket> tlm_socket_port;

    tlm::tlm_generic_payload trans;
    sc_core::sc_time delay = sc_core::sc_time(1.0, sc_core::SC_PS);

private:
    StandardPacket data;
    int data_length = sizeof(StandardPacket);
};

#endif // TLM_SOCKET_H
