----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineer: Javinator9889 & M. Octavian
-- 
-- Create Date: 23.10.2020 17:03:31
-- Design Name: 
-- Module Name: ALU - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision: 0.01
-- Revision 0.01 - File Created
-- Additional Comments: the ALU
-- 
----------------------------------------------------------------------------------


library IEEE;
library WORK;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;
use WORK.Registry;
use WORK.Constants.ALL;
use WORK.BinaryUtils.ALL;


entity ALU is
    port (
        -- Input operands
        ALU_a, ALU_b    : in STD_LOGIC_VECTOR (MSB downto LSB);
        -- ALU operation selection
        ALU_sel         : in STD_LOGIC_VECTOR (3 downto 0);
        -- ALU go flag
        ALU_go          : in STD_LOGIC;
        -- ALU output operation result
        ALU_out         : out STD_LOGIC_VECTOR (MSB downto LSB);
        -- ALU carry output
        ALU_carry       : out STD_LOGIC
    );
end ALU;

architecture Behavioral of ALU is

-- Global CLK signal that manages registries
signal clk   : STD_LOGIC := '0';

-- Global CLR signal that clears ALU registries
signal clr   : STD_LOGIC := '0';

-- Registry for operand 1 components
signal d1     : STD_LOGIC_VECTOR (MSB downto LSB);
signal q1     : STD_LOGIC_VECTOR (MSB downto LSB);
signal write1 : STD_LOGIC := '0';

-- Registry for operand 2 components
signal d2     : STD_LOGIC_VECTOR (MSB downto LSB);
signal q2     : STD_LOGIC_VECTOR (MSB downto LSB);
signal write2 : STD_LOGIC := '0';

-- Registry for ALU output
signal d3     : STD_LOGIC_VECTOR (MSB downto LSB);
signal q3     : STD_LOGIC_VECTOR (MSB downto LSB);
signal write3 : STD_LOGIC := '0';

-- Component registry declaration
component registry is
    port(
        write : inout STD_LOGIC;
        clk, clr : in STD_LOGIC;
        d : in STD_LOGIC_VECTOR (MSB downto LSB);
        q : in STD_LOGIC_VECTOR (MSB downto LSB)
    );
end component;

begin

-- First operand registry port
first_operand : Registry port map(
    write => write1,
    clk => clk,
    clr => clr,
    d => d1,
    q => q1
);

-- Second operand registry port
second_operand : Registry port map(
    write => write2,
    clk => clk,
    clr => clr,
    d => d2,
    q => q2
);

-- ALU output
output : Registry port map(
    write => write3,
    clk => clk,
    clr => clr,
    d => d3,
    q => q3
);

process (clk, ALU_go, ALU_sel)
begin
    if rising_edge(clk) and ALU_go = '1' then
        case(ALU_sel) is
            -- Addition
            when "0000" =>
                d3 <= q1 + q2;
                write3 <= '1';
            -- Subtraction
            when "0001" =>
                d3 <= q1 - q2;
                write3 <= '1';
            -- Multiplication
            when "0010" =>
                d3 <= mul(q1, q2, (MSB - LSB));
                write3 <= '1';
                -- d3 <= std_logic_vector(to_unsigned((to_integer(unsigned(d1)))
         end case;
    end if;
end process;
-- process (A, B, ALU_sel)
-- begin
--    case(ALU_sel) is
        -- Addition
--        when "0000" =>
            
-- end process;

end Behavioral;
