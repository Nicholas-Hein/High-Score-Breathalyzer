
#ifndef EEPROMMANAGER_H
#define EEPROMMANAGER_H

#define EEPROM_START 1

#define EEPROM_BEGIN 0xFF
#define EEPROM_END 0xFF

// Lowest score is latest
// Top 24 are highest
#define EEPROM_SCORE_SIZE 25

/**
 * Reads all scores from saved data.
 */
double *ReadScores (void);
/**
 * Adds a score to the saved data (in order).
 */
void AddScore (double score);


#endif
