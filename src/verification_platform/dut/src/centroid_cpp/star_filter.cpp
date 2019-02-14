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
 * \brief Star filter implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 11/11/2018
 * 
 * \addtogroup star-filter
 * \{
 */

#include "star_filter.h"

using namespace std;
using namespace cv;
using namespace st;

StarFilter::StarFilter()
{

}

StarFilter::~StarFilter()
{

}

void StarFilter::SetThreshold(uint8_t val)
{
    this->threshold = val;
}

uint8_t StarFilter::GetThreshold()
{
    return this->threshold;
}

vector<StarPixel> StarFilter::GetStarPixels(Mat img)
{
    (void) img;
    return vector<StarPixel>();
}

//! \} End of star-filter group
