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
 * \brief Star filter (using software) definition.
 *
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 11/11/2018
 * 
 * \defgroup star-filter-sw Star Filter (software)
 * \ingroup app
 * \{
 */

#ifndef STAR_FILTER_SW_H_
#define STAR_FILTER_SW_H_

#include "star_filter.h"

/**
 * \brief A class to filter star pixels from a image.
 */
class StarFilterSW: public StarFilter
{
    public:

        /**
         * \brief Class constructor.
         *
         * \return None.
         */
        StarFilterSW();

        /**
         * \brief Class constructor (overload).
         *
         * \param[in] thr is the threshold value.
         *
         * \return None.
         */
        StarFilterSW(uint8_t thr);

        /**
         * \brief Gets star pixels from a given image.
         *
         * \param[in] img is the image to search for the star pixels.
         *
         * \return A set of star pixels.
         */
        std::vector<st::StarPixel> GetStarPixels(cv::Mat img);

        /**
         * \brief Gets star pixels from a given image with a custom threshold value.
         *
         * \param[in] img is the image to search for the star pixels.
         *
         * \return A set of star pixels.
         */
        std::vector<st::StarPixel> GetStarPixels(cv::Mat img, uint8_t thr);

        /**
         * \brief Sets the threshold value of the threshold filter.
         *
         * \param[in] val is the new threshold value.
         *
         * \return None.
         */
        void SetThreshold(uint8_t val);
};

#endif // STAR_FILTER_SW_H_

//! \} End of star-filter-sw group
