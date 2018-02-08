/*
 * main.h
 *
 * Created: 11/18/2017 2:03:33 PM
 *  Author: nichh
 */ 


#ifndef MAIN_H_
#define MAIN_H_

//////////////////////////////////////////////////////////////////////////
//Operation Parameters
//////////////////////////////////////////////////////////////////////////
//Define CPU Frequency
#ifndef F_CPU
#define F_CPU 1000000UL //1MHz Clock Speed
#endif

#define EEPROM_MEM_ADDR 12

#define VREF 5.0

//////////////////////////////////////////////////////////////////////////
//Program Parameters
//////////////////////////////////////////////////////////////////////////
//Define program delays
#define SPLASHDELAY 5000
#define WARMUPDELAY 7UL * 60UL * 1000UL
#define OVERRIDDENISPLAYDELAY 2000
#define RESETDELAY 5000
#define RESETDISPLAYDELAY 2000
#define SAMPLEDELAY 10000
#define SAMPLERESDELAY 250
#define SCOREDELAY 7000
#define HIGHSCOREDELAY 10000
#define HIGHSCORERESDELAY 25

//Define keycodes
#define SAMPLEKEYCODE 0x01
#define RESETKEYCODE 0x02
#define OVERRIDEKEYCODE 0x04

//Equation Constants
#define SAMPLEINFLECTIONPOINT 5.0f
#define MINPPM 70.0
#define MAXPPM 500.0

//////////////////////////////////////////////////////////////////////////
//Protocol Parameters
//////////////////////////////////////////////////////////////////////////
//Define USART BAUD Rate and UBRR
#define USART_BAUDRATE 9600
#define UBRR_VALUE ((F_CPU / (USART_BAUDRATE * 16UL)) - 1)

//Define TWI Parameters
#define SSD1306_SCL	PC5	// SCL,	Pin 3 on SSD1306 Board
#define SSD1306_SDA	PC4	// SDA,	Pin 4 on SSD1306 Board
#define SSD1306_SA	0x78 // Slave address


//////////////////////////////////////////////////////////////////////////
//Includes
//////////////////////////////////////////////////////////////////////////
//STD Library includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <math.h>

//Library Includes
#include "cpufreq.h"
#include "tinusaur/ssd1306xled.h"
#include "tinusaur/ssd1306xled8x16.h"


//////////////////////////////////////////////////////////////////////////
//Prototypes
//////////////////////////////////////////////////////////////////////////
//Initialize
void Initialize(void);

//Screens
void PlaySplashScreen(void);
void PlayWarmUpScreen(void);
bool PlayResetScreen(void);
float PlaySampling(void);
void PlayScore(float);
void PlayHighScore(float);

//Worker Functions
void LoadStorage(float *);
void SaveStorage(float);
float ReadBAC(void);
void DisplayProgress(char);

//Background Workers
bool ListenKeyPress(char *);
double ReadADC(void);
int USART0SendByte(char, FILE *);
void TransmitReading(float *, bool);

void ftoa(float, char *, int);
int intToStr(int, char [], int);

#endif /* MAIN_H_ */