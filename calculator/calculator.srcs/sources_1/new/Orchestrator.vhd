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
use WORK.ButtonIf;
use WORK.SevenSegmentDisplay;
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
        q : out std_logic_vector (MSB downto LSB)
    );
end component;

-- Component switchif declaration
component switchif is
    port(
        sw : in std_logic_vector (MSB downto LSB)
    );
end component;

-- Component ButtonIf declaration
component buttonif is
    port (
        btn_sum: in std_logic;
        btn_sub: in std_logic;
        btn_mul: in std_logic;
        btn_div: in std_logic;
        btn_eq:  in std_logic;
        result:  inout std_logic_vector(3 downto 0);
        execute: inout std_logic
    );
end component;

-- Component 7-segment display
component SevenSegmentDisplay is
    Port ( clock_100Mhz : in STD_LOGIC;
           reset : in STD_LOGIC;
           number_to_be_displayed: in STD_LOGIC_VECTOR(MSB DOWNTO LSB);
           anode_activate : out STD_LOGIC_VECTOR (3 downto 0);
           led_out : out STD_LOGIC_VECTOR (6 downto 0));
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

-- Button signals
signal btn_sum      : std_logic;
signal btn_sub      : std_logic;
signal btn_mul      : std_logic;
signal btn_div      : std_logic;
signal btn_eq       : std_logic;
signal result       : std_logic_vector(3 downto 0);
signal ready_to_go      : std_logic;

-- First operand signals
signal f1_write     : std_logic;
signal f1_clr       : std_logic;
signal f1_d         : std_logic_vector(MSB downto LSB);
signal f1_q         : std_logic_vector(MSB downto LSB);

-- Second operand signals
signal f2_write     : std_logic;
signal f2_clr       : std_logic;
signal f2_d         : std_logic_vector(MSB downto LSB);
signal f2_q         : std_logic_vector(MSB downto LSB);

-- 7-segment display signals
signal clock_100Mhz : STD_LOGIC;
signal reset        : STD_LOGIC;
signal displayed_num: STD_LOGIC_VECTOR(MSB DOWNTO LSB);
signal aa           : STD_LOGIC_VECTOR (3 downto 0);
signal lout         : STD_LOGIC_VECTOR (6 downto 0);

-- Operand selection signal
signal state        : std_logic_vector(1 downto 0);

-- AC button counter
signal ac_counter   : natural;

begin

    sys_alu: ALU port map (ALU_a => ALU_a,
                           ALU_b => ALU_b,
                           ALU_sel => ALU_sel,
                           ALU_go => ALU_go,
                           ALU_out => ALU_out,
                           ALU_carry => ALU_carry);
                          
    first_operand: registry port map(clk => clk,
                                     write => f1_write,
                                     clr => f1_clr,
                                     d => f1_d,
                                     q => f1_q);
                                     
    second_operand: registry port map(clk => clk,
                                      write => f2_write,
                                      clr => f2_clr,
                                      d => f2_d,
                                      q => f2_q);
                                      
    button_selection: buttonif port map(btn_sum => btn_sum,
                                        btn_sub => btn_sub,
                                        btn_mul => btn_mul,
                                        btn_div => btn_div,
                                        btn_eq => btn_eq,
                                        result => result,
                                        execute => ready_to_go);
                                        
    operand_selection: switchif port map(sw => sw);
    
    display: SevenSegmentDisplay port map(clock_100Mhz => clock_100Mhz,
                                          reset => reset,
                                          number_to_be_displayed => displayed_num,
                                          anode_activate => aa,
                                          led_out => lout);
    
    process (clk)
    begin
        if rising_edge(clk) then
            case (state) is
                -- First operand data
                when "00" => 
                    f1_d <= bcd16_to_binary(sw);
                    f1_write <= '1';
                -- Second operand data
                when "01" =>
                    f2_d <= bcd16_to_binary(sw);
                    f2_write <= '1';
                when others =>
                    null;
            end case;
        end if;
    end process;
    
    process (clk, ready_to_go)
    begin
        if rising_edge(clk) and ready_to_go = '1' then
            case (state) is
                when "01" =>
                    ALU_b <= f2_q;
                    ALU_go <= '1';
                    state <= "10";
                when "10" =>
                    ac_counter <= ac_counter + 1;
                    if ac_counter = 2 then
                        state <= "00";
                        ac_counter <= 0;
                    end if;
                when others =>
                    null;
            end case;
            ready_to_go <= '0';
        end if;
    end process;
    
    process (result)
    begin
        if state = "00" then
            ALU_a <= f1_q;
            ALU_sel <= result;
            state <= "01";
        end if;
    end process;
    
    process (sw)
    begin
        if state(1) = '0' then
            -- 7-seg display show :v
        end if;
    end process;


end Behavioral;
