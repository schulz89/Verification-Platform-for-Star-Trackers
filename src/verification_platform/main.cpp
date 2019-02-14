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

#include <iostream>
#include <systemc>
#include <tlm>

#include "dut/centroiding_module.h"
#include "dut/centroiding_vhdl_module.h"
#include "dut/centroiding_cpp_module.h"
#include "dut/star_identification_module.h"
#include "dut/attitude_determination_module.h"
#include "dut/star_tracker_module.h"
#include "dut/tcp_wrapper_module.h"

#include "uvm/test.h"
#include "uvm/vip_if.h"

using namespace std;
using namespace sc_core;
using namespace uvm;
using namespace st;

enum dut_t {CENTROIDING_REGION_GROWING, // 0
            CENTROIDING_GABRIEL_CPP,    // 1
            CENTROIDING_GABRIEL_VHDL,   // 2
            STAR_ID_GRID,               // 3
            ATTITUDE_DETERMINATION,     // 4
            STAR_TRACKER,               // 5
            TCP_WRAPPER};               // 6

#define ENUM_DUT_T_SIZE 7

enum test_t {TEST_CENTROIDING,          // 0
             TEST_STAR_ID,              // 1
             TEST_ATTITUDE,             // 2
             TEST_STAR_TRACKER,         // 3
             TEST_STAR_TRACKER_IMG};    // 4

#define ENUM_TEST_T_SIZE 5

int sc_main(int argc, char * argv[])
{
    dut_t dut_sel   = STAR_TRACKER;
    test_t test_sel = TEST_STAR_TRACKER;

    if(argc == 3){
        dut_sel = (dut_t) atoi(argv[1]);
        test_sel = (test_t) atoi(argv[2]);
    }

    if(dut_sel >= ENUM_DUT_T_SIZE)   exit(1);
    if(test_sel >= ENUM_TEST_T_SIZE) exit(1);

    //uvm_config_db_options::turn_on_tracing();

    // Create the interfaces and add them to the database.
    vip_if *vip_if_in_1 = new vip_if;
    vip_if *vip_if_out_1 = new vip_if;
    uvm::uvm_config_db<vip_if*>::set(nullptr, "*", "vip_if_in_1", vip_if_in_1);
    uvm::uvm_config_db<vip_if*>::set(nullptr, "*", "vip_if_out_1", vip_if_out_1);

    uvm_component *my_dut = nullptr;

    switch (dut_sel) {
    case CENTROIDING_REGION_GROWING: {
        my_dut = new CentroidingModule("dut_centroiding", vip_if_in_1, vip_if_out_1);
    } break;
    case CENTROIDING_GABRIEL_CPP: {
        my_dut = new centroiding_cpp_module("dut_centroiding_cpp", vip_if_in_1, vip_if_out_1);
    } break;
    case CENTROIDING_GABRIEL_VHDL: {
        my_dut = new centroiding_vhdl_module("dut_centroiding_vhdl", vip_if_in_1, vip_if_out_1);
    } break;
    case STAR_ID_GRID: {
        my_dut = new StarIdentificationModule("dut_star_id", vip_if_in_1, vip_if_out_1);
    } break;
    case ATTITUDE_DETERMINATION: {
        my_dut = new AttitudeDeterminationModule("dut_attitude", vip_if_in_1, vip_if_out_1);
    } break;
    case STAR_TRACKER: {
        my_dut = new StarTrackerModule("dut_star_tracker", vip_if_in_1, vip_if_out_1);
    } break;
    case TCP_WRAPPER: {
        my_dut = new TcpWrapperModule("dut_tcp_wrapper", vip_if_in_1, vip_if_out_1);
    } break;
    }

    switch (test_sel) {
    case TEST_CENTROIDING : {
        run_test("test_centroiding");
    } break;
    case TEST_STAR_ID : {
        run_test("test_star_id");
    } break;
    case TEST_ATTITUDE : {
        run_test("test_attitude");
    } break;
    case TEST_STAR_TRACKER : {
        run_test("test_star_tracker");
    } break;
    case TEST_STAR_TRACKER_IMG : {
        run_test("test_star_tracker_img");
    } break;
    }

    delete my_dut;
    return 0;
}
