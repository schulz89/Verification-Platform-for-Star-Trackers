-- Copyright 2018 Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>

-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at

--     http://www.apache.org/licenses/LICENSE-2.0

-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
-- 
-- 

-- 
-- File: clock.vhd
-- 
-- Brief: Clock generation block for simulation.
-- 
-- Author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
-- 
-- Version: 1.0-dev
-- 
-- Date: 13/12/2017
-- 

library ieee;
use ieee.std_logic_1164.all;

entity Clock is
    port(
        clk : out std_logic
        );
end Clock;

architecture behavior of Clock is
    
    signal clock_signal : std_logic := '0';
    
    begin
    
    clock_signal <= not (clock_signal) after 10 ns;     -- Clock with 20 ns time period
    
    clk <= clock_signal;
    
end behavior;
