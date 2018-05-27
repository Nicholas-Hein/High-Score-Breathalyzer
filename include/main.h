
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



#define STANDBY_TIMEOUT 100

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
