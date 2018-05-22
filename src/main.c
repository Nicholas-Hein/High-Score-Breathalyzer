#include "BluetoothControl.h"
#include "main.h"

#include <util/delay.h>

int main(void)
{
    while (1) {
        Initialize();
        uint8_t initcode = BluetoothInitialize();
        if (initcode == 0x00) {
            Life();
        } else {
            Death();
        }
    }
}

void Initialize (void)
{
    
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
    while (BluetoothIsPaired()) {
        BluetoothSend({ ACK });
        //Activity
        BluetoothSend({ NACK });
    }
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
