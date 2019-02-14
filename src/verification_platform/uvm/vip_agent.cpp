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

#include "vip_agent.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

vip_agent::vip_agent( uvm_component_name name )
    : uvm_agent(name)
    , dut_sequencer(0)
    , dut_driver(0)
    , monitor(0)
    , monitor_dut_in(0)
{
    // cout << sc_time_stamp() << ":vip_agent constructor " << name << endl;
}

void vip_agent::build_phase(uvm_phase& phase){
    // cout << sc_time_stamp() << ":vip_agent build_phase " << name() << endl;

    uvm_agent::build_phase(phase);

    if (get_is_active() == UVM_ACTIVE){
        // UVM_INFO(get_name(), "is set to UVM_ACTIVE", UVM_NONE);

        if(get_name() == "dut_agent"){

            dut_sequencer = vip_sequencer<StandardPacket>::type_id::create("dut_sequencer", this);
            assert(dut_sequencer);

            dut_driver = vip_driver<StandardPacket>::type_id::create("dut_driver", this);
            assert(dut_driver);

            monitor_dut_in = vip_in_monitor::type_id::create("monitor_dut_in", this);
            assert(monitor_dut_in);

        }

    }else{
        // UVM_INFO(get_name(), "is set to UVM_PASSIVE", UVM_NONE);
        monitor = vip_out_monitor::type_id::create("monitor", this);
        assert(monitor);
    }
}

void vip_agent::connect_phase(uvm_phase& phase){
    (void)phase;
    // cout << sc_time_stamp() << ": connect_phase " << name() << endl;

    if (get_is_active() == UVM_ACTIVE){

        if(get_name() == "dut_agent"){
            dut_driver->seq_item_port.connect(dut_sequencer->seq_item_export);
        }

    }
}
