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

#ifndef VIP_AGENT_H_
#define VIP_AGENT_H_

#include <systemc>
#include <tlm>
#include <uvm>

#include "vip_sequencer.h"
#include "vip_driver.h"
#include "vip_in_monitor.h"
#include "vip_out_monitor.h"
#include "standard_packet.h"

class vip_agent : public uvm::uvm_agent{

public:

    vip_sequencer<StandardPacket> *dut_sequencer;
    vip_driver<StandardPacket> *dut_driver;
    vip_out_monitor *monitor;
    vip_in_monitor *monitor_dut_in;

    UVM_COMPONENT_UTILS(vip_agent)

    vip_agent( uvm::uvm_component_name name );
    void build_phase(uvm::uvm_phase& phase);
    void connect_phase(uvm::uvm_phase& phase);

};

#endif /* VIP_AGENT_H_ */
