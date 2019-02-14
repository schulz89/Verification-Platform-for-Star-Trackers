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

#include "testbench.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

testbench::testbench( uvm_component_name name) : uvm_env(name) {
    // constructor
    // cout << sc_time_stamp() << ": constructor " << name << endl;
}

void testbench::build_phase(uvm_phase& phase){
    // cout << sc_time_stamp() << ": build_phase " << name() << endl;

    uvm_env::build_phase(phase);

    dut_agent = vip_agent::type_id::create("dut_agent", this);
    assert(dut_agent);

    out_agent = vip_agent::type_id::create("out_agent", this);
    assert(out_agent);

    scoreboard0 = scoreboard_base::type_id::create("scoreboard0", this);
    assert(scoreboard0);

    uvm_config_db<int>::set(this, "dut_agent", "is_active", UVM_ACTIVE);
    uvm_config_db<int>::set(this, "out_agent", "is_active", UVM_PASSIVE);
}

void testbench::connect_phase(uvm_phase& phase){
    (void)phase;
    // cout << sc_time_stamp() << ": connect_phase " << name() << endl;
    dut_agent->monitor_dut_in->item_collected_port.connect(scoreboard0->xmt_listener_imp);
    out_agent->monitor->item_collected_port.connect(scoreboard0->rcv_listener_imp);
}
