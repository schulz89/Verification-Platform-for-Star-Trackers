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

test_star_tracker::test_star_tracker( uvm_component_name name )
    : test_base( name )
{// constructor
}

void test_star_tracker::overrides()
{
    set_type_override_by_type(scoreboard_base::get_type(),scoreboard_star_tracker::get_type());
}

void test_star_tracker::run_phase( uvm_phase& phase ){
    (void)phase;
    // UVM_INFO( get_name(), "** UVM TEST STARTED **", UVM_NONE );
    cout << endl;
}

void test_star_tracker::report_phase(uvm_phase& phase ){
    (void)phase;
    scoreboard_star_tracker *scoreboard = (scoreboard_star_tracker*)tb->scoreboard0;

    cout << endl;
    cout << "Runtime:" << endl;
    cout << "centroid time, total (s)        = " << scoreboard->acc_centr.time_acc << endl;
    cout << "centroid time, no threshold (s) = " << scoreboard->acc_centr.time_c_acc << endl;
    cout << "star id time (s)                = " << scoreboard->acc_starid.time_acc<< endl;
    cout << "attitude determination time (s) = " << scoreboard->acc_att.time_acc << endl;
    cout << "correct attitude ratio          = " << (double)scoreboard->acc_att.correct_acc/scoreboard->acc_att.total_acc << endl;
    cout << "rejected attitude ratio         = " << (double)scoreboard->acc_att.rejected_acc/scoreboard->acc_att.total_acc << endl;
    cout << "wrong attitude ratio            = " << (double)scoreboard->acc_att.wrong_acc/scoreboard->acc_att.total_acc << endl;
    cout << endl;

//    scoreboard->acc_centr.print();
//    scoreboard->acc_starid.print();
//    scoreboard->acc_att.print();

    if ( test_end ){
        // UVM_INFO( get_name(), "** UVM TEST PASSED **", UVM_NONE );
        sc_stop();
    }
    else
        UVM_ERROR( get_name(), "** UVM TEST FAILED **" );
}
