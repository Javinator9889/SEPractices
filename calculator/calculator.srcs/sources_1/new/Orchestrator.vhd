----------------------------------------------------------------------------------
-- Company: NPCMOS
-- Engineers: Javinator9889 & M. Octavian 
-- 
-- Create Date: 23.10.2020 20:02:52
-- Design Name: 
-- Module Name: Orchestrator - Behavioral
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
use IEEE.NUMERIC_STD.ALL;
use WORK.ALU;
use WORK.Registry;
use WORK.SwitchIf;
use WORK.Constants.ALL;
use WORK.BinaryUtils.ALL;

entity Orchestrator is
    port (
        -- 100 MHz system clock
        clk             : in std_logic;
        -- 7 segment display
        led_out         : out std_logic_vector(6 downto 0);
        -- 4 anode signals
        anode_activate  : out std_logic_vector(3 downto 0)
    );
end Orchestrator;

architecture Behavioral of Orchestrator is
-- Declare here 7 segment management

-- Component alu declaration
component alu is
    port(
        -- Input operands
        ALU_a, ALU_b    : in std_logic_vector (MSB downto LSB);
        -- ALU operation selection
        ALU_sel         : in std_logic_vector (3 downto 0);
        -- ALU go flag
        ALU_go          : inout std_logic;
        -- ALU output operation result
        ALU_out         : out std_logic_vector (MSB downto LSB);
        -- ALU carry output
        ALU_carry       : out std_logic
    );
end component;

-- Component registry declaration
component registry is
    port(
        write : inout std_logic;
        clk, clr : in std_logic;
        d : in std_logic_vector (MSB downto LSB);
        q : in std_logic_vector (MSB downto LSB)
    );
end component;

-- Component switchif declaration
component switchif is
    port(
        sw : in std_logic_vector (MSB downto LSB)
    );
end component;

-- ALU signals
signal ALU_a        : std_logic_vector (MSB downto LSB);
signal ALU_b        : std_logic_vector (MSB downto LSB);
signal ALU_sel      : std_logic_vector (3 downto 0);
signal ALU_go       : std_logic;
signal ALU_out      : std_logic_vector (MSB downto LSB);
signal ALU_carry    : std_logic;

-- Switch signals
signal sw           : std_logic_vector (MSB downto LSB);

begin


end Behavioral;
