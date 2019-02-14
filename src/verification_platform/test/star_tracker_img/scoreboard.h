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

#ifndef SCOREBOARD_STAR_TRACKER_IMG_H_
#define SCOREBOARD_STAR_TRACKER_IMG_H_

#include <systemc>
#include <uvm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <sys/stat.h>

#include "uvm/xmt_subscriber.h"
#include "uvm/rcv_subscriber.h"
#include "uvm/standard_packet.h"
#include "uvm/scoreboard.h"
#include "standard_structures.h"

#include "scoreboard.h"

class scoreboard_star_tracker_img : public scoreboard_base{

public:
    scoreboard_star_tracker_img( uvm::uvm_component_name name );
    ~scoreboard_star_tracker_img();
    virtual void compare_data ();
    UVM_COMPONENT_UTILS(scoreboard_star_tracker_img) // なければ動かない。

    double time_centroiding;
    double time_star_id;
    double time_attitude_determination;

private:
};

#endif /* SCOREBOARD_STAR_TRACKER_IMG_H_ */
