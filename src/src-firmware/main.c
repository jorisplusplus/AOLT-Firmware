/** INCLUDES ********************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xc.h>
#include <p18f2550.h>
#include <user_def.h>
#include <lcd.h>
#include <coms.h>
#include <commands.h>
#include <Millis.h>

/** DEFINES *********************************************/
#define _xtal_freq 48000000
#define Trigger PORTCbits.RC0

/** PROTOTYPES ******************************************/
void handleRF(void);
void handleTrigger(void);
void PrintStats(void);

/** VARIABLES *******************************************/
char readBuffer[8];
char writeBuffer[6];
char message[17];
char temp[16];
char SecondLine[16];
char Health;
char MaxHealth;
char Lives;
char InfiniteLives;
unsigned int Ammo;
unsigned int MaxAmmo;
char InfiniteAmmo;
const char WaitingState[16]     = "Waiting....";
const char StartingState[16]    = "Connected....";
const char Stats[16]            = "H:   L:   A:    ";
const char Listening[16]        = "ID:";
const char Death[16]            = "You are dead    ";
const char GameEnd[16]          = "Game over       ";
const char RespawningText[16]   = "Respawning      ";
const char EmtpyLine[16]        = "                ";
const char GoodLuck[16]         = "Good luck       ";
const char OutOfAmmo[16]        = "Out of ammo     ";
char Released;
char GameState;
char SwitchedState;
char UpdateStats;
char BlinkAmount;
char BlinkText;
unsigned long LCDBlink;

void main(void)
{
    TRISB = 0xF1; //Set SDI to input
    TRISA = 0xC0;
    TRISCbits.TRISC7 = 0;
    TRISCbits.TRISC6 = 0;
    IRLed = 0;
    initDisplay();
    rfInit();
    print(0,0,(char*)WaitingState);
    utoa(temp, ID, 10);
    strcpy(message,Listening);
    strcat(message, temp);
    print(1,0,message);
    T0CON = 0x07;
    INTCONbits.TMR0IE = 0;
    T1CON = 0b11110000;
    TMR1H = 0x00;
    TMR1L = 0x00;
    PIE1bits.TMR1IE = 0;
    Released = 1;
    GameState = Waiting;
    SwitchedState = 1;
    UpdateStats = 1;
    initMillis();
    while(1) {
        handleMillis();
        handleRF();
        switch(GameState) {
            case Starting :
                if(SwitchedState) {
                    clearDisplay();
                    print(0, 0, (char*)StartingState);
                    setPos(1,0);
                    SwitchedState = 0;
                }
                break;
            case Playing :
                if(SwitchedState) {
                    clearDisplay();
                    SwitchedState = 0;
                    UpdateStats = 1;
                    Health = MaxHealth;
                    Ammo = MaxAmmo;
                }
                if(UpdateStats) {
                PrintStats();
                setPos(1,0);
                UpdateStats = 0;
                }
                handleTrigger();
                if(PORTB < 0xF0 && T1CONbits.TMR1ON == 0) {
                    T1CONbits.TMR1ON = 1;
                }
                if((PORTB >= 0xF0 && T1CONbits.TMR1ON == 1) || IRLed == 1) {
                    T1CONbits.TMR1ON = 0;
                    TMR1H = 0x00;
                    TMR1L = 0x00;
                }
                if((TMR1L >= 0xDC && TMR1H == 0x05) || TMR1H > 0x05){ //Hit for more than 1 MS
                    TMR1H = 0x00;
                    TMR1L = 0x00;
                    Health--;
                    if(Health == 0){                        
                        if(Lives == 1 && InfiniteLives == 0){
                            Lives--;
                            clearDisplay();
                            strcpy(SecondLine, Death);
                            BlinkAmount = 10;
                            BlinkText = 1;
                            LCDBlink = Millis;
                            print(1, 0, (char*)Death);
                            sendMessage(0x00, Dead, writeBuffer);
                            GameState = NoLives;
                            SwitchedState = 1;
                        } else {
                            if(InfiniteLives == 0)Lives--;
                            strcpy(SecondLine, RespawningText);
                            BlinkAmount = 9;
                            BlinkText = 1;
                            LCDBlink = Millis;
                            print(1, 0, (char*)RespawningText);
                            sendMessage(0x00, Respawn, writeBuffer);
                            GameState = Respawning;
                            SwitchedState = 1;
                        }
                        
                    }
                    PrintStats();
                    setPos(1,0);                    
                }
                break;
            case GameOver :
                 if(SwitchedState) {
                     clearDisplay();
                     print(0, 0, (char*)GameEnd);
                     setPos(1,0);
                     SwitchedState = 0;
                 }
                break;
            case Respawning :
                if(SwitchedState) {
                    print(0, 0, (char*)GoodLuck);
                    SwitchedState = 0;
                    Ammo = MaxAmmo;
                    Health = MaxHealth;
                }
                if(BlinkAmount == 0) {
                    SwitchedState = 1;
                    GameState = Playing;
                }
                break;
            case NoLives :
                if(SwitchedState) {
                    print(0, 0, (char*)GameEnd);
                    SwitchedState = 0;
                }
        }
        if(BlinkAmount > 0) {
            if(LCDBlink + 1000 < Millis) {
                if(BlinkText) {
                    print(1, 0, (char*)EmtpyLine);
                    BlinkText = 0;
                } else {
                    print(1, 0, &SecondLine);
                    BlinkText = 1;
                }
                LCDBlink = Millis;
                BlinkAmount--;
            }
        }
    }
    }

