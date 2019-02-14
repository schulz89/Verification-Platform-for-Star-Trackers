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

#ifndef VIP_DRIVER_H_
#define VIP_DRIVER_H_

#include <systemc>
#include <tlm.h>
#include <uvm>

#include "standard_packet.h"
#include "vip_if.h"

template <class REQ>
class vip_driver : public uvm::uvm_driver<REQ> {

public:
    vip_if* vip_if_in_1;

    vip_driver( uvm::uvm_component_name name ) : uvm::uvm_driver<REQ>(name)
    {// constructor
    }

    UVM_COMPONENT_PARAM_UTILS(vip_driver<REQ>)

    void build_phase(uvm::uvm_phase& phase) {
        // std::cout << sc_core::sc_time_stamp() << ": build_phase " << this->name() << std::endl;
        uvm::uvm_driver<REQ>::build_phase(phase);
        if (!uvm::uvm_config_db<vip_if*>::get(this, "*", "vip_if_in_1", vip_if_in_1))
            UVM_FATAL(this->name(), "Virtual interface not defined! Simulation aborted!");
    }

    void run_phase(uvm::uvm_phase& phase) {
        (void)phase;
        // std::cout << sc_core::sc_time_stamp() << ": " << this->name() << " " << phase.get_name() << "..." << std::endl;
        REQ req, rsp;

        while(true) { // execute all sequences
            this->seq_item_port->get_next_item(req);
            drive_transfer(req);
            this->seq_item_port->item_done();
        }
    }

    void drive_transfer(const REQ& p){       // Recebe o packet
        wait(1.0,sc_core::SC_PS);
        vip_if_in_1->tlm_socket.write_register(0,p);
    }
};
#endif /* vip_driver_H_ */
