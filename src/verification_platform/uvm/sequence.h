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

#ifndef SEQUENCE_BASE_H_
#define SEQUENCE_BASE_H_

#include <systemc>
#include <tlm>
#include <uvm>

#include "star_simulator/starsimulator.h"

template <typename REQ = uvm::uvm_sequence_item, typename RSP = REQ>
class sequence_base : public uvm::uvm_sequence<REQ,RSP> {

public:
    ssim::StarSimulator ssim;

    REQ* req = new REQ();
    RSP* rsp = new RSP();
    //scv_smart_ptr< sc_dt::sc_uint<8> > data;

    sequence_base( const std::string& name )
        : uvm::uvm_sequence<REQ,RSP>( name )
    {
        // std::cout << sc_core::sc_time_stamp() << ": constructor " << name << std::endl;
        cv::FileStorage fs("config/common.yml", cv::FileStorage::READ);
        fs["vp_n_tests"] >> n_tests;
        fs.release();
    }

    UVM_OBJECT_PARAM_UTILS(sequence_base<REQ,RSP>)

    void pre_body(){
        // Code
        if(this->starting_phase != NULL)
            this->starting_phase->raise_objection(this);            // raise objection if started as a root sequence
    }

    void body() {

        // UVM_INFO(this->get_name(), "Starting sequence_dut", uvm::UVM_MEDIUM);
        /**
         * Permanesce aqui até que seja realizada todas sequências
         * conforme a função new_sequnce()
         */
        sequence();
        // UVM_INFO(this->get_name(), "Finishing sequence_dut", uvm::UVM_MEDIUM);
    }

    void post_body(){
        // drop objection if started as a root sequence
        if(this->starting_phase != NULL)
            this->starting_phase->drop_objection(this);
    }

    virtual void sequence(){
        for(int i=0;i<(int)n_tests;i++){
            this->req->sky_str = this->ssim.generate_sky();
            this->start_item(this->req);
            this->finish_item(this->req);
            //this->get_response(rsp);
        }
    }

private:
    double n_tests = 0;
};

#endif /* SEQUENCE_BASE_H_ */
