
#ifndef EEPROMMANAGER_H
#define EEPROMMANAGER_H

#define EEPROM_START 1

#define EEPROM_BEGIN 0xFF
#define EEPROM_END 0xFF

#define EEPROM_SCORE_SIZE 25
// Lowest score is latest
// Top 24 are highest


double *ReadScores (void);
void AddScore (double score);


#endif