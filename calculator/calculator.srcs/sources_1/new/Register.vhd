----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineers: Javinator9889 & M. Octavian
-- 
-- Create Date: 23.10.2020 17:15:11
-- Design Name: 
-- Module Name: Register - Behavioral
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
use IEEE.STD_LOGIC_1164.ALL;


entity Registry is
    Port ( write : inout STD_LOGIC;
           clr, clk : in STD_LOGIC;
           d : in STD_LOGIC_VECTOR (7 downto 0);
           q : out STD_LOGIC_VECTOR (7 downto 0));
end Registry;

architecture Behavioral of Registry is

begin

    process (clk, clr)
    begin
        if clr = '1' then
            q <= "00000000";
        elsif clk'event and clk = '1' and write = '1' then
            write <= '0';
            q <= d;
        end if;
    end process;

end Behavioral;
