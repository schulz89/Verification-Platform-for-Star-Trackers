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

#ifndef TLM_TARGET_H
#define TLM_TARGET_H

#include <iostream>
#include <vector>

#include <systemc>
#include <uvm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "standard_packet.h"

class TlmTarget : public sc_core::sc_module
{
public:
    TlmTarget();
    TlmTarget(sc_core::sc_module_name name);
    ~TlmTarget();
    virtual void b_transport (tlm::tlm_generic_payload &trans, sc_core::sc_time &delay);

    StandardPacket packet;
    sc_core::sc_event ev;
    tlm_utils::simple_target_socket<TlmTarget> tlm_target_port;
};

#endif // TLM_TARGET_H
