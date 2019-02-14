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

--! 
--! \brief: Centroid detection for a star tracker simulation.
--! 
--! \author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
--! 
--! \version: 1.0
--! 
--! \date: 13/12/2017
--! 

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity Threshold is
    generic(
        DATA_WIDTH  : natural := 8;                                 --! Pixel width in bits.
        TH_VALUE    : natural := 200                                --! Threshold value (0 to 255).
        );
    port(
        clk         : in std_logic;                                 --! Clock source.
        en          : in std_logic;                                 --! Enable signal (Activation "clock").
        data_in     : in std_logic_vector(DATA_WIDTH-1 downto 0);   --! Data input (gray scale pixels).
        data_out    : out std_logic_vector(DATA_WIDTH-1 downto 0);  --! Data output (above threshold pixels).
        pixclk      : out std_logic                                 --! Signalizes when to pick the data from the output.
        );
end Threshold;

architecture behavior of Threshold is

    signal pixclk_sig : std_logic := '0';

begin

    process(clk, en)
    begin
        if falling_edge(clk) then
            pixclk_sig <= '0';
        end if;

        if falling_edge(en) then
            if (data_in > std_logic_vector(to_unsigned(TH_VALUE, DATA_WIDTH))) then
                data_out <= data_in;
                pixclk_sig <= '1';
            else
                data_out <= x"00";
            end if;
        end if;
    end process;

    pixclk <= pixclk_sig;

end behavior;
