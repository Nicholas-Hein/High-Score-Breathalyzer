
#ifndef MAIN_H
#define MAIN_H

#define ACK 0xFF
#define NACK 0xAA
#define NON 0x00

#define RDY 0xF0
#define ERR 0x55
#define OVER 0x0F

#define CMD_OK 0x01
#define CMD_MEASUREBAC 0x02
#define CMD_MEASUREBAT 0x03
#define CMD_UNLOADSCORES 0x04


#define ERRFLASH_BTSETUP 5
#define ERRFLASH_UNKNOWN 8


#define COLOR_BLACK 0x00
#define COLOR_RED 0x01
#define COLOR_YELLOW 0x03
#define COLOR_GREEN 0x02
#define COLOR_CYAN 0x06
#define COLOR_BLUE 0x04
#define COLOR_MAGENTA 0x05
#define COLOR_WHITE 0x07
#define COLOR_VALUE_MASK 0x07

#define COLOR_PIN_OFFSET 5


#define STANDBY_TIMEOUT 100
#define DEATH_TIMEOUT 5000
#define DEATH_PERIOD 500

#define BT_COMMAND_MSG_SIZE 2


/**
 * Initializes the High-Score-Breathalyzer.
 */
unsigned char Initialize (void);
/**
 * Processes upon successful initialization.
 */
void Life (void);
/**
 * Processes upon unsuccessful initialization.
 */
void Death (unsigned char initcode);
/**
 * Main activities.
 */
void Activity (void);
/**
 * Standy activities.
 */
void Standby (void);

/**
 * Executes a command.
 */
void ExecuteCommand (char *cmd);
/**
 * Converts a double to its memory segments.
 */
char *ConvertDouble (double *d);

#endif
