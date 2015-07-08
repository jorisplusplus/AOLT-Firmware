/** INCLUDES *******************************************************/
#include <system_config/system.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <spi.h>
#include <usb/usb.h>
#include <app.h>
#include <usb_config.h>
#include <coms.h>
#include <Millis.h>

/** DEFINES ********************************************************/
#define _XTAL_FREQ 48000000

/** VARIABLES ******************************************************/
static uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];
char TXready;
char PutUSB;
unsigned long OldTime;

/** PROTOTYPES *****************************************************/
void NetworkScan(void);
char ReadRegister(char registerNumber);

void Initialize()
{
    CDCInitEP();
    TRISB = 0b00000001; //SDI on port RB0 (input)
    TRISA = 0xC0; //RA0 - RA5 output
    TRISCbits.TRISC7 = 0; //SD0 output;

    line_coding.bCharFormat = 0;
    line_coding.bDataBits = 8;
    line_coding.bParityType = 0;
    line_coding.dwDTERate = 9600;
    writeEEPROM(0x20,0x00);
    rfInit();
    initMillis();
    TXready = 1;
    PutUSB = 0;
    ID = 0;
    OldTime = 0;
}

void Tasks()
{
    if( USBUSARTIsTxTrfReady() == true) {
        char numBytesRead = getsUSBUSART(readBuffer, CDC_DATA_OUT_EP_SIZE);
        if(numBytesRead > 0) {
            if(readBuffer[0] == 0x00) {
                switch(readBuffer[1]) {
                    case 0x00 :
                        writeBuffer[0] = 0x00;
                        writeBuffer[1] = 0x00;
                        writeBuffer[2] = 0xFF;
                        PutUSB = 1;
                        break;
                    case 0x01 :
                        writeBuffer[0] = 0x00;
                        writeBuffer[1] = 0x01;
                        writeBuffer[2] = txReady();
                        PutUSB = 1;
                        break;
                    case 0x02 :
                        writeBuffer[0] = 0x00;
                        writeBuffer[1] = 0x02;
                        writeBuffer[2] = spiSendRead(NOP_STATUS);
                        PutUSB = 1;
                        break;
                    case 0x03 :
                        //Starting network scan. May god be with you
                        //After this the device is broken, restart to fix it.
                        NetworkScan();
                        break;
                    case 0x04 :
                        PORT_CE = 0;
                        __delay_us(5);
                        writeBuffer[0] = 0x00;
                        writeBuffer[1] = 0x04;
                        writeBuffer[2] = ReadRegister(readBuffer[2]);
                        PutUSB = 1;
                        PORT_CE = 1;
                        break;
                }
                
            } else {
                sendMessage(readBuffer[0], readBuffer[1], &readBuffer[2]);
                TXready = 0;
            }            
        } else {
            if(OldTime + 10 < Millis) {
                OldTime = Millis;
            char code = checkMessage(writeBuffer);
            if(code == 1){
                PutUSB = 1;
            } else if(txReady() == 1 && TXready == 0){
                TXready = 1;
                writeBuffer[0] = 0x00;
                writeBuffer[1] = 0xFE;
                if(Failed) {
                    Failed = 0;
                    writeBuffer[2] = 0x1F;
                } else {
                    writeBuffer[2] = 0x0F;
                }
                PutUSB = 1;
            }
            }
        }
        if(PutUSB) {
            putUSBUSART(writeBuffer,8);
            PutUSB = 0;
        }
    }
    CDCTxService();
    handleMillis();
}

void NetworkScan(void) {
    PORT_CE = 0;
    for(char channels = 0; channels<128; channels++) {
        writeRegister(RF_CH, channels);
        writeBuffer[0] = 0;
        writeBuffer[1] = 0;
        writeBuffer[2] = 0;
        writeBuffer[3] = channels;
        writeBuffer[4] = 0xFF;        
        for(char times = 0; times<200; times++) {
            PORT_CE = 1;
            __delay_us(600);
            
            if(ReadRegister(0x09) > 0x00) {
                writeBuffer[2]++;
            }
            PORT_CE = 0;
            __delay_us(10);
        }
        if(writeBuffer[2] > 0) {
        putUSBUSART(writeBuffer,5);
        }
        do {
        CDCTxService();
        } while(!USBUSARTIsTxTrfReady());
        __delay_ms(10);
    }

}

char ReadRegister(char registerNumber) {
    PORT_CSN = 0;
    spiSendRead(R_REGISTER | registerNumber);
    char data = spiSendRead(0x00);
    PORT_CSN = 1;
    return data;
}