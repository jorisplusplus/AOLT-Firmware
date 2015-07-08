/** INCLUDES *******************************************************/
#include <Millis.h>
#include <xc.h>

void initMillis(void) {
    Millis = 0;
    T3CON = 0b11111000;
    PIE2bits.TMR3IE = 0;
    TMR3H = 0;
    TMR3L = 0;
    T3CONbits.TMR3ON = 1;
}

void handleMillis(void) {
    if((TMR3L >= 0xDC && TMR3H == 0x05) || TMR3H > 0x05) {
            T3CONbits.TMR3ON = 0;
            TMR3H = 0;
            TMR3L = 0;
            Millis++;
            T3CONbits.TMR3ON = 1;
        }
}