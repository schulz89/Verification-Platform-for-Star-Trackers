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

#ifndef STRUCTURES_SSIM_H
#define STRUCTURES_SSIM_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <boost/qvm/all.hpp>

namespace ssim{

struct CatalogParameters {
    std::string hipparcos;
    std::string hipparcos2;
    int hipparcos_size;
    int epoch;
};

struct OpticalParameters {
    cv::Point2i resolution;
    cv::Point2d pixel_size;
    cv::Point2d sensor_size;
    double      focus;
    cv::Point2i center;
    double      max_mag;
};

struct SimulatorParameters{
    double psf_B;
    double psf_C;
    double psf_sigma;
    double psf_roi;
    double position_std_dev;
    double mag_std_dev;
    double background_std_dev;
    double background_dc;
    double focus_std_dev;
    double focus_dc;
    int    false_stars;
    bool   build_image;
};

struct Configuration {
    CatalogParameters catalog_parameters;
    OpticalParameters optical_parameters;
    SimulatorParameters simulator_parameters;
};

struct Hip{
    // Catalog entry (hipparcos)
    double Vmag;
    int    Proxy;
    int    VarFlag;
};

struct Hip2{
    // Catalog entry (hipparcos 2)
    int    HIP;
    double RArad;
    double DErad;
    double pmRA;
    double pmDE;
};

struct DB {
    // Database entry
    int    id;                             // star id from Hipparcos-2
    double mag;                            // star magnitude
    boost::qvm::vec<double,3> centroid;    // star centroid in camera coordinates (px)
    boost::qvm::vec<double,3> uv_camera;   // star unit vector in camera coordinates
    boost::qvm::vec<double,3> uv_inertial; // star unit vector in inertial coordinates
};

}
#endif // STRUCTURES_SSIM_H
