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

BUILD_DIR=bin
CXX = "ccache g++"
# CXX = "ccache clang++"

CXXFLAGS = "-pipe -Wall -W -fopenmp -O2"
# CXXFLAGS = "-pipe -Wall -W -g"

$(shell   mkdir -p $(BUILD_DIR))

all: grid_database tcp_dut_client verification_platform
	mkdir -p $(BUILD_DIR)

grid_database:
	$(MAKE) CXXFLAGS=$(CXXFLAGS) BUILD_DIR=$(CURDIR)/$(BUILD_DIR) -C src/grid_database

tcp_dut_client:
	$(MAKE) CXXFLAGS=$(CXXFLAGS) BUILD_DIR=$(CURDIR)/$(BUILD_DIR) -C src/tcp_dut_client

verification_platform:
	$(MAKE) CXXFLAGS=$(CXXFLAGS) BUILD_DIR=$(CURDIR)/$(BUILD_DIR) -C src/verification_platform

clean:
	$(MAKE) clean BUILD_DIR=$(CURDIR)/$(BUILD_DIR) -C src/grid_database
	$(MAKE) clean BUILD_DIR=$(CURDIR)/$(BUILD_DIR) -C src/tcp_dut_client
	$(MAKE) clean BUILD_DIR=$(CURDIR)/$(BUILD_DIR) -C src/verification_platform

clean_data:
	rm -rf data
	rm -rf config
