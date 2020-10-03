#include "time.h"
#include <xc.h>
#include <stdint.h>

volatile time_t _now = 0ULL;

void TIME_init(void) {
    TMR1 = 0x00;
    // Period = 1 ms;
    // Frequency = 39 628 800 Hz;
    // PR1 = 39628 == an interrupt ~= 0.99997 ms
    PR1 = 0x9ACC;
    
    // TON enabled; ->      1
    // Empty bit ->         0
    // TSIDL disabled; ->   0

    // Empty bit ->         0
    // Empty bit ->         0
    // Empty bit ->         0
    // Empty bit ->         0

    // Empty bit ->         0
    // Empty bit ->         0
    // TGATE disabled; ->   0
    // TCKPS 1:1; ->        0

    // Empty bit ->         0
    // TSYNC enabled ->     0
    // TCS FOSC/2; ->       0
    // Empty bit ->         0
    // 1000 0000 0000 0000 == 0x8000
    T1CON = 0x8000;

    // Clear interrupt flag,
    IFS0bits.T1IF = 0;
    // set priority to maximum
    IPC0bits.T1IP = 6;
    // and enable interrupts
    IEC0bits.T1IE = 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    // switch_portb();
    _now += 1ULL;
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
}

time_t TIME_now(void) {
    return _now;
}