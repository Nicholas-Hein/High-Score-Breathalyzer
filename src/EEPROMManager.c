
#include "EEPROMManager.h"

#include <avr/avr/io.h>
#include <avr/stdlib.h>
#include <avr/string.h>

void eepromWrite (unsigned int addr, unsigned char data)
{
    // Wait for previous write
    while (EECR & (1 << EEWE)) ;

    EEAR = addr;            // Set address
    EEDR = data;            // Set data
    EECR |= (1 << EEMWE);   // Enable master write
    EECR |= (1 << EEWE);    // Start EEPROM write
}

unsigned char eepromRead (unsigned int addr)
{
    // Wait for previous write
    while (EECR & (1 << EEWE)) ;

    EEAR = addr;            // Set address
    EECR |= (1 << EERE);    // Start EEPROM read
    return EEDR;            // Read data
}

unsigned char readScore (unsigned char index, double *data)
{
    unsigned int baseAddr = (EEPROM_START + 1) + sizeof(double) * index;
    unsigned char nextDouble [sizeof(double)];
    unsigned char item;

    for (unsigned char i = 0; i < sizeof(double); i++) {
        item = eepromRead(baseAddr + i);
        if (item != EEPROM_END) {
            nextDouble[i] = item;
        } else {
            return 0x00;
        }
    }

    memcpy(data, nextDouble, sizeof(double));
    return 0x01;
}

/**
 * Reads all scores from saved data.
 * @return  An array of scores of size EEPROM_SCORE_SIZE
 */
double *ReadScores (void)
{
    double *scores = (double *)malloc(EEPROM_SCORE_SIZE * sizeof(double));
    unsigned char begin = eepromRead(EEPROM_START);

    if (begin == EEPROM_BEGIN) {
        for (unsigned char i = 0; i < EEPROM_SCORE_SIZE; i++) {
            double val;
            if (readScore(i, &val)) {
                scores[i] = val;
            } else {
                break;
            }
        }
    } else {
        eepromWrite(EEPROM_START, EEPROM_BEGIN);
        eepromWrite(EEPROM_START, EEPROM_END);
    }

    return scores;
}

void sortScores (double *scores)
{
    unsigned char mindex;
    for (unsigned char i = 0; i < EEPROM_SCORE_SIZE; i++) {
        mindex = i;
        for (unsigned char j = i; j < EEPROM_SCORE_SIZE; j++) {
            if (scores[j] < scores[mindex]) {
                mindex = j;
            }
        }
        double temp = scores[i];
        scores[i] = scores[mindex];
        scores[mindex] = temp;
    }
}

void writeScore (unsigned char index, double score)
{
    unsigned char baseAddr = EEPROM_START + sizeof(double) * index + 1;
    unsigned char val [sizeof(double)];
    memcpy(val, &score, sizeof(double));

    for (unsigned char i = 0; i < sizeof(double); i++) {
        eepromWrite(baseAddr + i, val[i]);
    }
}

void writeScores (double *scores)
{
    eepromWrite(EEPROM_START, EEPROM_BEGIN);
    for (unsigned char i = 0; i < EEPROM_SCORE_SIZE; i++) {
        writeScore(i, scores[i]);
    }

    unsigned char lastAddr = EEPROM_START + sizeof(double) * EEPROM_SCORE_SIZE + 1;
    eepromWrite(lastAddr, EEPROM_END);
}

/**
 * Adds a score to the saved data (in order).
 * @param score The score to be added.
 */
void AddScore (double score)
{
    double *scores = ReadScores();
    sortScores(scores);
    scores[0] = score;
    sortScores(scores);
    writeScores(scores);
}
