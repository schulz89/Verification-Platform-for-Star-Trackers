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

#ifndef SEQUENCE_STAR_TRACKER_IMG_H_
#define SEQUENCE_STAR_TRACKER_IMG_H_

#include <iostream>
#include <cstring>
#include <systemc>
#include <tlm>
#include <uvm>

#include <opencv2/imgproc/imgproc.hpp>

#include "star_simulator/starsimulator.h"
#include "uvm/sequence.h"
#include "dut/star_tracker_module.h"

template <typename REQ = uvm::uvm_sequence_item, typename RSP = REQ>
class sequence_star_tracker_img : public sequence_base<REQ,RSP> {

public:
    sequence_star_tracker_img( const std::string& name )
        : sequence_base<REQ,RSP>( name )
    {

    }

    UVM_OBJECT_PARAM_UTILS(sequence_star_tracker_img<REQ,RSP>)

    virtual void sequence(){
        // Hard coded paths are ugly
        const std::string input_path = "/home/schulz/tmp/ptest/input_undist_cropped";
        const std::string input_quaternions_path = "/home/schulz/tmp/ptest/quaternions_ref.csv";
        FILE *qfile;
        qfile = fopen(input_quaternions_path.c_str(),"r");
        std::string quaternion;
        while(!feof(qfile)){
            // Building Sky standard structure..
            std_str::Sky sky;
            int num;
            fscanf(qfile,"%d,%lf,%lf,%lf,%lf",&num,&sky.q.r,&sky.q.v[0],&sky.q.v[1],&sky.q.v[2]);
            std::string filename = std::to_string(num) + ".png";
            std::string ipath = input_path + "/" + filename;
            cv::Mat image_color = cv::imread(ipath);
            cv::cvtColor(image_color, sky.image, CV_BGR2GRAY);
            // Done, doing a request...
            //this->req->sky_str = this->ssim.generate_sky(sky.q);
            //this->req->sky_str = this->ssim.generate_sky();
            this->req->sky_str = sky;
            this->start_item(this->req);
            this->finish_item(this->req);
            //this->get_response(rsp);
            //cv::imshow("Sequence image", sky.image); cv::waitKey(0);
        }
    }
};

#endif /* SEQUENCE_STAR_TRACKER_IMG_H_ */
