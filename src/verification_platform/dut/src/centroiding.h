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

#ifndef CENTROIDING_H
#define CENTROIDING_H

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include "structures_st.h"
#include "standard_structures.h"

namespace st{

class Centroiding
{
public:
    Centroiding();
    std_str::Sky process(std_str::Sky csky);

    CentroidingConfiguration config;
    int huge_objects;

private:
    int regionGrowingIterative(cv::Mat image, unsigned int threshold, unsigned int relaxed_threshold, unsigned int step, unsigned int min, unsigned int max, unsigned int huge, std::vector<std::vector<cv::Point2i> > &regions);
    void regionTestIterative(cv::Mat image, unsigned int threshold, uint n, std::vector<cv::Point2i> &starList);
    void centerOfMass(cv::Mat &image, std::vector<std::vector<cv::Point2i> > &regions, std::vector<cv::Point2d> &centers, std::vector<double> &intensity);
};

}

#endif // CENTROIDING_H
