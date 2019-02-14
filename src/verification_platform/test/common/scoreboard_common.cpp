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

#include "scoreboard_common.h"

using namespace std;
using namespace cv;
using namespace std_str;
using namespace boost::qvm;

// =============================================================================
// Scoreboard for Centroiding:
// =============================================================================

SBCentroiding::SBCentroiding(){
    reset();
}

void SBCentroiding::reset()
{
    total_acc = 0;
    identified_acc = 0;
    correct_acc = 0;
    error_acc = 0;
    time_acc = 0;
    time_c_acc = 0;
}

void SBCentroiding::print()
{
    cout << "Centroiding:"                                 << endl;
    cout << "total                   = " << total_acc               << endl;
    cout << "identified              = " << identified_acc          << endl;
    cout << "correct                 = " << correct_acc             << endl;
    cout << "error std               = " << error_acc / correct_acc << endl;
    cout << "centroid only time (s)  = " << time_c_acc              << endl;
    cout << "centroid total time (s) = " << time_acc                << endl;
    cout << endl;
}

Mat SBCentroiding::sky_to_mat(Sky input)
{// Transform the structures for using the OpenCV matcher.
    Mat output = Mat::zeros( input.stars.size(), 2 ,  CV_32F);
    for(uint i=0; i<input.stars.size();i++){
        output.at<float>(i,0) = (float)input.stars[i].centroid.x;
        output.at<float>(i,1) = (float)input.stars[i].centroid.y;
    }
    return output;
}

void SBCentroiding::compare_centroids(int &correct,         // out
                                      double &error,        // out
                                      Sky sky_in,           // in
                                      Sky sky_out,          // in
                                      float max_dist = 1.0) // in
{// Returns the number of correct centroids in the image.
    vector<DMatch> matches;
    BFMatcher *matcher = new BFMatcher(NORM_L2,true);
    Mat data_in = sky_to_mat(sky_in);
    Mat data_out = sky_to_mat(sky_out);
    if(data_in.size().height && data_out.size().height) {
        matcher->match(data_in, data_out, matches);
        delete matcher;
        for(auto &i : matches){
            if(i.distance < max_dist){
                error += i.distance;
                correct ++;
            }
        }
    }
}

void SBCentroiding::compare_data (Sky sky_in, Sky sky_out)
{// Compares input with output data. Called automatically.
    int correct = 0;
    double error = 0.0;
    int identified = sky_out.stars.size();
    int total = sky_in.stars.size();
    double time = 0;   // Total time
    double time_c = 0; // Time withouth threshold
    if(sky_out.time.size()) { // Collects the correct time for Centroiding
        time =   sky_out.time[0];
        time_c = sky_out.time[1];
    }
    compare_centroids(correct, error, sky_in, sky_out, 8.0);
    correct_acc += correct;
    identified_acc += identified;
    error_acc += error;
    total_acc += total;
    time_acc += time;
    time_c_acc += time_c;

    //cout << total << "," << identified << "," << correct << "," << error << endl;

//    cout << total                         << "\t"
//         << identified                    << "\t"
//         << correct                       << "\t"
//         << (double)correct/(double)total << "\t"
//         << error / correct               << "\t"
//         << time                          << endl;

}

// =============================================================================
// Scoreboard for Star Identification:
// =============================================================================

SBStarId::SBStarId()
{
    reset();
}

void SBStarId::reset()
{
    total_acc = 0;
    identified_acc = 0;
    correct_acc = 0;
    time_acc = 0;
}

void SBStarId::print()
{
    cout << "Star Identification:"                                    << endl;
    cout << "total            = " << total_acc                        << endl;
    cout << "identified ratio = " << (double)identified_acc/total_acc << endl;
    cout << "correct ratio    = " << (double)correct_acc/total_acc    << endl;
    cout << "total time (s)   = " << time_acc                         << endl;
    cout << endl;
}

