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

dut_t = {
    "CENTROIDING_REGION_GROWING" : 0,
    "CENTROIDING_GABRIEL_CPP"    : 1,
    "CENTROIDING_GABRIEL_VHDL"   : 2,
    "STAR_ID_GRID"               : 3,
    "ATTITUDE_DETERMINATION"     : 4,
    "STAR_TRACKER"               : 5,
    "TCP_WRAPPER"                : 6
}

test_t = {
    "CENTROIDING"        : 0,
    "STAR_ID"            : 1,
    "ATTITUDE"           : 2,
    "STAR_TRACKER"       : 3,
    "STAR_TRACKER_IMG"   : 4
}

grid_t = {
    "REFERENCE"     : 0,
    "REFERENCE_BIN" : 1,
    "BINARY"        : 2,
    "BINARY_INDEX"  : 3,
    "K"             : 4,
    "TREE"          : 5
}
