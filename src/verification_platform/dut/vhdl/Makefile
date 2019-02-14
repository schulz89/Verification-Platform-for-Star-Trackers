# Copyright 2018 Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

VHDL_FILES = starfilter.vhd clock.vhd grayscale.vhd sensor.vhd threshold.vhd counter.vhd log.vhd
ENTITY = StarFilter

ifndef STOP_TIME
	STOP_TIME = 5us
endif

FLAGS = --stop-time=$(STOP_TIME)

all:
	ghdl-mcode -c $(VHDL_FILES) -r $(ENTITY) $(FLAGS)

wave:
	ghdl-mcode -c $(VHDL_FILES) -r $(ENTITY) $(FLAGS) --vcd=sim.vcd
