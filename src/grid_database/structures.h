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

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <boost/qvm/all.hpp>

struct catalog_parameters {
    std::string hipparcos;
    std::string hipparcos2;
    int hipparcos_size;
    int epoch;
};

struct optical_parameters {
    cv::Point2i resolution;
    cv::Point2d pixel_size;
    cv::Point2d sensor_size;
    double      focus;
    cv::Point2i center;
    double      max_mag;
};

struct grid_parameters {
    int    g;
    int    pattern_radius; // unit: px
    double buffer_radius;  // unit: px
    int    confidence_factor;
    int    expected_false_stars;
    int    minimum_match;
    double error;
};

struct database_parameters {
    double pattern_radius; // unit: rad
    double buffer_radius;  // unit: rad
    double min_dist;
    double relevance_radius;
};

struct hip{
    // Catalog entry (hipparcos)
    double Vmag;
    int    Proxy;
    int    VarFlag;
};

struct hip2{
    // Catalog entry (hipparcos 2)
    int    HIP;
    double RArad;
    double DErad;
    double pmRA;
    double pmDE;
};

struct db {
    // Database entry
    int    id;
    double mag;
    int    proxy_flag;
    int    var_flag;
    int    nn;
    double angle_nn;
    double angle_2d;
    boost::qvm::vec<double,3> uv;
    std::vector<int> cells_descriptor;
    std::vector<unsigned long> bit_descriptor;
};

#endif // STRUCTURES_H
