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

#ifndef STRUCTURES_ST_H
#define STRUCTURES_ST_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace st{

struct CenterParameters
{
    int threshold;         // Threshold value for considering a pixel as member of a star
    int relaxed_threshold; // Relaxed threshold for using when inside a star region
    int step;              // Step value when searching for possible stars
    int min;               // Minimum number of pixels that should be considered as a region
    int max;               // Maximum number of pixels that should be considered as a region
    int huge;              // Object considered to be huge in pixels
};

struct CentroidingConfiguration
{
    CenterParameters center_parameters;
    cv::Mat camMatrix;
    cv::Mat distCoeffs;
    bool draw;
};

struct Candidate
{
    int pos_id;
    double mag;
    cv::Point2d centroid;
    cv::Point3d unit_vector;
};

struct OpticalParameters
{
    cv::Point2i resolution;
    cv::Point2d pixel_size;
    cv::Point2d sensor_size;
    double focus;
    cv::Point2i center;
    double max_mag;
};

struct GridParameters
{
    int    g;
    int    pattern_radius;
    double buffer_radius;
    int    confidence_factor;
    int    expected_false_stars;
    int    minimum_match;
    double error;
};

struct DatabaseParameters{
    std::string ref_db_filename;
    int ref_db_rows;
    int ref_db_cols;
    std::string lut_db_filename;
    std::string lut_nn_db_filename;
    int lut_db_rows;
    int lut_db_cols;
    std::string bin_db_filename;
    int bin_db_rows;
    int bin_db_cols;
    std::string index_filename;
    int index_size;
    int index_multiplier;
};

struct StarIdConfiguration {
    OpticalParameters  optical_parameters;
    GridParameters     grid_parameters;
    DatabaseParameters database_parameters;
};

}

#endif // STRUCTURES_ST_H
