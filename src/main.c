#include "BluetoothControl.h"
#include "main.h"

#include <util/delay.h>

int main(void)
{
    while (1) {
        uint8_t initcode = Initialize();
        if (initcode == 0x00) {
            Life();
        } else {
            Death();
        }
    }
}

uint8_t Initialize (void)
{
    uint8_t initcode = 0x00;
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
    BluetoothSend({ ACK, OVER });
    while (BluetoothIsPaired()) {
        BluetoothSend({ RDY, OVER });

        char *cmd;
        if (BluetoothReceive(cmd)) {
            ExecuteCommand(cmd);
        }
    }
    BluetoothSend({ NACK, OVER });
}

void ExecuteCommand (char *cmd)
{
    if (cmd[0] == CMD_OK) {
        BluetoothSend({ ACK, OVER });
    } else if (cmd[0] == CMD_MEASUREBAC) {
        double val = EthanolSensorMeasureBAC();
        TransmitDouble(val);
    } else if (cmd[0] == CMD_MEASUREBAT) {
        double val = PowerSourceMeasureBattery();
        TransmitDouble(val);
    } else {
        BluetoothSend({ ERR, OVER });
    }
}

void TransmitDouble (double *d)
{
    size_t dataSize = sizeof(double) + 1;
    char *data = (char *)malloc(dataSize);
    memcpy(data, d, sizeof(double));
    data[dataSize - 1] = OVER;

    BluetoothSend(data);
}

void Standby (void)
{
    while (!BluetoothIsPaired()) {
        //Standby
        _delay_ms(STANDBY_TIMEOUT);
    }
}

void Death (void)
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