void handleRF(void) {
        if(checkMessage(readBuffer)){
            switch(readBuffer[Command]){
                case Status :
                    writeBuffer[0] = ID;
                    writeBuffer[1] = Ammo >> 8;
                    writeBuffer[2] = Ammo;
                    writeBuffer[3] = Health;
                    writeBuffer[4] = Lives;
                    writeBuffer[5] = GameState;
                    sendMessage(0x00,Status,&writeBuffer);
                    break;
                case DisplayText :
                    BlinkAmount = readBuffer[2];
                    SecondLine[0] = readBuffer[3];
                    SecondLine[1] = readBuffer[4];
                    SecondLine[2] = readBuffer[5];
                    SecondLine[3] = readBuffer[6];
                    LCDBlink = Millis;
                    BlinkText = 1;
                    print(1, 0, &SecondLine);
                    break;
                case LoadText :
                    SecondLine[15] = SecondLine[9];
                    SecondLine[14] = SecondLine[8];
                    SecondLine[13] = SecondLine[7];
                    SecondLine[12] = SecondLine[6];
                    SecondLine[11] = SecondLine[5];
                    SecondLine[10] = SecondLine[4];
                    SecondLine[9] = readBuffer[7];
                    SecondLine[8] = readBuffer[6];
                    SecondLine[7] = readBuffer[5];
                    SecondLine[6] = readBuffer[4];
                    SecondLine[5] = readBuffer[3];
                    SecondLine[4] = readBuffer[2];
                    break;
                case SetAmmo :
                    Ammo = (readBuffer[2] << 8) + readBuffer[3];
                    MaxAmmo = (readBuffer[4] << 8) + readBuffer[5];
                    InfiniteAmmo = readBuffer[6];
                    UpdateStats = 1;
                    break;
                case SetHealth :
                    Health = readBuffer[2];
                    MaxHealth = readBuffer[3];
                    UpdateStats = 1;
                    break;
                case SetLives :
                    Lives = readBuffer[2];
                    InfiniteLives = readBuffer[3];
                    UpdateStats = 1;
                    break;
                case SetAll :
                    MaxAmmo = (readBuffer[2] << 8) + readBuffer[3];
                    InfiniteAmmo = readBuffer[4];
                    MaxHealth = readBuffer[5];
                    Lives = readBuffer[6];
                    InfiniteLives = readBuffer[7];
                    break;
                case SetID :
                    writeEEPROM(0x20, readBuffer[2]);
                    ID = readBuffer[2];
                    break;
                case SetDeath :
                    Health = 0;
                    break;
                case GetAmmo :
                    writeBuffer[0] = Ammo >> 8;
                    writeBuffer[1] = Ammo;
                    writeBuffer[2] = MaxAmmo >> 8;
                    writeBuffer[3] = MaxAmmo;
                    writeBuffer[4] = InfiniteAmmo;
                    sendMessage(0x00, GetAmmo, &writeBuffer);
                    break;
                case GetHealth :
                    writeBuffer[0] = Health;
                    writeBuffer[1] = MaxHealth;
                    sendMessage(0x00, GetHealth, &writeBuffer);
                    break;
                case GetLives :
                    writeBuffer[0] = Lives;
                    writeBuffer[1] = InfiniteLives;
                    Delay1KTCYx(1);
                    sendMessage(0x00, GetLives, &writeBuffer);
                    break;
                case SetGameState :
                    GameState = readBuffer[2];
                    SwitchedState = 1;
                    break;
            }
        }
}

void handleTrigger(void) {
    if(Trigger == 0) {
        if(Released && (Ammo > 0 || InfiniteAmmo > 0) && GameState == Playing) {
            Released = 0;
            IRLed = 1;
            T0CONbits.TMR0ON = 1;
            writeBuffer[0] = 0x01;
            writeBuffer[1] = 0x02;
            if(InfiniteAmmo == 0) {
                Ammo--;
                if(Ammo == 0) {
                    sendMessage(0x00, NoAmmo, &writeBuffer); //writebuffer not important.
                }
                UpdateStats = 1;
                if(Ammo == 0){
                    BlinkAmount = 10;
                    BlinkText = 1;
                    LCDBlink = Millis;
                    print(1, 0, (char*)OutOfAmmo);
                }
            }
            }
    } else if(T0CONbits.TMR0ON == 0) {
        Released = 1;
        }
    if(TMR0L >= 96) { //Disable IR led after 2 ms on
        IRLed = 0;
        TMR0H = 0;
        TMR0L = 0;
        T0CONbits.TMR0ON = 0;
        }
}

void PrintStats(void) {    
    strcpy(message, Stats);
    utoa(temp, Health, 10);
    message[2] = temp[0];
    if(temp[1] != '\0') message[3] = temp[1];    
    utoa(temp, Lives, 10);
    message[7] = temp[0];
    if(temp[1] != '\0') message[8] = temp[1];
    utoa(temp, Ammo, 10);
    message[12] = temp[0];
    if(temp[1] != '\0') {
        message[13] = temp[1];
        if(temp[2] != '\0') {
            message[14] = temp[2];
            if(temp[3] != '\0') message[15] = temp[3];
        }

    }
    print(0, 0, message);
}