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

#ifndef STANDARD_STRUCTURES_H
#define STANDARD_STRUCTURES_H

#include <vector>
#include <opencv2/core.hpp>

namespace std_str {

struct Star {
    int id = 0;               // star id from Hipparcos-2
    double mag;               // star brightness or magnitude
    cv::Point2d centroid;     // star centroid in camera coordinates (px)
    cv::Point3d uv_camera;    // star unit vector in camera coordinates
    cv::Point3d uv_inertial;  // star unit vector in inertial coordinates
};

struct Quaternion {           // attitude quaternion
    double r;
    double v[3];
};

struct Sky {
    cv::Mat image;            // sky image
    std::vector<Star> stars;  // stars present in image
    Quaternion q;             // attitude quaternion
    std::vector<double> time; // cpu time
};

}

#endif // STANDARD_STRUCTURES_H
