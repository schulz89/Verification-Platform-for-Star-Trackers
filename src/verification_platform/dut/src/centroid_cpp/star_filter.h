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
 * \brief Star filter definition.
 *
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 04/10/2018
 * 
 * \defgroup star-filter Star Filter
 * \ingroup app
 * \{
 */

#ifndef STAR_FILTER_H_
#define STAR_FILTER_H_

#include <vector>
#include <stdint.h>
#include <opencv2/opencv.hpp>

#include "star_pixel.hpp"

#define STAR_FILTER_DEFAULT_THRESHOLD_VAL   150         /**< Default value of the threshold filter (0 to 255). */

/**
 * \brief A class to read the star pixels from a memory region.
 */
class StarFilter
{
    protected:

        /**
         * \brief Threashold filter value.
         */
        uint8_t threshold;

    public:

        /**
         * \brief Class constructor without memory initialization.
         *
         * \return None.
         */
        StarFilter();

        /**
         * \brief Class destructor.
         *
         * \return None.
         */
        ~StarFilter();

        /**
         * \brief Sets the threshold value of the threshold filter.
         *
         * \param[in] val is the new threshold value.
         *
         * \return None.
         */
        virtual void SetThreshold(uint8_t val);

        /**
         * \brief Gets the threshold value of the threshold filter.
         *
         * \return The current threshold value.
         */
        uint8_t GetThreshold();

        /**
         * \brief Gets star pixels from a given image.
         *
         * \param[in] img is the image to search for the star pixels.
         *
         * \return A set of star pixels.
         */
        virtual std::vector<st::StarPixel> GetStarPixels(cv::Mat img);
};

#endif // STAR_FILTER_H_

//! \} End of star-filter group
