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
 * \brief CentroiD Processor Unit class definition.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 1.0
 * 
 * \date 05/03/2018
 * 
 * \defgroup cdpu CDPU
 * \ingroup app
 * \{
 */

#ifndef CDPU_H_
#define CDPU_H_

#include <stdint.h>
#include <cmath>

//#include <structs/centroid.hpp>
#include "centroid.hpp"

#define DISTANCE_THRESHOLD                  5
#define DISTANCE_THRESHOLD_MAN              2*sqrt(pow(DISTANCE_THRESHOLD, 2))/sqrt(2)

#define CDPU_DEFAULT_KALMAN_GAIN_FACTOR     0.78

/**
 * \brief Class to implement a CentroiD Processor Unit.
 */
class CDPU
{
    private:

        /**
         * \brief Calculated centroid.
         */
        st::Centroid centroid;

        /**
         * \brief Number of pixel of the centroid.
         */
        unsigned int pixels;

        /**
         * \brief Kalman filter gain.
         */
        float K;

    public:

        /**
         * \brief Constructor.
         *
         * \return None.
         */
        CDPU();

        /**
         * \brief Destructor.
         *
         * \return None.
         */
        ~CDPU();

        /**
         * \brief Updates the centroid parameters.
         *
         * \param[in] x_new is the x position of the new pixel of a star.
         * \param[in] y_new is the y position of the new pixel of a star.
         * \param[in] color_new is the color of the new pixel of a star.
         * \param[in] gain is the Kalman Filter gain factor.
         *
         * \return None.
         */
        void Update(unsigned int x_new, unsigned int y_new, uint8_t color_new, float gain=CDPU_DEFAULT_KALMAN_GAIN_FACTOR);

        /**
         * \brief Sets the values of the centroid.
         *
         * \param[in] x_new is the new x position reference.
         * \param[in] y_new is the new y position reference
         * \param[in] color_new is the new color (8-bit) value reference.
         *
         * \return None.
         */
        void SetCentroid(unsigned int x_new, unsigned int y_new, uint8_t color_new);

        /**
         * \brief Gets the final calculated centroid of a star.
         *
         * \return The final centroid.
         */
        st::Centroid GetCentroid();

        /**
         * \brief Calculates the distance from the centroid to a given point.
         *
         * \param[in] x_comp is the x-axis position of a point.
         * \param[in] y_comp is the y-axis position of a point.
         *
         * \return The distance in pixels.
         */
        float DistanceFrom(unsigned int x_comp, unsigned int y_comp);

        /**
         * \brief Gets the number of pixels used to calculate the centroid parameters.
         *
         * \return The number of pixels used to calculate the centroid.
         */
        uint8_t GetPixels();
};

#endif // CDPU_H_

//! \} End of cdpu group
