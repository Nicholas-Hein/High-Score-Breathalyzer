#include "BluetoothControl.h"
#include "EthanolSensorControl.h"
#include "PowerSourceControl.h"
#include "EEPROMManager.h"
#include "main.h"

#include <avr/util/delay.h>
#include <avr/avr/io.h>
#include <avr/string.h>
#include <avr/stdlib.h>

int main(void)
{
    while (1) {
        unsigned char initcode = Initialize();
        if (initcode == 0x00) {
            Life();
        } else {
            Death(initcode);
        }
    }
}

/**
 * Initializes the High-Score-Breathalyzer.
 * @return  The initialization code.
 */
unsigned char Initialize (void)
{
    unsigned char initcode = 0x00;
    initcode |= BluetoothInitialize();
    PowerSourceInitialize();
    EthanolSensorInitialize();

    DDRD |= 0xE0;

    return initcode;
}

void changeStatusLED (unsigned char rgb)
{
    rgb &= COLOR_VALUE_MASK;

    if (rgb &= COLOR_RED) {
        PORTD |= (COLOR_RED << COLOR_PIN_OFFSET);
    } else {
        PORTD &= ~(COLOR_RED << COLOR_PIN_OFFSET);
    }
    if (rgb &= COLOR_GREEN) {
        PORTD |= (COLOR_GREEN << COLOR_PIN_OFFSET);
    } else {
        PORTD &= ~(COLOR_GREEN << COLOR_PIN_OFFSET);
    }
    if (rgb &= COLOR_BLUE) {
        PORTD |= (COLOR_BLUE << COLOR_PIN_OFFSET);
    } else {
        PORTD &= ~(COLOR_BLUE << COLOR_PIN_OFFSET);
    }
}

unsigned char getStatusLED (void)
{
    return (PORTD & (COLOR_VALUE_MASK << COLOR_PIN_OFFSET)) >> COLOR_PIN_OFFSET;
}

/**
 * Processes upon successful initialization.
 */
void Life (void)
{
    while (1) {
        Activity();
        Standby();
    }
}

/**
 * Main activities.
 */
void Activity (void)
{
    changeStatusLED(COLOR_GREEN);
    char *message = (char *)malloc(BT_COMMAND_MSG_SIZE * sizeof(char));
    message = (char [BT_COMMAND_MSG_SIZE]){ ACK, OVER };
    BluetoothSend(message);
    while (BluetoothIsPaired()) {
        message = (char [BT_COMMAND_MSG_SIZE]){ RDY, OVER };
        BluetoothSend(message);

        char *cmd = (char *)malloc(sizeof(char) * BLUETOOTH_RECEIVE_SIZE);
        if (BluetoothReceive(cmd)) {
            ExecuteCommand(cmd);
        }
    }
    message = (char [BT_COMMAND_MSG_SIZE]){ NACK, OVER };
    BluetoothSend(message);
}

void unloadScores (void)
{
    unsigned char dataSize = sizeof(double) + 1;
    double *scores = ReadScores();
    char val [dataSize - 1];
    char *msg = (char *)malloc(dataSize);
    msg = val;
    msg[dataSize - 1] = RDY;

    for (int i = 0; i < EEPROM_SCORE_SIZE; i++) {
        memcpy(&val, &(scores[i]), sizeof(double));
        BluetoothSend(msg);
    }

    char overmsg [1] = { OVER };
    BluetoothSend(overmsg);
}

void bacProgressCallback (double bac)
{
    if (getStatusLED() & COLOR_BLUE) {
        changeStatusLED(COLOR_BLACK);
    } else {
        changeStatusLED(COLOR_BLUE);
    }

    BluetoothSend(ConvertDouble(&bac));
}

/**
 * Executes a command.
 * @param cmd The command to be executed.
 */
void ExecuteCommand (char *cmd)
{
    char *msg = (char *)malloc(BT_COMMAND_MSG_SIZE * sizeof(unsigned char));
    if (cmd[0] == CMD_OK) {
        msg = (char [BT_COMMAND_MSG_SIZE]){ ACK, OVER };
        BluetoothSend(msg);
    } else if (cmd[0] == CMD_MEASUREBAC) {
        double val = EthanolSensorMeasureBAC(&bacProgressCallback);
        BluetoothSend(ConvertDouble(&val));
    } else if (cmd[0] == CMD_MEASUREBAT) {
        double val = PowerSourceMeasureBattery();
        BluetoothSend(ConvertDouble(&val));
    } else if (cmd[0] == CMD_UNLOADSCORES) {
        unloadScores();
    } else {
        msg = (char [BT_COMMAND_MSG_SIZE]){ ERR, OVER };
        BluetoothSend(msg);
    }
    free(msg);
}

/**
 * Converts a double to its memory segments.
 * @param  d The double to be converted.
 * @return   The memory segments of size sizeof(double)
 */
char *ConvertDouble (double *d)
{
    size_t dataSize = sizeof(double) + 1;
    char *data = (char *)malloc(dataSize);
    memcpy(data, d, sizeof(double));
    data[dataSize - 1] = OVER;
    return data;
}

/**
 * Standby activities.
 */
void Standby (void)
{
    changeStatusLED(COLOR_YELLOW);
    while (!BluetoothIsPaired()) {
        //Standby
        _delay_ms(STANDBY_TIMEOUT);
    }
}

/**
 * Processes upon unsuccessful initialization.
 * @param initcode The unsuccessful initialization code.
 */
void Death (unsigned char initcode)
{
    while (1) {
        changeStatusLED(COLOR_RED);
        char flashCount;
        if (initcode & ((1 << RQST_BLUETOOTH_BAUD_INDEX)
                | (1 << RQST_BLUETOOTH_NAME_INDEX)
                | (1 << RQST_BLUETOOTH_PIN_INDEX))) {
            // Bluetooth setup error
            flashCount = ERRFLASH_BTSETUP;
        } else {
            // Other error
            flashCount = ERRFLASH_UNKNOWN;
        }

        for (char i = 0; i < flashCount; i++) {
            changeStatusLED(COLOR_BLACK);
            _delay_ms(DEATH_PERIOD);
            changeStatusLED(COLOR_RED);
            _delay_ms(DEATH_PERIOD);
        }
        _delay_ms(DEATH_TIMEOUT);
    }
}
