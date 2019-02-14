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

#ifndef TEST_BASE_H_
#define TEST_BASE_H_

#include <systemc>
#include <tlm>
#include <uvm>

#include "uvm/testbench.h"

class test_base : public uvm::uvm_test {

public:

    bool test_end;
    testbench* tb;

    test_base( uvm::uvm_component_name name );

    UVM_COMPONENT_UTILS(test_base)

    virtual void build_phase( uvm::uvm_phase& phase );
    virtual void run_phase( uvm::uvm_phase& phase );
    virtual void extract_phase( uvm::uvm_phase& phase );
    virtual void report_phase(uvm::uvm_phase& phase );

    virtual void overrides();
};

#endif /* TEST_BASE_H_ */
