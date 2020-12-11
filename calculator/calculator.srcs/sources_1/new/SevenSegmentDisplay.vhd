----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 23.10.2020 19:36:53
-- Design Name: 
-- Module Name: SevenSegmentDisplay - Behavioral
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
use IEEE.std_logic_unsigned.all;
use work.constants.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity SevenSegmentDisplay is
    Port ( clock_100Mhz : in STD_LOGIC;
           reset : in STD_LOGIC;
           number_to_be_displayed: in STD_LOGIC_VECTOR(MSB DOWNTO LSB);
           anode_activate : out STD_LOGIC_VECTOR (3 downto 0);
           led_out : out STD_LOGIC_VECTOR (6 downto 0));
end SevenSegmentDisplay;

architecture Behavioral of SevenSegmentDisplay is

signal one_second_counter: STD_LOGIC_VECTOR(27 DOWNTO 0);
signal one_second_enable: STD_LOGIC;
signal displayed_number: STD_LOGIC_VECTOR(15 DOWNTO 0);
signal led_bcd: STD_LOGIC_VECTOR(3 DOWNTO 0);
signal refresh_counter: STD_LOGIC_VECTOR(19 DOWNTO 0);
signal LED_activating_counter: STD_LOGIC_VECTOR(1 DOWNTO 0);

begin

process(led_bcd)
begin
    case (led_bcd) is
    when "0000" => LED_out <= "0000001"; -- "0"     
    when "0001" => LED_out <= "1001111"; -- "1" 
    when "0010" => LED_out <= "0010010"; -- "2" 
    when "0011" => LED_out <= "0000110"; -- "3" 
    when "0100" => LED_out <= "1001100"; -- "4" 
    when "0101" => LED_out <= "0100100"; -- "5" 
    when "0110" => LED_out <= "0100000"; -- "6" 
    when "0111" => LED_out <= "0001111"; -- "7" 
    when "1000" => LED_out <= "0000000"; -- "8"     
    when "1001" => LED_out <= "0000100"; -- "9" 
    when "1010" => LED_out <= "0000010"; -- a
    when "1011" => LED_out <= "1100000"; -- b
    when "1100" => LED_out <= "0110001"; -- C
    when "1101" => LED_out <= "1000010"; -- d
    when "1110" => LED_out <= "0110000"; -- E
    when "1111" => LED_out <= "0111000"; -- F
    when others => LED_out <= "0000000";
    end case;
end process;

process(clock_100Mhz, reset)
begin
    if(reset='1') then
        refresh_counter <= (others => '0');
    elsif(rising_edge(clock_100Mhz)) then
        refresh_counter <= refresh_counter + 1;
    end if;
end process;
LED_activating_counter <= refresh_counter(19 DOWNTO 18);

process(LED_activating_counter)
begin
    case LED_activating_counter is 
    when "00" =>
        Anode_Activate <= "0111";
        led_bcd <= displayed_number(15 DOWNTO 12);
    when "01" =>
        Anode_Activate <= "1011";
        led_bcd <= displayed_number(11 DOWNTO 8);
    when "10" =>
        Anode_Activate <= "1101";
        led_bcd <= displayed_number(7 DOWNTO 4);
    when "11" =>
        Anode_Activate <= "1110";     
        led_bcd <= displayed_number(3 DOWNTO 0);
    when others =>
        null;
    end case;
end process;

process(clock_100Mhz)
begin
    displayed_number <= number_to_be_displayed;
end process;

--process(clock_100Mhz, reset)
--begin
--    if(reset = '1') then
--        one_second_counter <= (others => '0');
--    elsif(rising_edge(clock_100Mhz)) then
--        if (one_second_counter >= x"5F5E0FF") then
--            one_second_counter <= (others => '0');
--        else
--            one_second_counter <= one_second_counter + "0000001";
--        end if;
--    end if;
--end process;
--one_second_enable <= '1'  when one_second_counter = x"5F5E0FF" else '0';
--process(clock_100Mhz, reset)
--begin
--    if(reset='1') then
--        displayed_number <= (others => '0');
--    elsif(rising_edge(clock_100Mhz)) then 
--        if(one_Second_enable = '1') then
--            displayed_number <= displayed_number + x"0001";
--        end if;
--    end if;
--end process;
end Behavioral;
