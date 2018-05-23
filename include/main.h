
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



#define STANDBY_TIMEOUT 100



void Initialize (void);
void Life (void);
void Death (void);
void Activity (void);
void Standby (void);

void ExecuteCommand (char *cmd);
void TransmitDouble (double *d);

#endif
