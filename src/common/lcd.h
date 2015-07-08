#ifndef LCD
/** DEFINES *******************************************************/
#define LCD
#define PORT_DATA PORTA
#define PIN_ENABLE LATAbits.LATA5
#define PIN_RS LATAbits.LATA4

/** PROTOTYPES ****************************************************/
void initDisplay(void);
void setOutput(char);
void pulseEnable(void);
void pulseEnableLong(void);
void sendCommand(char);
void sendData(char);
void print(char, char, char*);
void setPos(char, char);
void clearDisplay(void);

#endif
