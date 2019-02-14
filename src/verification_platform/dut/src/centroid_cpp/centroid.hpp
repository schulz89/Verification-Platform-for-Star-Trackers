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
 * \brief Centroid structure.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 1.0
 * 
 * \date 10/05/2018
 * 
 * \defgroup centroid Centroid
 * \ingroup structs
 * \{
 */

#ifndef CENTROID_HPP_
#define CENTROID_HPP_

/**
 * \brief Star tracker namespace.
 */
namespace st
{
    /**
     * \brief Centroid structure.
     */
    class Centroid
    {
        public:

            /**
             * \brief Class constructor.
             *
             * \return None.
             */
            Centroid()
            {
                this->value     = 0;
                this->pixels    = 0;
                this->x         = 0;
                this->y         = 0;
            }

            /**
             * \brief Class constructor with initialization.
             *
             * \param[in] val is the centroid pixel value.
             * \param[in] x_pos is the x-axis position.
             * \param[in] y_pos is the y-axis position.
             *
             * return None.
             */
            Centroid(unsigned int val, double x_pos, double y_pos)
            {
                this->value     = val;
                this->pixels    = 1;
                this->x         = x_pos;
                this->y         = y_pos;
            }

            /**
             * \brief Class destructor.
             *
             * \return None.
             */
            ~Centroid()
            {

            }

            /**
             * \brief Centroid value (sum of pixel values).
             */
            unsigned int value;

            /**
             * \brief X-axis position.
             */
            double x;

            /**
             * \brief Y-axis position.
             */
            double y;

            /**
             * \brief Number of pixels used to estimate the centroid.
             */
            unsigned int pixels;
    };
}

#endif // CENTROID_HPP_

//! \} End of centroid group
