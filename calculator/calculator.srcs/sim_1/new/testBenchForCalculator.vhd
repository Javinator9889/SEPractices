----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 14.12.2020 18:33:49
-- Design Name: 
-- Module Name: testBenchForCalculator - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity testBenchForCalculator is
    --Port ( clk : in STD_LOGIC;
    --       btn_sum : in STD_LOGIC;
    --       btn_sub : in STD_LOGIC;
    --       btn_mul : in STD_LOGIC;
    --       btn_div : in STD_LOGIC;
    --       btn_eq : in STD_LOGIC;
    --       LED_out : out STD_LOGIC_VECTOR (6 downto 0);
    --       anode_activate : out STD_LOGIC_VECTOR (3 downto 0);
    --       sw : inout STD_LOGIC_VECTOR (15 downto 0));
end testBenchForCalculator;

architecture Behavioral of testBenchForCalculator is
    signal btn_sum, btn_sub, btn_mul, btn_div, btn_eq: STD_LOGIC;
    signal LED_out : STD_LOGIC_VECTOR (6 downto 0);
    signal anode_activate : STD_LOGIC_VECTOR (3 downto 0);
    signal swk : STD_LOGIC_VECTOR (15 DOWNTO 0) := (others => '0');
    signal clk : STD_LOGIC := '0';
    
    constant clk_period : time := 10 ns;
begin

    UUT : entity work.orchestrator port map (btn_sum => btn_sum, btn_sub => btn_sub, 
                                             btn_mul => btn_mul, btn_div => btn_div, 
                                             btn_eq => btn_eq, LED_out => LED_out,
                                             anode_activate => anode_activate,
                                             sw => swk, clk => clk);
                                             
    clk <= not clk after clk_period;                                         
    swk <= "0000000000000001" after 20ns; 
    btn_sum <= '1' after 40ns, '0' after 60ns;
    btn_eq <= '1' after 80ns;
     
end Behavioral;
