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

test_attitude::test_attitude( uvm_component_name name )
    : test_base( name )
{// constructor
}

void test_attitude::overrides()
{
    set_type_override_by_type(scoreboard_base::get_type(),scoreboard_attitude::get_type());
}

void test_attitude::run_phase( uvm_phase& phase ){
    (void)phase;
    // UVM_INFO( get_name(), "** UVM TEST STARTED **", UVM_NONE );
    cout << endl;
}

void test_attitude::report_phase(uvm_phase& phase ){
    (void)phase;
    scoreboard_attitude *scoreboard = (scoreboard_attitude*)tb->scoreboard0;

    cout << endl;
    cout << "total          = " << scoreboard->acc.total_acc                                      << endl;
    cout << "correct ratio  = " << (double)scoreboard->acc.correct_acc/scoreboard->acc.total_acc  << endl;
    cout << "rejected ratio = " << (double)scoreboard->acc.rejected_acc/scoreboard->acc.total_acc << endl;
    cout << "wrong ratio    = " << (double)scoreboard->acc.wrong_acc/scoreboard->acc.total_acc    << endl;
    cout << "error std      = " << scoreboard->acc.error_acc / scoreboard->acc.correct_acc        << endl;
    cout << "total time (s) = " << scoreboard->acc.time_acc                                       << endl;
    cout << endl;

    if ( test_end ){
        // UVM_INFO( get_name(), "** UVM TEST PASSED **", UVM_NONE );
        sc_stop();
    }
    else
        UVM_ERROR( get_name(), "** UVM TEST FAILED **" );
}
