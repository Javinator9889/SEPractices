----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineers: Javinator9889 & M. Octavian
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
        return int_to_binary(
                binary_to_int(bcd(3 downto 0)) +
                (binary_to_int(bcd(7 downto 4)) * 10) +
                (binary_to_int(bcd(11 downto 8)) * 100) +
                (binary_to_int(bcd(15 downto 12)) * 1000), 16);
    end bcd16_to_binary;
    
    function binary_to_bcd16(bin: std_logic_vector) return std_logic_vector is
        variable bcd    :   std_logic_vector(15 downto 0);
        variable bint   :   std_logic_vector(13 downto 0);
    begin
        bcd := (others => '0');
        bint := bin(13 downto 0);
        
        for i in 0 to 13 loop
            bcd(15 downto 1) := bcd(14 downto 0);
            bcd(0) := bint(13);
            bint(13 downto 1) := bint(12 downto 0);
            bint(0) := '0';
            
            if i < 13 and bcd(3 downto 0) > "0100" then
                bcd(3 downto 0) := std_logic_vector(unsigned(bcd(3 downto 0)) + 3);
            end if;
            if i < 13 and bcd(7 downto 4) > "0100" then
                bcd(7 downto 4) := std_logic_vector(unsigned(bcd(7 downto 4)) + 3);
            end if;
            if i < 13 and bcd(11 downto 8) > "0100" then
                bcd(11 downto 8) := std_logic_vector(unsigned(bcd(11 downto 8)) + 3);
            end if;
            if i < 13 and bcd(15 downto 12) > "0100" then
                bcd(11 downto 8) := std_logic_vector(unsigned(bcd(15 downto 12)) + 3);
            end if;
        end loop;
        return bcd;
    end binary_to_bcd16;
end BinaryUtils;
