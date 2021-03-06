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

#include "star_identification_module.h"

using namespace std;
using namespace std::chrono;
using namespace cv;
using namespace uvm;
using namespace sc_core;
using namespace st;
using namespace std_str;

StarIdentificationModule::StarIdentificationModule(uvm_component_name name, vip_if *in, vip_if *out) : uvm_component(name)
{
    vif_in = in;
    vif_out = out;
    star_identification = new StarIdentification();
}

StarIdentificationModule::~StarIdentificationModule()
{
    delete star_identification;
}

void StarIdentificationModule::build_phase(uvm_phase &phase)
{
    // cout << sc_time_stamp() << ": build_phase " << name() << endl;
    uvm_component::build_phase(phase);
    // if (!uvm_config_db<vip_if*>::get(this, "*", "vip_if_in_1", vif_in))
    //     UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");
    // if (!uvm_config_db<vip_if*>::get(this, "*", "vip_if_out_1", vif_out))
    //     UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");
}

void StarIdentificationModule::run_phase(uvm_phase& phase)
{
    uvm_component::run_phase(phase);
    while(true){
        wait(vif_in->tlm_target.ev);
        Sky gsky = vif_in->tlm_target.packet.sky_str;
        auto start = high_resolution_clock::now();
        Sky isky = star_identification->identifyStars(gsky);
        auto end = high_resolution_clock::now();
        duration<double> elapsed_seconds = end-start;
        isky.time.push_back(elapsed_seconds.count());
        output_packet.sky_str = isky;
        vif_out->tlm_socket.write_register(0,output_packet);
    }
}
