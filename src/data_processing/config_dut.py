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
from definitions import *

@dataclass
class ConfigDut:
    dut_name : str = "STAR_TRACKER"
    dut_identifier : str = "Generic Star Tracker"
    
    # ==========================================================================
    # Configuration parameters
    # ==========================================================================

    # Centroiding parameters (region growing)
    cp_threshold :int = 20
    cp_relaxed_threshold :int = 7
    cp_step :int = 1
    cp_min_px :int = 3
    cp_max_px :int = 768
    cp_huge :int = 768

    # Grid algorithm parameters
    gp_g :int = 24
    gp_pattern_radius :int = 750 # this is related to the sensor resolution diagonal
    gp_buffer_radius :float = 1.4142135623730951
    gp_confidence_factor :int = 255
    gp_expected_false_stars :int = 8
    gp_minimum_match :int = 3
    gp_nn_error_angle :float = 0.001
    gp_alg_variation : str = "REFERENCE"

    # Gabriel's centroiding parameters
    gb_threshold :int = 150
    gb_cdpu_count :int = 99
    gb_kalman_gain :float = 0.8

    def generate_yml (self, config_filename):
        # ==========================================================================
        # File output
        # ==========================================================================

        fs = cv2.FileStorage(config_filename, cv2.FILE_STORAGE_WRITE)

        fs.write("cp_threshold", self.cp_threshold)
        fs.write("cp_relaxed_threshold", self.cp_relaxed_threshold)
        fs.write("cp_step", self.cp_step)
        fs.write("cp_min_px", self.cp_min_px)
        fs.write("cp_max_px", self.cp_max_px)
        fs.write("cp_huge", self.cp_huge)

        fs.write("gp_g", self.gp_g)
        fs.write("gp_pattern_radius", self.gp_pattern_radius)
        fs.write("gp_buffer_radius", self.gp_buffer_radius)
        fs.write("gp_confidence_factor", self.gp_confidence_factor)
        fs.write("gp_expected_false_stars", self.gp_expected_false_stars)
        fs.write("gp_minimum_match", self.gp_minimum_match)
        fs.write("gp_nn_error_angle", self.gp_nn_error_angle)
        fs.write("gp_alg_variation", grid_t[self.gp_alg_variation])

        fs.write("gb_threshold", self.gb_threshold)
        fs.write("gb_cdpu_count", self.gb_cdpu_count)
        fs.write("gb_kalman_gain", self.gb_kalman_gain)

        fs.release()
