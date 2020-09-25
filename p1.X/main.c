/*
 * File:   main.c
 * Author: javinator9889
 *
 * Created on 25 de septiembre de 2020, 17:03
 */

#include "pragmas.h"
#include "xc.h"
#include <stdbool.h>
#include <stdint.h>

void setup(void);
void loop(void);

int main(void) {
    setup();
    while (true) {
        loop();
    }

    return 0;
}

inline void init_pins(void) {
    // Setup all possible analog pins as digital
    AD1PCFGL = 0xFFFF;
    // Setup PORTB as output
    TRISB = 0;
}

inline void setup(void) {
#ifdef CONFIG_OVERCLOCK
    // FRCDIV FRC/1; PLLPRE 2; DOZE 1:8; PLLPOST 1:2; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3000;
    // TUN Center frequency; 
    OSCTUN = 0x00;
    // Disable Watchdog timer
    RCONbits.SWDTEN = 0;
    
    // For working at ~79.2576 MHz:
    // F_osc = F_in * M / (N1 * N2)
    // F_cy = F_osc / 2
    // F_osc ~= 79.2576 MHz -> F_osc = 7.3728 * 43 / (2 * 2) = 79.2576 MHz
    // F_cy = F_osc / 2 = 39.6288 MHz
    //
    // Then, setup the PLL's prescaler, postcaler and divisor
    PLLFBDbits.PLLDIV = 41; // M = PLLDIV + 2 -> PLLDIV = 65 - 2 = 63
    CLKDIVbits.PLLPOST = 0; // N2 = 2 * (PLLPOST + 1) -> PLLPOST = (N2 / 2) - 1 = 0
    CLKDIVbits.PLLPRE = 0; // N1 = PLLPRE + 2; -> PLLPRE = N1 - 2 = 0
    // AD1MD enabled; PWMMD enabled; T3MD enabled; T4MD enabled; T1MD enabled; U2MD enabled; T2MD enabled; U1MD enabled; QEI1MD enabled; SPI2MD enabled; SPI1MD enabled; C2MD enabled; C1MD enabled; DCIMD enabled; T5MD enabled; I2C1MD enabled; 
    PMD1 = 0x00;
    // OC5MD enabled; OC6MD enabled; OC7MD enabled; OC8MD enabled; OC1MD enabled; IC2MD enabled; OC2MD enabled; IC1MD enabled; OC3MD enabled; OC4MD enabled; IC6MD enabled; IC7MD enabled; IC5MD enabled; IC8MD enabled; IC4MD enabled; IC3MD enabled; 
    PMD2 = 0x00;
    // ADC2MD enabled; PMPMD enabled; U3MD enabled; QEI2MD enabled; RTCCMD enabled; CMPMD enabled; T9MD enabled; T8MD enabled; CRCMD enabled; T7MD enabled; I2C2MD enabled; T6MD enabled; 
    PMD3 = 0x00;
    // CF no clock failure; NOSC FRCPLL; CLKLOCK unlocked; OSWEN Switch is Complete; IOLOCK not-active; 
    __builtin_write_OSCCONH((uint8_t) (0x03));
    __builtin_write_OSCCONL((uint8_t) (0x01));

    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN != 0);
    // And wait for clock switching to happen
    // First, wait for clock switch to occur
    // and then wait the PLL to lock
    while (OSCCONbits.COSC != 0b011);
    while (OSCCONbits.LOCK != 1);
#endif
    
    init_pins();
}

inline void loop(void) {
    // Change RB1 value with the latest one saved
    PORTBbits.RB1 = !LATBbits.LATB1;
}
