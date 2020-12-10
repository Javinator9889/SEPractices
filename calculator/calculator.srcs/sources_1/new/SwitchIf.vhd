----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineers: Javinator9889 & M. Octavian 
-- 
-- Create Date: 23.10.2020 19:49:19
-- Design Name: 
-- Module Name: SwitchIf - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision: 0.01
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
library WORK;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use WORK.Registry;
use WORK.Constants.ALL;

entity SwitchIf is
    Port ( sw : in std_logic_vector (MSB downto LSB));
end SwitchIf;

architecture Behavioral of SwitchIf is

-- Component registry declaration
component registry is
    port(
        write : inout std_logic;
        clk, clr : in std_logic;
        d : in std_logic_vector (MSB downto LSB);
        q : out std_logic_vector (MSB downto LSB)
    );
end component;

-- Global CLK signal that manages registries
signal clk      : std_logic := '0';

-- Global CLR signal that clears switches registries
signal clr      : std_logic := '0';

-- Global 'd' signal that stores temporarily the value
signal d        : std_logic_vector (MSB downto LSB);

-- Register variable
signal q        : std_logic_vector (MSB downto LSB);

-- Start registry writing
signal write    : std_logic := '0';
begin

-- Input switches registry
input : registry port map(
    write => write,
    clk => clk,
    clr => clr,
    d => d,
    q => q
);

process (clk, sw)
begin
    if rising_edge(clk) then
        d <= sw;
        write <= '1';
    end if;
end process;

end Behavioral;
