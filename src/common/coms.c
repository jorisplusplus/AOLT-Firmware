/** INCLUDES *******************************************************/
#include <coms.h>
#include <timers.h>
#include <xc.h>

/** VARIABLES ******************************************************/
char PayloadBuffer[8];
char Retries;
unsigned long TimeElapsed;

void rfInit(void) {
    ID = readEEPROM(0x20);
    if(ID > 15) {
        ID = 15;
        writeEEPROM(0x20, 15);
    }
    PORT_CE = 0;
    PORT_CSN = 1;
    OpenSPI(SPI_FOSC_4, MODE_00, SMPMID);
    Delay10TCYx(100);
    writeRegister(EN_AA, DISABLE_AA);
    writeRegister(CONFIG, RX_MODE);
    writeRegister(SETUP_AW, ADDRESS);
    writeRegister(EN_RXADDR, ENABLE_PIPE_ZERO);
    writeRegister(SETUP_RETR, DISABLE_ARC);
    writeRegister(RX_PW_P0, SETUP_PAYLOAD);
    writeRegister(RF_SETUP, SETUP_RF);
    PORT_CSN = 0;
    Delay10TCYx(5);
    spiSendRead(FLUSH_RX);
    PORT_CSN = 1;
    Delay10TCYx(5);
    PORT_CE = 1;
    Failed = 0;
    Status = NotBusy;
}

void writeRegister(char registerNumber, char value) {
    PORT_CSN = 0;
    spiSendRead(W_REGISTER | registerNumber);
    spiSendRead(value);
    PORT_CSN = 1;
    Delay10TCYx(5);
}

//Send data that is in the PayloadBuffer
void send(void) {
    PORT_CE = 0;
    Delay10TCYx(1);
    writeRegister(CONFIG, TX_MODE);
    PORT_CSN = 0;
    spiSendRead(W_TX_PAYLOAD);
    for(char i = 0; i <8; i++) {
        spiSendRead(PayloadBuffer[i]);
    }
    PORT_CSN = 1;
    Delay10TCYx(1);
    PORT_CE = 1;
    TimeElapsed = Millis;
    char state;
    do {
        Delay10TCYx(20);
        PORT_CSN = 0;
        spiSendRead(FIFO_STATUS);
        state = spiSendRead(0x00);
        PORT_CSN = 1;
    } while ((state & 0b00010000) == 0);
    PORT_CE = 0;
    writeRegister(CONFIG, RX_MODE);
    writeRegister(REG_STATUS, 0x20);
    PORT_CE = 1;
}

char sendMessage(char address, char command, char* data) {
    if(Status) {
        PayloadBuffer[0] = address | (ID << 4);
        PayloadBuffer[1] = command;
        for(char i = 0; i <6; i++) {
            PayloadBuffer[2+i] = data[i];
        }
        Retries = 0;
        Status = Busy;
        send();
        return 1;
    }
    return 0;
}

char checkMessage(char* data) {
    char StatusReg;
    PORT_CSN = 0;
    StatusReg = spiSendRead(NOP_STATUS);
    PORT_CSN = 1;
    StatusReg = StatusReg & 0b00001110;               //MASK RX FIFO
    if(StatusReg == 0x00) {                        //Data in datapipe 0
        PORT_CE = 0;
        PORT_CSN = 0;
        spiSendRead(R_RX_PAYLOAD);
        for(char i = 0; i < 8; i++) {           //Read the 8 byte payload
            data[i] = spiSendRead(0xFF);
        }
        PORT_CSN = 1;
        Delay10TCYx(5);
        writeRegister(REG_STATUS, 0x40);            //Clear RX interrupt
        PORT_CE = 1;
        if((data[Target] & 0x0F) == ID) {
            if(data[Command] == 0xFF) {
                Status = NotBusy;
                return 0;
            }
            Delay1KTCYx(10);
            PayloadBuffer[0] = (data[Target] >> 4) | (ID << 4);            //Prepare ack package
            PayloadBuffer[1] = 0xFF;
            send();                             //Send ack package
            return 1;
            }  
        }
    if(Status == Busy && TimeElapsed + 5 <= Millis) { //No ack received in 5ms
        send();
        Retries++;
        if(Retries > 10){
            Status = NotBusy;
            Failed = 1;
        } //Failed transmission ten times
    }
    return 0;
}

char txReady(void) { //1 when ready 0 when busy.
    return Status;
}

char spiSendRead(char byte) {
    SSPBUF = byte;
    while(!DataRdySPI())
        ;
    return SSPBUF;
 }
 
char readEEPROM(char address) {
    char data;
    /*write the address to the register*/
    EEADR=address;
    /*clear the bits to select data eeprom and the eemprom memory*/
    EECON1bits.EEPGD=0;
    EECON1bits.CFGS=0;
    /*start a read sequence*/
    EECON1bits.RD=1;
    /*read the value*/
    return(data=EEDATA);
}

void writeEEPROM(char address,char data) {
    /*writes the address to the register*/
    EEADR=address;
    /*writes the data to the register*/
    EEDATA=data;
    /*selects eeprom data memory*/
    EECON1bits.EEPGD=0;
    EECON1bits.CFGS=0;
    /*enables writes to the eeprom*/
    EECON1bits.WREN=1;
    /*disable interrupts*/
    INTCONbits.GIEH = 0;
    INTCONbits.GIE=0;
    /*exact sequence to write data to the eeprom*/
    EECON2=0x55;
    EECON2=0x0AA;

    /*write data to the eeprom*/
    EECON1bits.WR=1;
    /*enables interrupts*/
    INTCONbits.GIEH=1;

}