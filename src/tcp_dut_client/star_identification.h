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

#ifndef STAR_IDENTIFICATION_H
#define STAR_IDENTIFICATION_H

extern "C"{
#include <cblas.h>
}
#include <algorithm>
#include <limits>
#include <vector>
#include <string.h>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "structures_st.h"
#include "centroiding.h"
#include "standard_structures.h"

namespace st {

enum Stmode {REFERENCE, REFERENCE_BIN, BINARY, BINARY_INDEX, K};

class StarIdentification
{
public:
    StarIdentification();
    ~StarIdentification();
    std_str::Sky identifyStars(std_str::Sky csky);
    void reload();

    StarIdConfiguration config;
private:
    void bit_descriptor (unsigned long *bucket, unsigned int bucket_size, std::vector<cv::Point2d> &translated);
    void initialize_catalog(void *ptr, size_t size, std::string filename);
    void translate_rotate (std::vector<cv::Point2d> &translated, double &nn_dist, std::vector<std_str::Star> stars, uint ref);
    int normalized_coordinate (double pixel_coordinate);
    int cell(double x, double y);
    void descriptor(std::vector<int> &desc, std::vector<cv::Point2d> &translated);
    void pixel_to_unit_vector(cv::Point3d *unit_vector, cv::Point2d star);
    int array_intersection(int* array0, int size0, int* array1, int size1, int max_element);
    int binSearch(double what);
    int binSearch2(float what, float *data, int len);
    int classify(int *max, int *desc_size, std::vector<cv::Point2d> &translated);
    int classifyK(int *max, int *desc_size, std::vector<cv::Point2d> &translated, double nn_angle);
    int classify_binary(int *max, int *desc_size, std::vector<cv::Point2d> &translated, double nn_angle);
    int classify_binary_index(int *max, int *desc_size, std::vector<cv::Point2d> &translated, double nn_angle);
    int classify_binary_ref(int *max, int *desc_size, std::vector<cv::Point2d> &translated);
    void classifier(std::vector<Candidate> &candidates, std::vector<std_str::Star> stars);
    cv::Point3d cross(cv::Point3d a, cv::Point3d b);
    double vector_angle(cv::Point3d a, cv::Point3d b);
    void cluster (std::vector<std::vector<Candidate> > &output, std::vector<Candidate> &candidates, double v_fov);
    std::vector<Candidate>* verify (std::vector<std::vector<Candidate> > &clustered);
    double fov(double size, double focus);
    std_str::Star to_star(Candidate cd);
    std::vector<std_str::Star> format_output(std::vector<Candidate> *verified);
    void loadConfig();
    void loadDatabase();

    double **ref_cat;
    unsigned long long **bin_cat;
    short **lut_cat;
    float **lut_nn_cat;
    short *index;

    Stmode mode;
};

}

#endif // STAR_IDENTIFICATION_H
