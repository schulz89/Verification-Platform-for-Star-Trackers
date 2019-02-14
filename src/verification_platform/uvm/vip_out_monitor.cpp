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

#include "vip_out_monitor.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

vip_out_monitor::vip_out_monitor(uvm_component_name name)
    : uvm_monitor(name)
    , item_collected_port("item_collected_port")
    // , checks_enable(false)
    // , coverage_enable(false)
{// constructor

}

void vip_out_monitor::build_phase(uvm_phase& phase) {
    (void)phase;
    // cout << sc_time_stamp() << ": build_phase " << name() << endl;

    if (!uvm_config_db<vip_if*>::get(this, "*", "vip_if_out_1", vif_monitor_out)) // Connection dut -> monitor
        UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");

    // uvm_config_db<bool>::get(this, "*", "checks_enable", checks_enable);
    // uvm_config_db<bool>::get(this, "*", "coverage_enable", coverage_enable);
}

void vip_out_monitor::run_phase( uvm_phase& phase ) {
    (void)phase;
    StandardPacket p;

    while (true) {
        wait(vif_monitor_out->tlm_target.ev);
        p = vif_monitor_out->tlm_target.packet;
        item_collected_port.write(p);
        // if(checks_enable) { cout << "no checks yet" << endl; }
        // if(coverage_enable) { cout << "no checks yet" << endl; }
    }
}
