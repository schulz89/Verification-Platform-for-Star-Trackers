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

#ifndef SEQUENCE_CENTROIDING_H_
#define SEQUENCE_CENTROIDING_H_

#include <iostream>
#include <systemc>
#include <tlm>
#include <uvm>
#include <opencv2/core/core.hpp>

#include "star_simulator/starsimulator.h"
#include "uvm/sequence.h"

template <typename REQ = uvm::uvm_sequence_item, typename RSP = REQ>
class sequence_centroiding : public sequence_base<REQ,RSP> {

public:
    sequence_centroiding( const std::string& name )
        : sequence_base<REQ,RSP>( name )
    {
        // std::cout << sc_core::sc_time_stamp() << ": constructor " << name << std::endl;

        // this->ssim.config.optical_parameters.resolution.x = 800;
        // this->ssim.config.optical_parameters.resolution.y = 600;
        // this->ssim.config.simulator_parameters.psf_C = 50000;
        // this->ssim.reload();

        // Custom simulator configuration
        // this->ssim.config.simulator_parameters.position_std_dev   = 0;   // in px
        // this->ssim.config.simulator_parameters.mag_std_dev        = 0.5;   // in mag
        // this->ssim.config.simulator_parameters.background_std_dev = 4;   // 0-255
        // this->ssim.config.simulator_parameters.background_dc      = 2;   // 0-255
    }

    UVM_OBJECT_PARAM_UTILS(sequence_centroiding<REQ,RSP>)
};

#endif /* SEQUENCE_CENTROIDING_H_ */
