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

#include "test.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

test_base::test_base( uvm_component_name name )
    : uvm_test( name )
    , test_end(false)
    , tb(0)
{// constructor

}

void test_base::build_phase( uvm_phase& phase ){

    uvm_test::build_phase(phase);

    tb = testbench::type_id::create("tb",this);
    assert(tb);

    uvm_config_db<uvm_object_wrapper*>
            ::set( this, "tb.dut_agent.dut_sequencer.run_phase","default_sequence", sequence_base<StandardPacket>::type_id::get());
    overrides(); // Call overrides.
}

void test_base::run_phase( uvm_phase& phase ){
    (void)phase;
    // UVM_INFO( get_name(), "** UVM TEST STARTED **", UVM_NONE );
}

void test_base::extract_phase( uvm_phase& phase ){
    (void)phase;
    if ( tb->scoreboard0->end_test){

        // Post-processing of scoreboard data
        // cout << "Final do testes " << endl;
    }
    test_end = true;
}

void test_base::report_phase(uvm_phase& phase ){
    (void)phase;
    cout << "This is the report phase of the (empty) base test class." << endl;
    cout << "This is supposed to be replaced by a real test." << endl;
    if ( test_end ){
        // UVM_INFO( get_name(), "** UVM TEST PASSED **", UVM_NONE );
        sc_stop();
    }
    else
        UVM_ERROR( get_name(), "** UVM TEST FAILED **" );
}

void test_base::overrides()
{
    // No overrides
}
