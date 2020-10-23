----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineers: Javinator9889 & M. Octavian 
-- 
-- Create Date: 23.10.2020 18:41:57
-- Design Name: 
-- Module Name:  - 
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

package BinaryUtils is
    -- Convert a given binary to integer
    function binary_to_int(b: std_logic_vector) return integer;
    
    -- Convert a given integer to a binary, with the specified "msb" and "lsb"
    function int_to_binary(i: integer; length: natural) return std_logic_vector;
    
    -- Multiply two binary numbers
    function mul(b1: std_logic_vector; 
                 b2: std_logic_vector; 
                 length: natural) return std_logic_vector;
                 
    -- Divide two binary numbers
    function div(b1: std_logic_vector; 
                 b2: std_logic_vector; 
                 length: natural) return std_logic_vector;
    
    function bcd16_to_binary(bcd: std_logic_vector) return std_logic_vector;
end package;
