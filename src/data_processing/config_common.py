#!/usr/bin/env python3

# Copyright 2019 Victor Hugo Schulz

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import cv2
import numpy as np
from dataclasses import dataclass

@dataclass
class ConfigCommon:
    test_name        : str = "STAR_TRACKER"
    test_identifier  : str = "Generic Test"
    x_axis_name      : str = "x"
    y_axis_name      : str = "y"
    output_directory : str = "data"
    output_filename  : str = "output.csv"
    start            : int = 0
    end              : int = 2
    steps            : int = end - start + 1
    n_tests          : int = 1000

    # ==========================================================================
    # Configuration parameters
    # ==========================================================================
    # Virtual camera optical parameters
    op_resolution   :tuple = (800, 600)         # pixels
    op_vertical_fov :int   = 8                  # degrees
    op_pixel_size   :tuple = (2.8e-6, 2.8e-6) # meters
    op_max_mag      :int   = 6                  # mag
    op_dist_coeffs  :tuple = (0, 0, 0, 0, 0)    # distortion coefficients

    # Simulator parameters
    sp_psf_B              :float = 0
    sp_psf_C              :float = 50000
    sp_psf_sigma          :float = 1
    sp_psf_roi            :float = 10
    sp_position_std_dev   :float = 0
    sp_mag_std_dev        :float = 0
    sp_background_std_dev :float = 0
    sp_background_dc      :float = 0
    sp_focus_std_dev      :float = 0
    sp_focus_dc           :float = 0
    sp_false_stars        :int = 0
    sp_generate_image     :int = 1

    # Catalog parameters
    cp_hipparcos_filename  :str = "hipparcos/hip_main.dat"
    cp_hipparcos2_filename :str = "hipparcos/hip2.dat"
    cp_hipparcos_size      :int = 120417
    cp_epoch               :int = 2018

    def generate_yml (self, config_filename):
        # ==========================================================================
        # Virtual camera parameter calculation
        # ==========================================================================

        vertical_fov_rad = self.op_vertical_fov * (np.pi / 180) # conversion to radians
        op_sensor_size = (self.op_resolution[0] * self.op_pixel_size[0], self.op_resolution[1] * self.op_pixel_size[1])
        op_focus = op_sensor_size[1] / (2*np.tan(vertical_fov_rad/2))
        op_center = (self.op_resolution[0]/2, self.op_resolution[1]/2)
        op_focus_px = op_focus / self.op_pixel_size[1]
        op_camera_matrix = np.array([[op_focus_px, 0, op_center[0]], [0, op_focus_px, op_center[1]], [0, 0, 1]])

        # ==========================================================================
        # File output
        # ==========================================================================
        fs = cv2.FileStorage(config_filename, cv2.FILE_STORAGE_WRITE)

        # Write optical parameters
        fs.write("op_resolution", self.op_resolution)
        fs.write("op_pixel_size", self.op_pixel_size)
        fs.write("op_sensor_size", op_sensor_size)
        fs.write("op_focus", op_focus)
        fs.write("op_center", op_center)
        fs.write("op_max_mag", self.op_max_mag)
        fs.write("op_camera_matrix", op_camera_matrix)
        fs.write("op_dist_coeffs", self.op_dist_coeffs)

        # Write simulator parameters
        fs.write("sp_psf_B", self.sp_psf_B)
        fs.write("sp_psf_C", self.sp_psf_C)
        fs.write("sp_psf_sigma", self.sp_psf_sigma)
        fs.write("sp_psf_roi", self.sp_psf_roi)
        fs.write("sp_position_std_dev", self.sp_position_std_dev)
        fs.write("sp_mag_std_dev", self.sp_mag_std_dev)
        fs.write("sp_background_std_dev", self.sp_background_std_dev)
        fs.write("sp_background_dc", self.sp_background_dc)
        fs.write("sp_focus_std_dev", self.sp_focus_std_dev * 1e-3)
        fs.write("sp_focus_dc", self.sp_focus_dc * 1e-3)
        fs.write("sp_false_stars", self.sp_false_stars)
        fs.write("sp_generate_image", self.sp_generate_image)

        # Write catalog parameters
        fs.write("cp_hipparcos_filename", self.cp_hipparcos_filename)
        fs.write("cp_hipparcos2_filename", self.cp_hipparcos2_filename)
        fs.write("cp_hipparcos_size", self.cp_hipparcos_size)
        fs.write("cp_epoch", self.cp_epoch)

        # Write verification platform parameters
        fs.write("vp_output_directory", self.output_directory)
        fs.write("vp_output_filename", self.output_filename)
        fs.write("vp_n_tests", self.n_tests)

        fs.release()
