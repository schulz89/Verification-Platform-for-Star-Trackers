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

#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
#include <tlm.h>
#include <uvm>

#include "vip_agent.h"
#include "scoreboard.h"
#include "sequence.h"
#include "standard_packet.h"

class testbench : public uvm::uvm_env{

public:
    // instances
    vip_agent* dut_agent;
    vip_agent* out_agent;
    scoreboard_base* scoreboard0;
//    uvm::uvm_scoreboard* scoreboard0;
    UVM_COMPONENT_UTILS(testbench)

    testbench( uvm::uvm_component_name name);

    void build_phase(uvm::uvm_phase& phase);
    void connect_phase(uvm::uvm_phase& phase);
};

#endif /* TESTBENCH_H_ */
