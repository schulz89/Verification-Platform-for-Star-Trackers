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

name = tcp_ip_dut

src = $(shell find . -name "*.cpp")
obj = $(src:.cpp=.o)

ifndef BUILD_DIR
	BUILD_DIR=.
endif

LDFLAGS =`pkg-config --libs opencv atlas blas`

$(name): $(obj)
	$(CXX) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(BUILD_DIR)/$(name)
