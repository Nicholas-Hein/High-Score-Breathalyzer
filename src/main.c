#include "BluetoothControl.h"
#include "EthanolSensorControl.h"
#include "PowerSourceControl.h"
#include "EEPROMManager.h"
#include "main.h"

#include <avr/util/delay.h>
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

unsigned char Initialize (void)
{
    unsigned char initcode = 0x00;
    initcode |= BluetoothInitialize();
    PowerSourceInitialize();
    EthanolSensorInitialize();

    return initcode;
}

void Life (void)
{
    while (1) {
        Activity();
        Standby();
    }
}

void Activity (void)
{
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

void ExecuteCommand (char *cmd)
{
    char *msg = (char *)malloc(BT_COMMAND_MSG_SIZE * sizeof(unsigned char));
    if (cmd[0] == CMD_OK) {
        msg = (char [BT_COMMAND_MSG_SIZE]){ ACK, OVER };
        BluetoothSend(msg);
    } else if (cmd[0] == CMD_MEASUREBAC) {
        double val = EthanolSensorMeasureBAC();
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

char *ConvertDouble (double *d)
{
    size_t dataSize = sizeof(double) + 1;
    char *data = (char *)malloc(dataSize);
    memcpy(data, d, sizeof(double));
    data[dataSize - 1] = OVER;
    return data;
}

void Standby (void)
{
    while (!BluetoothIsPaired()) {
        //Standby
        _delay_ms(STANDBY_TIMEOUT);
    }
}

void Death (unsigned char initcode)
{
    while (1) {
        if (initcode & (1 << RQST_BLUETOOTH_BAUD_INDEX)) {
            //BAUD error
        } else if (initcode & (1 << RQST_BLUETOOTH_NAME_INDEX)) {
            //Name error
        } else if (initcode & (1 << RQST_BLUETOOTH_PIN_INDEX)) {
            //PIN error
        } else {
            //Other error
        }
        _delay_ms(STANDBY_TIMEOUT);
    }
}
