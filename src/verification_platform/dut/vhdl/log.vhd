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
--! \file: log.vhd
--! 
--! \brief: Log data to a file and/or prints it on the screen.
--! 
--! \author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
--! 
--! \version: 1.0-dev
--! 
--! \date: 26/01/2018
--! 

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    use std.textio.all;

entity Log is
    generic(
        DATA_BITS   : natural := 8;                                     --! Length of the pixel data.
        X_POS_BITS  : natural := 8;                                     --! Length of the x position data.
        Y_POS_BITS  : natural := 8;                                     --! Length of the y position data.
        PRINT_DATA  : boolean := FALSE;                                 --! TRUE/FALSE to print the log data on the screen.
        FILE_NAME   : string := "star_pixels.csv"                       --! Output log file name.
        );
    port(
        log_data    : in std_logic;                                     --! Enables a log line writing.
        x_pos       : in std_logic_vector(X_POS_BITS-1 downto 0);       --! x position of the pixels values.
        y_pos       : in std_logic_vector(Y_POS_BITS-1 downto 0);       --! y position of the pixels values.
        pix_val     : in std_logic_vector(DATA_BITS-1 downto 0)         --! Pixel value in the (x,y) position.
        );
end Log;

architecture behavior of Log is

    file log_file       : text; 

    signal x_pos_val    : std_logic_vector(X_POS_BITS-1 downto 0);
    signal y_pos_val    : std_logic_vector(Y_POS_BITS-1 downto 0);
    signal pix_value    : std_logic_vector(DATA_BITS-1 downto 0);

    begin

    x_pos_val <= x_pos;
    y_pos_val <= y_pos;
    pix_value <= pix_val;

    process(log_data)

        variable log_line   : line;

        begin

        if falling_edge(log_data) then
            file_open(log_file, FILE_NAME, append_mode);                -- Opens the file in "append mode".

            if (PRINT_DATA = TRUE) then
                report "Star Pixel: " & integer'image(to_integer(unsigned(pix_value))) & " at (" & integer'image(to_integer(unsigned(x_pos_val))) & "," & integer'image(to_integer(unsigned(y_pos_val))) & ")";
            end if;

            write(log_line, integer'image(to_integer(unsigned(pix_value))), right, 1);
            write(log_line, string'(","));
            write(log_line, integer'image(to_integer(unsigned(x_pos_val))), right, 1);
            write(log_line, string'(","));
            write(log_line, integer'image(to_integer(unsigned(y_pos_val))), right, 1);

            writeline(log_file, log_line);

            file_close(log_file);
        end if;

    end process;

end behavior;
