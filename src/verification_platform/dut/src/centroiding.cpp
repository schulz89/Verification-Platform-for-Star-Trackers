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

#include "centroiding.h"

using namespace std;
using namespace cv;
using namespace std_str;

namespace st{

Centroiding::Centroiding()
{// Configure the centroiding parameters.
    huge_objects = 0;

    double dtmp;
    // Load parameters from file:
    FileStorage fs("config/common.yml", FileStorage::READ);
    fs["op_camera_matrix"] >> config.camMatrix;
    fs["op_dist_coeffs"] >> config.distCoeffs;
    fs.release();
    FileStorage fs2("config/dut.yml", FileStorage::READ);
    fs2["cp_threshold"]         >> dtmp; config.center_parameters.threshold = (int)dtmp;
    fs2["cp_relaxed_threshold"] >> dtmp; config.center_parameters.relaxed_threshold = (int)dtmp;
    fs2["cp_step"]              >> dtmp; config.center_parameters.step = (int)dtmp;
    fs2["cp_min_px"]            >> dtmp; config.center_parameters.min = (int)dtmp;
    fs2["cp_max_px"]            >> dtmp; config.center_parameters.max = (int)dtmp;
    fs2["cp_huge"]              >> dtmp; config.center_parameters.huge = (int)dtmp;
    fs2.release();
    config.draw = false;
}

Sky Centroiding::process(Sky csky)
{// Calculates the center of mass of the stars.
    vector<vector<Point2i> > regions;
    vector<Point2d> centers;
    vector<double> intensity;
    vector<Star> stars;

    if(csky.image.data != NULL){
        huge_objects = regionGrowingIterative(csky.image,config.center_parameters.threshold, config.center_parameters.relaxed_threshold,  config.center_parameters.step, config.center_parameters.min, config.center_parameters.max, config.center_parameters.huge, regions);
        centerOfMass(csky.image,regions,centers,intensity);

        vector<Point2d> undistorted;
        if(centers.size()){
            undistortPoints(centers,undistorted,config.camMatrix,config.distCoeffs,noArray(),config.camMatrix);
            // Passing the camera matrix again results in just radial/tangential correction, without normalization.
        }

        for(uint i=0;i<centers.size();i++){
            Star s;
            s.centroid = undistorted.at(i);
            s.mag = -intensity.at(i); // minus sign: compatibility with magnitude in comparisons < and >.
            stars.push_back(s);
        }

        if(config.draw){ // Code for drawing:
            Mat image_kp;
            vector<KeyPoint> centers_kp;
            for(uint i=0; i<undistorted.size(); i++){
                Point2f center_f = (Point2f(undistorted.at(i).x,undistorted.at(i).y));
                float size = 2*intensity.at(i);
                centers_kp.push_back(KeyPoint(center_f,size));
            }
            drawKeypoints(csky.image,centers_kp,image_kp,Scalar(0,0,255),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            csky.image = image_kp;
        }
        csky.stars = stars;
    }
    return csky;
}

int Centroiding::regionGrowingIterative(Mat image, unsigned int threshold, unsigned int relaxed_threshold, unsigned int step, unsigned int min, unsigned int max, unsigned int huge, vector<vector<Point2i> > &regions)
{// Region growing algorithm. Calculates the regions corresponding to stars in the images.
    int huge_objects = 0;
    Mat image_internal(image.clone());
    for(int i=0;i<image_internal.rows;i=i+step){
        for(int j=0;j<image_internal.cols;j=j+step){                             // here my seed is (i,j)
            if(image_internal.at<uchar>(i,j)>threshold){                         // test if it is a proper seed
                uint n = 0;                                                      // counter that points to the top of the list
                vector<Point2i> starList;                                        // empty vector of the points in the region
                starList.push_back(Point2i(i,j));                                // adds the seed to the top of the list // n=0, starList.size() = 1;
                image_internal.at<uchar>(i,j) = 0;
                do{
                    regionTestIterative(image_internal, relaxed_threshold, n, starList); // runs an iteration
                    n++;
                } while(n < starList.size() - 1);
                if(n > huge)                                                     // something is really big
                    huge_objects++;                                              // cerr << "Huge object detected!" << endl;
                if(starList.size() >= min && starList.size() <= max)
                    regions.push_back(starList);
            }
        }
    }
    return huge_objects;
}

void Centroiding::regionTestIterative(Mat image, unsigned int threshold, uint n, vector<Point2i> &starList)
{// Iteration of the region growing algorithm.
    int x = starList.at(n).x;
    int y = starList.at(n).y;
    if(x > 0)
        if(image.at<uchar>(x-1,y) > threshold){
            starList.push_back(Point2i(x-1,y));
            image.at<uchar>(x-1,y) = 0;
        }
    if(x < image.rows-1)
        if(image.at<uchar>(x+1,y) > threshold){
            starList.push_back(Point2i(x+1,y));
            image.at<uchar>(x+1,y) = 0;
        }
    if(y > 0)
        if(image.at<uchar>(x,y-1) > threshold){
            starList.push_back(Point2i(x,y-1));
            image.at<uchar>(x,y-1) = 0;
        }
    if(y < image.cols-1)
        if(image.at<uchar>(x,y+1) > threshold){
            starList.push_back(Point2i(x,y+1));
            image.at<uchar>(x,y+1) = 0;
        }
}

void Centroiding::centerOfMass(Mat &image, vector<vector<Point2i> > &regions, std::vector<Point2d> &centers, std::vector<double> &intensity)
{// Calculates the center of mass of the regions (From regionGrowingIterative).
    for(uint i=0;i<regions.size();i++){
        Point2d center(0,0);
        long unsigned int norm = 0;
        for(uint j=0;j<regions.at(i).size();j++){
            center.y += image.at<uchar>(regions.at(i).at(j).x,regions.at(i).at(j).y) * regions.at(i).at(j).x;
            center.x += image.at<uchar>(regions.at(i).at(j).x,regions.at(i).at(j).y) * regions.at(i).at(j).y;
            norm += image.at<uchar>(regions.at(i).at(j).x,regions.at(i).at(j).y);
        }
        center.x /= norm;
        center.y /= norm;
        //center.z = sqrt(regions.at(i).size()/M_PI);
        centers.push_back(center);
        intensity.push_back( sqrt((double)norm/255) ); // Intensity
    }
}

}
