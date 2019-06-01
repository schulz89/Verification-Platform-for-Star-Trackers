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

#ifndef SCOREBOARD_COMMON_H
#define SCOREBOARD_COMMON_H

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <boost/qvm/all.hpp>
#include "standard_structures.h"

class SBCentroiding
{
public:
    SBCentroiding();
    void compare_data(std_str::Sky sky_in, std_str::Sky sky_out); // Do an iteration
    void print();
    void reset();

    int identified_acc;
    int total_acc;
    int correct_acc;
    double error_acc;
    double time_acc;
    double time_c_acc;

private:
    cv::Mat sky_to_mat(std_str::Sky input);
    void compare_centroids(int &correct,         // out
                           double &error,        // out
                           std_str::Sky sky_in,  // in
                           std_str::Sky sky_out, // in
                           float max_dist);      // in
};

class SBStarId
{
public:
    SBStarId();
    void compare_data (std_str::Sky sky_in, std_str::Sky sky_out);
    void print();
    void reset();

    int correct;
    int identified;
    int total;
    double time;

    int identified_acc;
    int total_acc;
    int correct_acc;
    double time_acc;
private:
    int compare_id_stars(std_str::Sky sky_in, std_str::Sky sky_out);
};

class SBAttitude
{
public:
    SBAttitude();
    void compare_data (std_str::Sky sky_in, std_str::Sky sky_out);
    void print();
    void reset();

    int correct;
    int rejected;
    int wrong;
    double error;

    int correct_acc;
    int rejected_acc;
    int total_acc;
    int wrong_acc;
    double error_acc;
    double time_acc;

private:
    double q_dist(std_str::Quaternion q1, std_str::Quaternion q2);
};

#endif // SCOREBOARD_COMMON_H
