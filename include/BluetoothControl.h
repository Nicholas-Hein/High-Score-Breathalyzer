
#ifndef BLUETOOTHCONTROL_H
#define BLUETOOTHCONTROL_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif



#define BLUETOOTH_NAME "HiScoreBreathalyzer"
#define BLUETOOTH_NAME_LENGTH 19

#define BLUETOOTH_PIN "0000"
#define BLUETOOTH_PIN_LENGTH 4



#define CONFIRM_ACK_TIMEOUT 100

#define BLUETOOTH_DELIMITER '\r'
#define BLUETOOTH_RECEIVE_SIZE 4



#define RQST_BLUETOOTH_BAUD_INDEX 0
#define RQST_BLUETOOTH_BAUD "AT+BAUD4"
#define ACK_BLUETOOTH_BAUD "OK9600"
#define ACK_BLUETOOTH_BAUD_LENGTH 7

#define RQST_BLUETOOTH_NAME_INDEX 1
#define RQST_BLUETOOTH_NAME "AT+NAME"BLUETOOTH_NAME
#define ACK_BLUETOOTH_NAME "OK"BLUETOOTH_NAME
#define ACK_BLUETOOTH_NAME_LENGTH (3 + BLUETOOTH_NAME_LENGTH)

#define RQST_BLUETOOTH_PIN_INDEX 2
#define RQST_BLUETOOTH_PIN "AT+PIN"BLUETOOTH_PIN
#define ACK_BLUETOOTH_PIN "OKsetpin"
#define ACK_BLUETOOTH_PIN_LENGTH 9


#define RQST_BLUETOOTH_PAIRED "AT"
#define ACK_BLUETOOTH_PAIRED "OK"
#define ACK_BLUETOOTH_PAIRED_LENGTH 3


/**
 * Initializes the bluetooth module.
 */
unsigned char BluetoothInitialize (void);
/**
 * Checks if the bluetooth is paired.
 */
unsigned char BluetoothIsPaired (void);

/**
 * Sends data to the bluetooth module.
 */
unsigned char BluetoothSend (char *data);
/**
 * Receives data from the bluetooth module.
 */
unsigned char BluetoothReceive (char *data);

#endif
