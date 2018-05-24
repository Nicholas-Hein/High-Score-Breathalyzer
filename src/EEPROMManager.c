
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

uint8_t readScore (uint8_t index, double *data)
{
    unsigned int baseAddr = (EEPROM_START + 1) + sizeof(double) * index;
    unsigned char nextDouble [sizeof(double)];
    unsigned char data;

    for (uint8_t i = 0; i < sizeof(double); i++) {
        data = eepromRead(baseAddr + i);
        if (data != EEPROM_END) {
            nextDouble[i] = data;
        } else {
            return 0x00;
        }
    }

    memcpy(data, nextDouble, sizeof(double));
    return 0x01;
}

double *ReadScores (void)
{
    double scores[EEPROM_SCORE_SIZE];
    unsigned char begin = eepromRead(EEPROM_START);

    if (begin == EEPROM_BEGIN) {
        for (uint8_t i = 0; i < EEPROM_SCORE_SIZE; i++) {
            double val;
            if (readScore(i, val)) {
                scores[i] = val;
            } else {
                break;
            }
        }
    } else {
        eepromWrite(EEPROM_START, EEPROM_BEGIN);
        eepromWrite(EEPROM_START + )
    }

    return scores;
}

void sortScores (double *scores)
{
    uint8_t mindex;
    for (uint8_t i = 0; i < EEPROM_SCORE_SIZE; i++) {
        mindex = i;
        for (uint8_t j = i; j < EEPROM_SCORE_SIZE; j++) {
            if (scores[j] < scores[mindex]) {
                mindex = j;
            }
        }
        double temp = scores[i];
        scores[i] = scores[mindex];
        scores[mindex] = temp;
    }
}

void writeScore (uint8_t index, double score)
{
    uint8_t baseAddr = EEPROM_START + sizeof(double) * index + 1;
    unsigned char val [sizeof(double)];
    memcpy(val, &score, sizeof(double));

    for (uint8_t i = 0; i < sizeof(double); i++) {
        eepromWrite(baseAddr + i, val[i]);
    }
}

void writeScores (double *scores)
{
    eepromWrite(EEPROM_START, EEPROM_BEGIN);
    for (uint8_t i = 0; i < EEPROM_SCORE_SIZE; i++) {
        writeScore(i, scores[i]);
    }

    uint8_t lastAddr = EEPROM_START + sizeof(double) * EEPROM_SCORE_SIZE + 1;
    eepromWrite(lastAddr, EEPROM_END);
}

void AddScore (double score)
{
    double *scores = ReadScores();
    sortScores(*scores);
    scores[0] = score;
    sortScores(*scores);
    writeScores(*scores);
}
