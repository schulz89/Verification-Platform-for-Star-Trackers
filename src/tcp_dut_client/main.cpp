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

#include <iostream>
#include <chrono>

#include "tcpdutclient.h"
#include "standard_structures.h"

#include "centroiding.h"
#include "star_identification.h"
#include "attitude_determination.h"

#include "centroid_cpp/star_pixel.hpp"
#include "centroid_cpp/centroid.hpp"

#include "centroid_cpp/star_filter.h"
#include "centroid_cpp/star_filter_sw.h"
#include "centroid_cpp/centroider.h"

using namespace std;
using namespace std::chrono;
using namespace std_str;
using namespace cv;
using namespace st;

Centroiding *centroiding;
StarIdentification *star_identification;
AttitudeDetermination *attitude_determination;

StarFilter *star_filter;
Centroider *centroider;

float gain;

Sky process_reference (Sky input);
Sky process_modified (Sky input);
void test_reference();
void test_modified();

int main() {

    //test_reference();
    test_modified();

    return 0;
}

Sky process_reference (Sky input) {
    auto t0 = high_resolution_clock::now();
    Sky csky = centroiding->process(input);
    auto t1 = high_resolution_clock::now();
    Sky isky = star_identification->identifyStars(csky);
    auto t2 = high_resolution_clock::now();
    Sky asky = attitude_determination->process(isky);
    auto t3 = high_resolution_clock::now();
    duration<double> d0 = t1-t0;
    duration<double> d1 = t2-t1;
    duration<double> d2 = t3-t2;
    asky.time.push_back(d0.count());
    asky.time.push_back(0.0);
    asky.time.push_back(d1.count());
    asky.time.push_back(d2.count());
    return asky;
}

Sky process_modified (Sky input) {
    auto t0 = high_resolution_clock::now();
    vector<StarPixel> star_pixels = star_filter->GetStarPixels(input.image);
    auto t1 = high_resolution_clock::now();
    vector<Centroid> cents = centroider->ComputeFromList(star_pixels, gain);
    auto t2 = high_resolution_clock::now();
    Sky csky;
    for(auto &i : cents){
        Star s;
        s.centroid.x = i.x;
        s.centroid.y = i.y;
        s.mag = i.value;
        csky.stars.push_back(s);
    }
    csky.image = input.image;
    auto t3 = high_resolution_clock::now();
    Sky isky = star_identification->identifyStars(csky);
    auto t4 = high_resolution_clock::now();
    Sky asky = attitude_determination->process(isky);
    auto t5 = high_resolution_clock::now();
    duration<double> d0 = t2-t0;
    duration<double> d1 = t2-t1;
    duration<double> d2 = t4-t3;
    duration<double> d3 = t5-t4;
    asky.time.push_back(d0.count());
    asky.time.push_back(d1.count());
    asky.time.push_back(d2.count());
    asky.time.push_back(d3.count());
    return asky;
}

void test_reference()
{
    centroiding = new Centroiding;
    star_identification = new StarIdentification();
    attitude_determination = new AttitudeDetermination;
    unsigned long count = 0;
    TcpDutClient tcpDutClient("holo.omn.icu");
    while(true){
        tcpDutClient.clientReceiveSend(process_reference); // Dark sorcery...
        cout << "Running iteration: " << count++ << endl;
    }
    delete centroiding;
    delete star_identification;
    delete attitude_determination;
}

void test_modified()
{
    star_identification = new StarIdentification();
    attitude_determination = new AttitudeDetermination;

    star_identification->config.grid_parameters.error = 0.001;
    gain = 0.8;
    unsigned int threshold = 150;    // 150, 40;
    unsigned int max_centroids = 99; // 30

    star_filter = new StarFilterSW(threshold);
    centroider  = new Centroider(max_centroids);

    unsigned long count = 0;
    TcpDutClient tcpDutClient("holo.omn.icu");
    while(true){
        tcpDutClient.clientReceiveSend(process_modified); // Dark sorcery...
        cout << "Running iteration: " << count++ << endl;
    }
    delete star_identification;
    delete attitude_determination;
    delete star_filter;
    delete centroider;
}
