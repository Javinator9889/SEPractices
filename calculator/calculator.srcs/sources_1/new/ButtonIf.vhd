----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineers: Javinator9889 & M. Octavian
-- 
-- Create Date: 09.12.2020 17:31:43
-- Design Name: 
-- Module Name: ButtonIf - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ButtonIf is
    Port ( btn_sum: in std_logic;
           btn_sub: in std_logic;
           btn_mul: in std_logic;
           btn_div: in std_logic;
           btn_eq:  in std_logic;
           result:  inout std_logic_vector(3 downto 0);
           execute: inout std_logic
    );
end ButtonIf;

architecture Behavioral of ButtonIf is

begin

    process (btn_sum)
    begin
        if btn_sum = '1' then
            result <= "0000";
        end if;
    end process;
    
    process (btn_sub)
    begin
        if btn_sub = '1' then
            result <= "0001";
        end if;
    end process;
    
    process (btn_mul)
    begin
        if btn_mul = '1' then
            result <= "0010";
        end if;
    end process;
    
    process (btn_div)
    begin
        if btn_div = '1' then
            result <= "0011";
        end if;
    end process;
    
    process (btn_eq)
    begin
        if btn_eq = '1' then
            execute <= '1';
        end if;
    end process;

end Behavioral;
