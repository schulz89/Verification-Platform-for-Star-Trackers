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

#include "test.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

test_star_tracker_img::test_star_tracker_img( uvm_component_name name )
    : test_base( name )
{// constructor
    // DUT configuration:
    uvm::uvm_component *dut;
    uvm::uvm_config_db<uvm::uvm_component*>::get(this, "*.tb.*", "dut_star_tracker", dut);
    StarTrackerModule *star_tracker = (StarTrackerModule*)dut;
    CentroidingModule *centroiding_module = (CentroidingModule*)star_tracker->centroiding;
    centroiding_module->centroiding.config.center_parameters.threshold = 90;
    centroiding_module->centroiding.config.center_parameters.relaxed_threshold = 70;
    centroiding_module->centroiding.config.draw = true;
}

void test_star_tracker_img::overrides()
{
    set_type_override_by_type(sequence_base<StandardPacket>::get_type(),sequence_star_tracker_img<StandardPacket>::get_type());
    set_type_override_by_type(scoreboard_base::get_type(),scoreboard_star_tracker_img::get_type());
}

void test_star_tracker_img::run_phase( uvm_phase& phase ){
    (void)phase;
    // UVM_INFO( get_name(), "** UVM TEST STARTED **", UVM_NONE );
    cout << endl;
}

void test_star_tracker_img::report_phase(uvm_phase& phase ){
    (void)phase;
    scoreboard_star_tracker_img *scoreboard = (scoreboard_star_tracker_img*)tb->scoreboard0;

    cout << endl;
    cout << "centroid time (s) = " << scoreboard->time_centroiding << endl;
    cout << "star id time (s) = " << scoreboard->time_star_id << endl;
    cout << "attitude determination time (s) = " << scoreboard->time_attitude_determination << endl;
    cout << endl;

    if ( test_end ){
        // UVM_INFO( get_name(), "** UVM TEST PASSED **", UVM_NONE );
        sc_stop();
    }
    else
        UVM_ERROR( get_name(), "** UVM TEST FAILED **" );
}
