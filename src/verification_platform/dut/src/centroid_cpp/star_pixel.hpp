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
 * \brief Star pixel class.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 04/10/2018
 * 
 * \defgroup star-pixel Star Pixel
 * \ingroup structs
 * \{
 */

#ifndef STAR_PIXEL_HPP_
#define STAR_PIXEL_HPP_

/**
 * \brief Star tracker namespace.
 */
namespace st
{
    /**
     * \brief Star pixel structure.
     */
    class StarPixel
    {
        public:
            /**
             * \brief Class constructor.
             *
             * \return None.
             */
            StarPixel()
            {
                this->value = 0;
                this->x     = 0;
                this->y     = 0;
            }

            /**
             * \brief Class constructor with initialization.
             *
             * \param[in] val is the pixel value.
             * \param[in] x_pos is the x-axis position.
             * \param[in] y_pos is the y-axis position.
             *
             * return None.
             */
            StarPixel(unsigned int val, unsigned int x_pos, unsigned int y_pos)
            {
                this->value = val;
                this->x     = x_pos;
                this->y     = y_pos;
            }

            /**
             * \brief Class destructor.
             *
             * \return None.
             */
            ~StarPixel()
            {

            }

            /**
             * \brief Pixel value.
             */
            unsigned int value;

            /**
             * \brief X-axis position.
             */
            unsigned int x;

            /**
             * \brief Y-axis position.
             */
            unsigned int y;
    };
}

#endif // STAR_PIXEL_HPP_

    //! \} End of star-pixel group
