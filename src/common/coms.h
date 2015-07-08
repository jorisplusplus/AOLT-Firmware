#ifndef COMS_H
/** DEFINES ********************************************************/
#define	COMS_H

#define PORT_CSN    LATBbits.LATB2
#define PORT_CE     LATBbits.LATB3

#define NotBusy                 0x01
#define Busy                    0x00
//SPI commands
#define R_REGISTER		0x00
#define W_REGISTER		0x20
#define R_RX_PAYLOAD            0x61
#define W_TX_PAYLOAD            0xA0
#define FLUSH_TX		0xE1
#define FLUSH_RX		0xE2
#define REUSE_TX_PL             0xE3
#define NOP_STATUS		0xFF

//SPI command data masks
#define R_REGISTER_DATA         0x1F
#define W_REGISTER_DATA         0x1F

//SPI registers
#define CONFIG			0x00
#define EN_AA			0x01
#define EN_RXADDR		0x02
#define SETUP_AW		0x03
#define SETUP_RETR		0x04
#define RF_CH			0x05
#define RF_SETUP		0x06
#define REG_STATUS      	0x07
#define OBSERVE_TX		0x08
#define CD			0x09
#define RX_ADDR_P0		0x0A
#define RX_ADDR_P1		0x0B
#define RX_ADDR_P2		0x0C
#define RX_ADDR_P3		0x0D
#define RX_ADDR_P4		0x0E
#define RX_ADDR_P5		0x0F
#define TX_ADDR                 0x10
#define RX_PW_P0		0x11
#define RX_PW_P1		0x12
#define RX_PW_P2		0x13
#define RX_PW_P3		0x14
#define RX_PW_P4		0x15
#define RX_PW_P5		0x16
#define FIFO_STATUS             0x17

//SPI config bytes
#define RX_MODE                 0b00001011
#define TX_MODE                 0b00001010
#define DISABLE_AA              0b00000000
#define ENABLE_PIPE_ZERO        0b00000001
#define SETUP_ADDRESS           0b00000011 //Default
#define DISABLE_ARC             0b00000000
#define SETUP_RF                0b00100110
#define SETUP_PAYLOAD           0b00001000 //8 bytes payload
#define CHANNEL                 0b01100100
#define ADDRESS                 0b00000001 //3 bytes address
#define MaxTries                0x0F

#define Target                  0x00
#define Command                 0x01

/** INCLUDES *******************************************************/
#include <xc.h>
#include <spi.h>
#include <Millis.h>

/** VARIABLES ******************************************************/
char ID;
char Failed;
char Status;

/** PROTOTYPES *****************************************************/
void rfInit(void);
void writeRegister(char, char);
char sendMessage(char, char, char*);
void send(void);
char txReady(void);
char checkMessage(char*);
char spiSendRead(char);
char checkMessage(char* data);
char readEEPROM(char);
void writeEEPROM(char, char);
#endif	/* COMS_H */