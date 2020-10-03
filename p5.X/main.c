/*
 * File:   main.c
 * Author: javinator9889
 *
 * Created on 25 de septiembre de 2020, 17:03
 */

#include "pragmas.h"
#include "xc.h"
#include <libpic30.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "time.h"

void setup(void);
void loop(void);
void init_pins(void);
void initUART(void);
void init_ADC(void);

volatile static bool ADC_finished = false;
volatile static uint16_t ADC_result = 0;
volatile static time_t next = 0ULL;

void __attribute__ ((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;
}

void __attribute__ ((__interrupt__, no_auto_psv)) _ADC1Interrupt(void) {
    ADC_finished = true;
    ADC_result = ADCBUF0;
    
    IFS0bits.AD1IF = 0;
}

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

    /* RA0 = AN0
    RA1 = AN1
    RB0 = AN2 */
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB0 = 1;
    AD1PCFGLbits.PCFG0 = 0;
    AD1PCFGLbits.PCFG1 = 0;
    AD1PCFGLbits.PCFG2 = 0;

     /* Set TRISx registers for uart */
    TRISBbits.TRISB6 = 1; /* RB6 is configured as input (UART1 RX) */
    TRISBbits.TRISB7 = 0; /* RB7 is configured as output (UART1 TX) */
}

inline void initUART(void) {
    /* Specified pins for UART1 */
    RPINR18bits.U1RXR = 6; /* Pin RP6 asigned to UART1 RX */
    RPOR3bits.RP7R = 0b00011; /* UART1 TX asigned to RP7 */

    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;
    
    // Setup UART
    // Stop on idle
    U1MODEbits.USIDL = 1;
    // Disable IrDA
    U1MODEbits.IREN = 0;
    // Use only TX and RX pins
    // ignoring CTS, RTS and BCLK
    U1MODEbits.UEN = 0;
    // Do not wake-up with UART
    U1MODEbits.WAKE = 0;
    // Disable loopback mode
    U1MODEbits.LPBACK = 0;
    // Do not use automatic baudrate when receiving
    U1MODEbits.ABAUD = 0;
    // Disable polarity inversion. Idle state is '1'
    U1MODEbits.URXINV = 0;
    // Do not use high speed baudrate
    U1MODEbits.BRGH = 0;
    // 8 data bits without parity
    U1MODEbits.PDSEL = 0;
    // One stop bit
    U1MODEbits.STSEL = 0;
    
    // Calculate the baudrate using the following equation
    // UxBRG = ((FCY / Desired Baud rate) / 16) - 1
    // For 9600 bauds and FCY = 39.6288E6, the obtained BRG is
    // -> 257, and the obtained baudrate is: 9600, with an error
    // of 0%
    U1BRG = 257; 

    // Interrupt after one RX character is received;
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN enabled; ADDEN disabled; 
    U1STA = 0x400;
    
    // Enable UART TX Interrupt
    IEC0bits.U1TXIE = 1;
    IEC0bits.U1RXIE = 1;
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;
    IPC2bits.U1RXIP = 0b110;

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U1MODEbits.UARTEN = 1; // enabling UART ON bit
    U1STAbits.UTXEN = 1;
    
    // Wait 105 uS (when baudrate is 9600) for a first
    // transmission bit to be sent and detected, so then
    // the UART can be used
    __delay32(4122UL);
}

inline void setup(void) {
#ifdef CONFIG_OVERCLOCK
    // FRCDIV FRC/1; PLLPRE 2; DOZE 1:8; PLLPOST 1:2; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3000;
    // TUN Center frequency; 
    OSCTUN = 0x00;

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
    // Disable Watchdog timer
    RCONbits.SWDTEN = 0;

    init_pins();
    initUART();
    TIME_init();
    init_ADC();
}

inline void loop(void) {
    if (TIME_now() >= next) {
        next = TIME_now() + 333ULL;
        for (uint8_t channel = 0; channel < 3; ++channel) {
            AD1CHS0bits.CH0SA = channel;
            __delay32(40);
            // Start sampling
            AD1CON1bits.SAMP = 1;
            // Wait the minimum dealy admited by __delay32
            __delay32(12);
            // Hold the value and start conversion
            AD1CON1bits.SAMP = 0;
            while (!ADC_finished);
            ADC_finished = false;
            printf("%d ", ADC_result);
        }
        printf("\r\n");
    }
}

inline void init_ADC(void) {
    AD1CON1bits.AD12B = 0; /* 10 bit conversion */
    AD1CON2bits.VCFG = 0b000; /* Vdd and Vss as voltage references */
    AD1CON3bits.ADRC = 0; /* clock derived from system clock */
    AD1CON3bits.ADCS = 0x02; /* Clock for a TAD of 75 ns */
    AD1CHS0bits.CH0SA = 0b00010; /* Channel 0 positive input as AN2 */
    AD1CHS0bits.CH0NA = 0; /* Channel 0 negative input as -Vref */
    AD1CON2bits.CHPS = 0b00; /* Channel 0 selected for sample and conversion */
    AD1CON2bits.CSCNA = 0; /* Do not scan inputs */
    AD1CON1bits.ASAM = 0; /* Sample begins when SAMP bit is set */
    AD1CON1bits.SAMP = 0;
    AD1CON1bits.SSRC = 0b000; /* Clearing sample bit ends sampling and start conversion */
    AD1CON1bits.FORM = 0b00; /* Output format = integer */
    AD1CON2bits.BUFM = 0; /* Always starts filling buffer at address 0 */
    AD1CON2bits.ALTS = 0; /* Always uses channel input selects for Sample A */
    AD1CON2bits.SMPI = 0x0; /* One conversion per interrupt */
    IPC3bits.AD1IP = 7;
    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;
    AD1CON1bits.ADON = 1; /* Start ADC module */
}
