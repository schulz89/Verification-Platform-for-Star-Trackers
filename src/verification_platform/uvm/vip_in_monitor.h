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

#ifndef VIP_IN_MONITOR_H_
#define VIP_IN_MONITOR_H_

#include <systemc>
#include <tlm>
#include <uvm>

#include "vip_if.h"
#include "standard_packet.h"

class vip_in_monitor : public uvm::uvm_monitor {

public:
    uvm::uvm_analysis_port<StandardPacket> item_collected_port; // connection to scoreboard
    vip_if * vif_monitor;

    StandardPacket packet;

    // bool   checks_enable;
    // bool   coverage_enable;

    vip_in_monitor(uvm::uvm_component_name name);
    void build_phase(uvm::uvm_phase& phase);
    void run_phase( uvm::uvm_phase& phase );

    UVM_COMPONENT_UTILS(vip_in_monitor)
};

#endif /* vip_in_monitor_H_ */
