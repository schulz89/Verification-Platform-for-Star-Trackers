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

#ifndef STAR_TRACKER_MODULE_H
#define STAR_TRACKER_MODULE_H
#include <systemc>
#include <uvm>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "centroiding_module.h"
#include "centroiding_cpp_module.h"
#include "star_identification_module.h"
#include "attitude_determination_module.h"

#include "uvm/vip_if.h"

class StarTrackerModule : public uvm::uvm_component
{
public:
    // channels
    vip_if * vif_in;  // To be recovered from the database.
    vip_if * vif_out; // To be recovered from the database.

    vip_if vif_block_0_1; // Instantiated here.
    vip_if vif_block_1_2; // Instantiated here.

    StarTrackerModule(uvm::uvm_component_name name, vip_if *in, vip_if *out);

    void build_phase(uvm::uvm_phase& phase);
    void run_phase(uvm::uvm_phase& phase);

    uvm_component *centroiding;
    uvm_component *star_id;
    uvm_component *attitude;

private:
    StandardPacket output_packet;
};

#endif // STAR_TRACKER_MODULE_H
