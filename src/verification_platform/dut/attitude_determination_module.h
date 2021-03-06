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

#ifndef ATTITUDE_DETERMINATION_MODULE_H
#define ATTITUDE_DETERMINATION_MODULE_H
#include <systemc>
#include <uvm>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <ctime>

#include "src/attitude_determination.h"
#include "uvm/vip_if.h"

class AttitudeDeterminationModule : public uvm::uvm_component
{
public:
    // channels
    vip_if * vif_in;
    vip_if * vif_out;

    AttitudeDeterminationModule(uvm::uvm_component_name name, vip_if *in, vip_if *out);

    void build_phase(uvm::uvm_phase& phase);
    void run_phase(uvm::uvm_phase& phase);

private:
    st::AttitudeDetermination attitude_determination;
    StandardPacket output_packet;
};

#endif // ATTITUDE_DETERMINATION_MODULE_H
