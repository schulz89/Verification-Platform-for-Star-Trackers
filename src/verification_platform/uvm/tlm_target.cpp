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

#include "tlm_target.h"

using namespace std;
using namespace sc_core;
using namespace tlm;

TlmTarget::TlmTarget()
    : TlmTarget("tlm_target"){}

TlmTarget::TlmTarget(sc_module_name name)
    : sc_module(name)
    , packet()
{
    tlm_target_port.register_b_transport(this, &TlmTarget::b_transport);
}

TlmTarget::~TlmTarget(){}

void TlmTarget::b_transport (tlm_generic_payload &trans, sc_time &delay)
{
    (void)delay;
    tlm_command CMD = trans.get_command();
    int              adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned int     wid = trans.get_streaming_width();
    unsigned char*   byt = trans.get_byte_enable_ptr();

    if (adr >= 20) { /// Check for storage address overflow
        trans.set_response_status( TLM_ADDRESS_ERROR_RESPONSE );
        return;
    }
    if (byt) { /// Target unable to support byte enable attribute
        trans.set_response_status( TLM_BYTE_ENABLE_ERROR_RESPONSE );
        return;
    }
    if (wid < len) { /// Target unable to support streaming width attribute
        trans.set_response_status( TLM_BURST_ERROR_RESPONSE );
        return;
    }
    if (CMD == TLM_WRITE_COMMAND){  /// Execute command
        StandardPacket *ptr_ = (StandardPacket*)ptr;
        packet.sky_str.stars = ptr_->sky_str.stars;
        memcpy(&packet.sky_str.q, &ptr_->sky_str.q, sizeof(ptr_->sky_str.q));
        packet.sky_str.image = ptr_->sky_str.image.clone();
        packet.sky_str.time = ptr_->sky_str.time;

    }else if (CMD == TLM_READ_COMMAND){
        //memcpy(ptr, &packet, len);
    }
    trans.set_response_status( TLM_OK_RESPONSE ); /// Successful completion
    ev.notify();
}
