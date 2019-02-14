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

#include "tlm_socket.h"

using namespace sc_core;
using namespace tlm;

TlmSocket::TlmSocket()
    : TlmSocket("tlm_socket"){}

TlmSocket::TlmSocket(sc_module_name name)
    : sc_module(name){}

TlmSocket::~TlmSocket(){}

StandardPacket TlmSocket::read_register(int address)
{
    trans.set_command(TLM_READ_COMMAND);
    trans.set_data_length(data_length);
    trans.set_streaming_width(data_length);
    trans.set_byte_enable_ptr(0);
    trans.set_dmi_allowed(false);
    trans.set_data_ptr((unsigned char*)(&data) );
    trans.set_address(address);
    trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

    tlm_socket_port->b_transport(trans,delay);

    if ( trans.is_response_error() ){
        SC_REPORT_ERROR("TLM-2.0", trans.get_response_string().c_str());
    }

    StandardPacket resp = *reinterpret_cast<StandardPacket*>(&data);

    if (trans.is_response_error()){
        SC_REPORT_ERROR("TLM-2.0", trans.get_response_string().c_str());
    }

    return resp;
}

void TlmSocket::write_register(int address, StandardPacket writeData)
{
    tlm_generic_payload *trans = new tlm_generic_payload;

    trans->set_command(TLM_WRITE_COMMAND);
    trans->set_address(address);
    trans->set_data_ptr((unsigned char*)(&writeData) );
    trans->set_data_length(data_length);
    trans->set_streaming_width(data_length);
    trans->set_byte_enable_ptr(0);
    trans->set_dmi_allowed(false);
    trans->set_response_status(TLM_INCOMPLETE_RESPONSE);

    tlm_socket_port->b_transport(*trans, delay);
    delete trans;
}
