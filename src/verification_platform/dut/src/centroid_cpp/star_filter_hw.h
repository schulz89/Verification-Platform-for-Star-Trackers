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
 * \brief Star filter (using hardware) definition.
 *
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 11/11/2018
 * 
 * \defgroup star-filter-hw Star Filter (hardware)
 * \ingroup app
 * \{
 */

#ifndef STAR_FILTER_HW_H_
#define STAR_FILTER_HW_H_

#include "star_filter.h"

#define STAR_FILTER_HW_VHDL_FILES_DIR           "src/verification_platform/dut/vhdl"

#define STAR_FILTER_HW_BUFFER_IMG               "/tmp/img_buf.pgm"
#define STAR_FILTER_HW_BUFFER_STAR_PIXELS       "/tmp/star_pixels.csv"
#define STAR_FILTER_HW_SIM_MAX_TIME_US          "200000"

#define STAR_FILTER_STAR_PIXELS_ROW_VAL         0
#define STAR_FILTER_STAR_PIXELS_ROW_X           1
#define STAR_FILTER_STAR_PIXELS_ROW_Y           2

/**
 * \brief A class to filter star pixels from a image.
 */
class StarFilterHW: public StarFilter
{
    private:

        /**
         * \brief Runs a hardware simulation with a given image.
         *
         * \param[in] img is the image to run the hardware simulation.
         *
         * \return None.
         */
        void RunSimulation(cv::Mat img);

        /**
         * \brief Reads a list of star pixels from a CSV file.
         *
         * \param[in] file is the CSV file to read the star pixels.
         *
         * \return A list of star pixels.
         */
        std::vector<st::StarPixel> ReadStarPixelsFromFile(const char *file);

    public:

        /**
         * \brief Class constructor.
         *
         * \return None.
         */
        StarFilterHW();

        /**
         * \brief Class constructor (overload).
         *
         * \param[in] thr is the threshold value.
         *
         * \return None.
         */
        StarFilterHW(uint8_t thr);

        /**
         * \brief Class destructor.
         *
         * \return None.
         */
        ~StarFilterHW();

        /**
         * \brief Clears the memory buffer.
         *
         * \return None.
         */
        void Clear();

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
         * \return
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

//! \} End of star-filter-hw group
