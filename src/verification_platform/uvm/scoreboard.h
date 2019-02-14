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

#ifndef SCOREBOARD_BASE_H_
#define SCOREBOARD_BASE_H_

#include <systemc>
#include <uvm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>

#include "uvm/xmt_subscriber.h"
#include "uvm/rcv_subscriber.h"
#include "uvm/standard_packet.h"
#include "standard_structures.h"

class scoreboard_base : public uvm::uvm_scoreboard{

public:
    bool end_test = false;
    std_str::Sky sky_in;
    std_str::Sky sky_out;

    uvm::uvm_analysis_export<StandardPacket> xmt_listener_imp;
    uvm::uvm_analysis_export<StandardPacket> rcv_listener_imp;

    xmt_subscriber* xmt_listener;
    rcv_subscriber* rcv_listener;

    scoreboard_base( uvm::uvm_component_name name );
    ~scoreboard_base();

    UVM_COMPONENT_UTILS(scoreboard_base)

    void build_phase(uvm::uvm_phase& phase);
    void connect_phase(uvm::uvm_phase& phase);
    void write_xmt(const StandardPacket& in_p);
    void write_rcv(const StandardPacket& out_p);
    virtual void compare_data ();

    std::ofstream *ofs;
};

#endif /* SCOREBOARD_BASE_H_ */