int SBStarId::compare_id_stars(Sky sky_in, Sky sky_out)
{// Determines the number of correctly identified stars.
    const int max_element = 120417; // This is the maximum id in hipparcos
    int *seen = new int[max_element];
    int count = 0;
    for(int i=0;i<max_element;i++)
        seen[i] = 0;
    for(auto &i : sky_in.stars)
        seen[i.id]++;
    for(auto &i : sky_out.stars){
        if(seen[i.id] > 0){
            count++;
            seen[i.id] = 0; //remove duplicates
        }
    }
    delete(seen);
    return count;
}

void SBStarId::compare_data (Sky sky_in, Sky sky_out)
{// Compares input with output data. Called automatically.
    correct = 0;
    identified = sky_out.stars.size();
    total = sky_in.stars.size();
    double time = 0;
    if(sky_out.time.size()){ // Collects the correct time for Star ID
        if(sky_out.time.size() == 1)
            time = sky_out.time[0];
        else{
            time = sky_out.time[sky_out.time.size() - 2];
        }
    }

    correct = compare_id_stars(sky_in, sky_out);
    correct_acc += correct;

    identified_acc += identified;
    total_acc += total;
    time_acc += time;
    // cout << total                         << "\t"
    //      << identified                    << "\t"
    //      << correct                       << "\t"
    //      << (double)correct/(double)total << "\t"
    //      << time / 1e6                    << endl;
}

// =============================================================================
// Scoreboard for Attitude Determination:
// =============================================================================

SBAttitude::SBAttitude()
{
    reset();
}

void SBAttitude::reset()
{
    correct_acc = 0;
    rejected_acc = 0;
    total_acc = 0;
    error_acc = 0.;
    time_acc = 0.;
    wrong_acc = 0;
}

void SBAttitude::print()
{
    cout << "Attitude Determination:"                             << endl;
    cout << "total          = " << total_acc                      << endl;
    cout << "correct ratio  = " << (double)correct_acc/total_acc  << endl;
    cout << "rejected ratio = " << (double)rejected_acc/total_acc << endl;
    cout << "wrong ratio    = " << (double)wrong_acc/total_acc    << endl;
    cout << "error std      = " << error_acc/correct_acc          << endl;
    cout << "total time (s) = " << time_acc                       << endl;
    cout << endl;
}

double SBAttitude::q_dist(Quaternion q1, Quaternion q2)
{// Determines the absolute angular distance between the two quaternion attitude representations.
    quat<double> q1_boost; memcpy(q1_boost.a,&q1,sizeof(q1));
    quat<double> q2_boost; memcpy(q2_boost.a,&q2,sizeof(q2));
    quat<double> q = q1_boost * inverse(q2_boost);
    double* s = (double*)&q.a[0];
    vec<double,3>* v = (vec<double,3>*)&q.a[1];
    return 2 * atan2(mag(*v),*s);
}

void SBAttitude::compare_data(Sky sky_in, Sky sky_out)
{
    rejected = 0;
    correct = 0;
    wrong = 0;
    error = 0;
    total_acc++;
    if(sky_out.q.r == 0.0 && sky_out.q.v[0] == 0.0 &&
            sky_out.q.v[1] == 0.0 && sky_out.q.v[2] == 0.0){
        rejected = 1;
        rejected_acc++;
    }
    else{
        error = q_dist(sky_in.q, sky_out.q);
        error_acc += error;
        if(error <= 0.1){ // Acceptable angle
            correct = 1;
            correct_acc++;
        }
        else{
            wrong = 1;
            wrong_acc++;
//            cout << "---" << endl;
//            cout << error << endl;
//            cout << sky_in.q.r << "\t"
//                 << sky_in.q.v[0] << "\t"
//                 << sky_in.q.v[1] << "\t"
//                 << sky_in.q.v[2] << endl;
//            cout << sky_out.q.r << "\t"
//                 << sky_out.q.v[0] << "\t"
//                 << sky_out.q.v[1] << "\t"
//                 << sky_out.q.v[2] << endl;
        }
    }

    if(sky_out.time.size())
        time_acc += sky_out.time[sky_out.time.size() - 1]; // Collects the correct time for Attitude Determination
}
