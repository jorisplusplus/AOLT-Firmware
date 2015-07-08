/* 
 * File:   commands.h
 * Author: Joris
 *
 * Created on July 15, 2014, 12:12 AM
 */

#ifndef COMMANDS_H
#define	COMMANDS_H

#define Status              0x00
#define DisplayText         0x01
#define LoadText            0x02
#define SetAmmo             0x03
#define SetHealth           0x04
#define SetLives            0x05
#define SetAll              0x06
#define SetID               0x07
#define SetDeath            0x08
#define GetAmmo             0x09
#define GetHealth           0x0A
#define GetLives            0x0B
#define SetGameState        0x0C
#define Respawn             0x0D
#define Dead                0x0E
#define NoAmmo              0x0F

//GameStates
#define Waiting             0x00
#define Starting            0x01
#define Playing             0x02
#define GameOver            0x03
#define Respawning          0x04
#define NoLives             0x05

#endif	/* COMMANDS_H */