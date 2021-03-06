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

#ifndef TEST_CENTROIDING_H_
#define TEST_CENTROIDING_H_

#include <systemc>
#include <tlm>
#include <uvm>

#include "uvm/test.h"
#include "test/centroiding/sequence.h"
#include "test/centroiding/scoreboard.h"

class test_centroiding : public test_base {

public:
    test_centroiding( uvm::uvm_component_name name );

    UVM_COMPONENT_UTILS(test_centroiding)

    virtual void overrides();
    virtual void run_phase( uvm::uvm_phase &phase );
    virtual void report_phase( uvm::uvm_phase& phase );
};

#endif /* TEST_CENTROIDING_H_ */
