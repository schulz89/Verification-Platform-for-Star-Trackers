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

#ifndef STARSIMULATOR_H
#define STARSIMULATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <math.h>
#include <limits>
#include <algorithm>
#include <random>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/qvm/all.hpp>
#include "structures_ssim.h"
#include "standard_structures.h"

namespace ssim {

class StarSimulator
{
public:
    StarSimulator();
    std_str::Sky generate_sky ();
    std_str::Sky generate_sky (std_str::Quaternion q);
    void reload();

    Configuration config;

private:
    boost::qvm::quat<double> random_quaternion();
    std::vector<DB> projected_fov(boost::qvm::quat<double> q);
    cv::Mat build_image(std::vector<DB> star_list);
    void load_config(std::string filename);
    std::vector<DB> initialize_database();
    void proper_motion_correction (Hip2 entry, int time_y, DB *output);
    std::vector<DB> visible_stars(std::vector<DB> &database);
    double vangle (boost::qvm::vec<double,3> p1, boost::qvm::vec<double,3> p2);
    boost::qvm::quat<double> rot_quat (boost::qvm::vec<double,3> v1, boost::qvm::vec<double,3> v2);
    std::vector<DB> rotate3d(std::vector<DB> visible_stars, boost::qvm::quat<double> q);
    void to_pixel_coordinates (std::vector<DB> &star_list);
    std::vector<DB> relevant_fov(std::vector<DB> &lf_list);
    boost::qvm::vec<double,3> random_unit_vector ();
    double random_angle ();
    double awgn(double standard_deviation);
    DB fake_star();

    std::vector<DB> database;
    std::vector<DB> vstars;
    std::default_random_engine rng_quaternion;
    std::default_random_engine rng_noise;
    std::default_random_engine rng_fake_star;
};

}

#endif // STARSIMULATOR_H
