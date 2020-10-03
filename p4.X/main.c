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

void setup(void);
void loop(uint_fast64_t *next, char *item, char *dot);
void switch_portb(void);
void init_pins(void);
void TMR1_init(void);
void TMR3_init(void);
void initUART(void);

volatile static uint_fast64_t time_ms = 0ULL;
volatile static char rec_char = NULL;
static char values[] = { '|', '/', '-', '\\' };
static char *dots[] = { ".", "..", "..." };

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    // switch_portb();
    time_ms += 100ULL;
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void) {
    switch_portb();
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
}

void __attribute__ ((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;
}

void __attribute__ ((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;
    if (U1STAbits.FERR == 1)
        return;
    if (U1STAbits.URXDA == 1) {
        rec_char = U1RXREG;
    }
}

int main(void) {
    setup();
    uint_fast64_t next = time_ms + 1000ULL;
    char item = 0;
    char dot = 0;
    while (true) {
        loop(&next, &item, &dot);
    }
    return 0;
}

/*void putch(char data) {
    while (!IFS0bits.U1TXIF);
    U1TXREG = data;
}

void _putchar(char data) {
    putch(data);
}*/

inline void init_pins(void) {
    // Setup all possible analog pins as digital
    AD1PCFGL = 0xFFFF;
    // Setup PORTB as output
    TRISB = 0;
    /* All possible analog bits are configured as digital */
    AD1PCFGL = 0xFFFF;
     /* Set TRISx registers for uart */
    TRISBbits.TRISB6 = 1; /* RB6 is configured as input (UART1 RX) */
    TRISBbits.TRISB7 = 0; /* RB7 is configured as output (UART1 TX) */
}

inline void TMR1_init(void) {
    T1CONbits.TON = 0; // Disable TMR1
    T1CONbits.TCS = 0; // Select internal instruction cycle clock
    T1CONbits.TGATE = 0; // Disable Gated Timer mode
    T1CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
    T1CONbits.TSIDL = 0;
    TMR1 = 0x00; // Clear timer register
    PR1 = 0xF1E0; // Load the period value
    IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer1 interrupt
    T1CONbits.TON = 1; // Start Timer
}

inline void TMR3_init(void) {
    T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
    T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
    T2CONbits.T32 = 1; // Enable 32-bit Timer mode
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b01; // Select 1:1 Prescaler
    TMR3 = 0x00; // Clear 32-bit Timer (msw)
    TMR2 = 0x00; // Clear 32-bit Timer (lsw)
    PR3 = 0x0179; // Load 32-bit period value (msw)
    PR2 = 0xEE00; // Load 32-bit period value (lsw)
    IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    T2CONbits.TON = 1; // Start 32-bit Timer
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
    TMR1_init();
    TMR3_init();
}

inline void loop(uint_fast64_t *next, char *item, char *dot) {
    /*if (time_ms >= *next) { // if 1 sec passed
        printf("%d\n\r", *value);
        *value += 1;
        if (*value == 10)
            *value = 0;
        *next += 1000UL;
    }*/
    if (time_ms >= *next) {
        // printf("\t\t\t\t\t\r");
        printf("Loading%s\t[%c]\r", dots[*dot], values[*item]);
        *item = (*item + 1) % 4;
        *dot = (*dot + 1) % 3;
        *next += 100UL;
    }
    if (rec_char != NULL) {
        printf("%c", rec_char);
        rec_char = NULL;
    }
}

inline void switch_portb(void) {
    PORTBbits.RB1 = !LATBbits.LATB1;
}
