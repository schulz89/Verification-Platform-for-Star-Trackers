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

#include "centroiding_vhdl_module.h"

using namespace std;
using namespace std::chrono;
using namespace cv;
using namespace uvm;
using namespace sc_core;
using namespace std_str;
using namespace st;

centroiding_vhdl_module::centroiding_vhdl_module(uvm_component_name name, vip_if *in, vip_if *out) : uvm_component(name)
{
    vif_in = in;
    vif_out = out;

    double dtmp;
    FileStorage fs("config/dut.yml", FileStorage::READ);
    fs["gb_threshold"]   >> dtmp; cfg_threshold = (int)dtmp;
    fs["gb_cdpu_count"]  >> dtmp; cfg_cdpu_count = (int)dtmp;
    fs["gb_kalman_gain"] >> cfg_kalman_gain;
    fs.release();

    star_filter = new StarFilterHW(cfg_threshold);
    centroider = new Centroider(cfg_cdpu_count);
}

centroiding_vhdl_module::~centroiding_vhdl_module()
{
    //delete star_filter;
    delete centroider;
}

void centroiding_vhdl_module::build_phase(uvm_phase &phase)
{
    // cout << sc_time_stamp() << ": build_phase " << name() << endl;
    uvm_component::build_phase(phase);
    // if (!uvm_config_db<vip_if*>::get(this, "*", "vip_if_in_1", vif_in))
    //     UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");
    // if (!uvm_config_db<vip_if*>::get(this, "*", "vip_if_out_1", vif_out))
    //     UVM_FATAL(name(), "Virtual interface not defined! Simulation aborted!");
}

void centroiding_vhdl_module::run_phase(uvm_phase& phase)
{
    uvm_component::run_phase(phase);
    while(true){
        wait(vif_in->tlm_target.ev);
        Sky gsky = vif_in->tlm_target.packet.sky_str;

        // Mat color_image;
        // cvtColor(gsky.image, color_image, cv::COLOR_GRAY2BGR);

        auto t0 = high_resolution_clock::now();

        vector<StarPixel> star_pixels = star_filter->GetStarPixels(gsky.image);

        auto t1 = high_resolution_clock::now();

        vector<Centroid> centroids = centroider->ComputeFromList(star_pixels, cfg_kalman_gain);

        auto t2 = high_resolution_clock::now();

        duration<double> elapsed_seconds = t2-t0;
        duration<double> c_time = t2 - t1;        // time without threshold;
        Sky csky;
        for(auto &i : centroids){
            Star s;
            s.centroid.x = i.x;
            s.centroid.y = i.y;
            s.mag = i.value;
            csky.stars.push_back(s);
        }
        csky.image = gsky.image;
        csky.time.push_back(elapsed_seconds.count());
        csky.time.push_back(c_time.count());

        output_packet.sky_str = csky;
        vif_out->tlm_socket.write_register(0,output_packet);
    }
}
