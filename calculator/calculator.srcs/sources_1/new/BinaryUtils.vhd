----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 23.10.2020 18:50:32
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
library WORK;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;
use WORK.BinaryUtils.ALL;


package body BinaryUtils is
    -- Convert a given binary to integer
    function binary_to_int(b: std_logic_vector) return integer is
    begin
        return to_integer(unsigned(b));
    end binary_to_int;
    
    -- Convert a given integer to a binary, with the specified "msb" and "lsb"
    function int_to_binary(i: integer; length: natural) return std_logic_vector is
    begin
        return std_logic_vector(to_signed(i, length));
    end int_to_binary;
    
    -- Multiply two binary numbers
    function mul(b1: std_logic_vector; 
                 b2: std_logic_vector;
                 length: natural) return std_logic_vector is
    begin
        return int_to_binary((binary_to_int(b1) * binary_to_int(b2)), length);
    end mul;
                 
    -- Divide two binary numbers
    function div(b1: std_logic_vector; 
                 b2: std_logic_vector; 
                 length: natural) return std_logic_vector is
    begin
        return int_to_binary((binary_to_int(b1) / binary_to_int(b2)), length);
    end div;
    
    -- Convert 16 bit BCD to common binary
    function bcd16_to_binary(bcd: std_logic_vector) return std_logic_vector is
    begin
        return (bcd(3 downto 0) * "01") +
                (bcd(7 downto 4) * "1010") +
                (bcd(11 downto 8) * "1100100") +
                (bcd(15 downto 12) * "1111101000");
    end bcd16_to_binary;
end BinaryUtils;
