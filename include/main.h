#ifndef MAIN_H
#define MAIN_H

#define ACK 0xFF
#define NACK 0xAA
#define NON 0x00

#define STANDBY_TIMEOUT 100



void Initialize (void);
void Life (void);
void Death (void);
void Activity (void);
void Standby (void);

#endif
