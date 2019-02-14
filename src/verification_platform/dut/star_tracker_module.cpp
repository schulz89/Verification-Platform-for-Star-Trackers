// Copyright 2019 Victor Hugo Schulz

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "star_tracker_module.h"

using namespace std;
using namespace uvm;
using namespace cv;
using namespace sc_core;
using namespace st;
using namespace std_str;

StarTrackerModule::StarTrackerModule(uvm_component_name name, vip_if *in, vip_if *out) : uvm_component(name)
{
    centroiding = new CentroidingModule          ("dut_centroiding", in,             &vif_block_0_1);
//    centroiding = new centroiding_cpp_module     ("dut_centroiding", in,             &vif_block_0_1);
    star_id     = new StarIdentificationModule   ("dut_star_id",     &vif_block_0_1, &vif_block_1_2);
    attitude    = new AttitudeDeterminationModule("dut_attitude",    &vif_block_1_2, out           );
}

void StarTrackerModule::build_phase(uvm_phase &phase)
{
    // cout << sc_time_stamp() << ": build_phase " << name() << endl;
    uvm_component::build_phase(phase);
}

void StarTrackerModule::run_phase(uvm_phase& phase)
{
    uvm_component::run_phase(phase);
}
