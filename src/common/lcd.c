/** INCLUDES **************************************************/
#include "lcd.h"
#include <xc.h>
#include <delays.h>

void initDisplay(void) {
    PIN_ENABLE = 0;
    PIN_RS = 0;
    setOutput(0x03);
    pulseEnableLong();
    pulseEnable();
    pulseEnable();
    setOutput(0x02);
    pulseEnableLong();
    sendCommand(0x28); //4-bit interface with 5x7 pixels
    sendCommand(0x0C);
    sendCommand(0x01); //Clear screen
    sendCommand(0x06); //Entry mode
    sendCommand(0x02); //Move cursor home

}

void setOutput(char data) {
    //First we preserve the data on the high nibble
    //Remove any data on the high nibble
    //Combine the two for the new output
    char temp = LATA & 0xF0;
    char temp2 = data & 0x0F;

    LATA = temp | temp2;
    Nop();
}

void pulseEnable(void) {
    PIN_ENABLE = 1;
    Delay100TCYx(12);
    PIN_ENABLE = 0;
    Delay100TCYx(14);
}

void pulseEnableLong(void) {
    PIN_ENABLE = 1;
    Delay10KTCYx(3);
    PIN_ENABLE = 0;
    Delay10KTCYx(4);
}

void sendCommand(char command) {
    setOutput(command>>4);
    pulseEnableLong();
    setOutput(command);
    pulseEnableLong();
}

void sendData(char data) {
    setOutput(data>>4);
    pulseEnable();
    setOutput(data);
    pulseEnable();
}

void print(char line, char pos, char* data) {
    setPos(line, pos);
    PIN_RS = 1;
    for(char i = 0; data[i] != '\0' && i<16; i++) {
        sendData(data[i]);
    }
    PIN_RS = 0;
}

void setPos(char line, char pos) {
    sendCommand(0x80 + line*64 + pos);
}

void clearDisplay(void){
    sendCommand(0x01);
}
