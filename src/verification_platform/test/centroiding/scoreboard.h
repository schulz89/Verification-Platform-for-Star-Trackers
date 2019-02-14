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

#ifndef SCOREBOARD_CENTROIDING_H_
#define SCOREBOARD_CENTROIDING_H_

#include <systemc>
#include <uvm>
#include <iostream>
#include <iomanip>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "uvm/xmt_subscriber.h"
#include "uvm/rcv_subscriber.h"
#include "uvm/standard_packet.h"
#include "uvm/scoreboard.h"
#include "standard_structures.h"
#include "test/common/scoreboard_common.h"

#include "scoreboard.h"

class scoreboard_centroiding : public scoreboard_base{

public:
    scoreboard_centroiding( uvm::uvm_component_name name );
    UVM_COMPONENT_UTILS(scoreboard_centroiding) // なければ動かない。
    virtual void compare_data ();
    SBCentroiding acc;

private:

};

#endif /* SCOREBOARD_CENTROIDING_H_ */
