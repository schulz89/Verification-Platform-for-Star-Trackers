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

#include "vip_in_monitor.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

vip_in_monitor::vip_in_monitor(uvm_component_name name)
    : uvm_monitor         (name)
    , item_collected_port ("item_collected_port")
    , vif_monitor         (0)
    // , checks_enable       (false)
    // , coverage_enable     (false)
{//constructor

}

void vip_in_monitor::build_phase(uvm_phase& phase) {

    // cout << sc_time_stamp() << ": build_phase " << name() << endl;
    uvm_monitor::build_phase(phase);

    if (!uvm_config_db<vip_if*>::get(this, "*", "vip_if_in_1", vif_monitor))
        UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");

    //if (!uvm_config_db<dut*>::get(this, "*", "my_dut", my_dut_1))
    //    UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");

    //uvm_config_db<bool>::get(this, "*", "checks_enable", checks_enable);
    //uvm_config_db<bool>::get(this, "*", "coverage_enable", coverage_enable);
}

void vip_in_monitor::run_phase( uvm_phase& phase ) {
    (void)phase;
    while (true) {
        wait(vif_monitor->tlm_target.ev);
        packet = vif_monitor->tlm_target.packet;

        item_collected_port.write(packet);

        //if(checks_enable) { cout << "no checks yet" << endl; }
        //if(coverage_enable) { cout << "no checks yet" << endl; }
    }
}
