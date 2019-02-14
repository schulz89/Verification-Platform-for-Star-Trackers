// Copyright 2018 Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * \brief Star filter (using software) implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 11/11/2018
 * 
 * \addtogroup star-filter-sw
 * \{
 */

#include "star_filter_sw.h"

using namespace std;
using namespace cv;
using namespace st;

StarFilterSW::StarFilterSW()
    : StarFilter()
{

}

StarFilterSW::StarFilterSW(uint8_t thr)
    : StarFilter()
{
    this->SetThreshold(thr);
}

vector<StarPixel> StarFilterSW::GetStarPixels(Mat img)
{
    vector<StarPixel> star_pixels;

    for(int i=0; i<img.rows; i++)
    {
        for(int j=0; j<img.cols; j++)
        {
            uint8_t pix_color;

            if (img.channels() > 1)
            {
                pix_color = img.at<Vec3b>(i,j)[1];  // Green channel
            }
            else
            {
                pix_color = img.at<uchar>(i,j);
            }

            // Pixel threshold
            if (pix_color > this->GetThreshold())
            {
                star_pixels.push_back(StarPixel(pix_color, j, i));
            }
        }
    }

    return star_pixels;
}

vector<StarPixel> StarFilterSW::GetStarPixels(Mat img, uint8_t thr)
{
    this->SetThreshold(thr);

    return this->GetStarPixels(img);
}

void StarFilterSW::SetThreshold(uint8_t val)
{
    this->threshold = val;
}

//! \} End of star-filter-sw group
