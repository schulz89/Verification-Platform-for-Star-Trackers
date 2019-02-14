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
 * \brief Centroid computation definition.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.1.0
 * 
 * \date 06/10/2018
 * 
 * \defgroup centroider Centroider
 * \ingroup app
 * \{
 */

#ifndef CENTROIDER_H_
#define CENTROIDER_H_

#include <vector>
#include <opencv2/opencv.hpp>

#include "cdpu.h"
#include "star_pixel.hpp"
#include "centroid.hpp"

#define CENTROIDER_DEFAULT_MAX_CDPUS                20
#define CENTROIDER_DEFAULT_DISTANCE_THRESHOLD       10

#define CENTROIDER_CDPU_DEFAULT_KALMAN_GAIN         0.8

/**
 * \brief Centroider class.
 */
class Centroider
{
    private:

        /**
         * \brief CDPUs list.
         */
        std::vector<CDPU> cdpus;

        /**
         * \brief Number of CDPUs.
         */
        unsigned int max_cdpus;

        /**
         * \brief Distance threshold between two pixels to become a star.
         */
        unsigned int distance_threshold;

    public:

        /**
         * \brief Class constructor.
         *
         * \return None.
         */
        Centroider();

        /**
         * \brief Class constructor.
         *
         * \param[in] n is the number of CDPUs.
         *
         * \return None.
         */
        Centroider(unsigned int n);

        /**
         * \brief Class destructor.
         *
         * \return None.
         */
        ~Centroider();

        /**
         * \brief Sets the number of CDPUs.
         *
         * \param[in] n is the new number of CDPUs.
         *
         * \return Non.
         */
        void SetNumberOfCDPUs(unsigned int n);

        /**
         * \brief Sets the distance threshold between two pixel to become a star.
         *
         * \param[in] d
         *
         * \return None.
         */
        void SetDistanceThreshold(unsigned int d);

        /**
         * \brief .
         *
         * \param[in] star_pix
         *
         * \return None.
         */
        void Compute(st::StarPixel star_pix, float gain=CENTROIDER_CDPU_DEFAULT_KALMAN_GAIN);

        /**
         * \brief Computes the centroids from a list of star pixels.
         *
         * \return A vector with all the computed centroids.
         */
        std::vector<st::Centroid> ComputeFromList(std::vector<st::StarPixel> stars, float gain=CENTROIDER_CDPU_DEFAULT_KALMAN_GAIN);

        /**
         * \brief Gets the last computed centroids.
         *
         * \return A vector with all the last computed centroids.
         */
        std::vector<st::Centroid> GetCentroids();

        /**
         * \brief Sorts a vector with centroids by their brightness.
         *
         * \param[in] centroids is a vector with unordered centroids.
         *
         * \return A vector with centroids ordered by their brightness.
         */
        std::vector<st::Centroid> SortCentroids(std::vector<st::Centroid> centroids);

        /**
         * \brief Resets the CDPUs.
         *
         * \return None.
         */
        void Reset();

        /**
         * \brief Prints a pack of centroids to a given image.
         *
         * \param[out] img is the image to print the list of centroids.
         * \param[in] centroids is the list of centroids to print.
         * \param[in] print_id is a flag to print or not the centroids ID.
         *
         * \return The result image.
         */
        cv::Mat PrintCentroids(cv::Mat img, std::vector<st::Centroid> centroids, bool print_id=false);
};

#endif // CENTROIDER_H_

//! \} End of centroider group
