#include "init.h"
#include <xc.h>
#include <libpic30.h>

void init_micro(void) {
    RCONbits.SWDTEN = 0; // Disable Watchdog Timer

    // Configure Oscillator to operate the device at 40 Mhz
    // Fosc = Fin*M/(N1*N2), Fcy = Fosc/2
    // Fosc = 7.3728*43/(2*2) = 79.2576 Mhz
    // Fcy = Fosc/2 = 39.6288 MHz

    // Configure PLL prescaler, PLL postscaler and PLL divisor

    PLLFBDbits.PLLDIV = 41; // M = PLLDIV + 2 = 43 -> PLLDIV = 43 - 2 = 41
    CLKDIVbits.PLLPOST = 0; // N2 = 2 (Output/2)
    CLKDIVbits.PLLPRE = 0; // N1 = 2 (Input/2)

    // clock switching to incorporate PLL
    __builtin_write_OSCCONH(0x03); // Initiate Clock Switch to Primary
    __builtin_write_OSCCONL(0x01); // Start clock switching

    while (OSCCONbits.COSC != 0b011); // Wait for Clock switch to occur
    while (OSCCONbits.LOCK != 1) {
    }; // Wait for PLL to lock (If LOCK = 1 -> PLL start-up timer is satisfied)

}

void init_ports(void) {
    /* All possible analog bits are configured as digital */
    AD1PCFGL = 0xFFFF;

    /* Set TRISx registers for uart */
    TRISBbits.TRISB6 = 1; /* RB6 is configured as input (UART1 RX) */
    TRISBbits.TRISB7 = 0; /* RB7 is configured as output (UART1 TX) */

}

void init_uart(void) {
    /* Specified pins for UART1 */
    RPINR18bits.U1RXR = 6; /* Pin RP6 asigned to UART1 RX */
    RPOR3bits.RP7R = 0b00011; /* UART1 TX asigned to RP7 */

    U1MODEbits.USIDL = 1; // Stop on idle mode
    U1MODEbits.IREN = 0; // disable IrDA operation
    U1MODEbits.UEN = 0; // Only RX and TX are used (non CTS, RTS and BCLK)
    U1MODEbits.WAKE = 0; // Wake up on start bit is disabled
    U1MODEbits.LPBACK = 0; // Loopback mode disabled
    U1MODEbits.ABAUD = 0; // No automatic baud rate
    U1MODEbits.URXINV = 0; // Non polarity inversion. Idle state is 1
    U1MODEbits.BRGH = 0; // High baude rate disabled
    U1MODEbits.PDSEL = 0; // 8 bit data with no parity
    U1MODEbits.STSEL = 0; // One stop bit.

    U1STAbits.URXISEL = 0; // Interrupt on each character received

    U1BRG = 257; // 9600 Baudios. 39.6288*10**6/(16*9600) - 1

    /* In this configuration uart interrupts are not allowed */
    IPC2bits.U1RXIP = 0;
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 0;

    U1MODEbits.UARTEN = 1; // Enable UART operation
    U1STAbits.UTXEN = 1; // Enable uart1 TX (must be done after UARTEN)

    /* It is needed to wait one transmision bit to ensure start bit detection 
     When 9600 Baud rate is selected it is necessary to wait 104 us */
    __delay32(4122);
}
