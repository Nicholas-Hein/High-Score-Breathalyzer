
#include "BluetoothControl.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>

int usart0SendByte (char data, FILE *stream)
{
    if (data == '\n') {
        usart0SendByte('\r', stream);
    }
    while (!(UCSRA & (1 << UDRE))) ;
    UDR = data;
    return 0;
}

int usart0ReceiveByte (FILE *stream)
{
    while (!(UCSRA & (1 << RXC))) ;
    return UDR;
}

FILE USART0_OStream = FDEV_SETUP_STREAM(usart0SendByte, NULL, _FDEV_SETUP_WRITE);
FILE USART0_IStream = FDEV_SETUP_STREAM(NULL, usart0ReceiveByte, _FDEV_SETUP_READ);

void usartInitialize (void)
{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

#if USE_2X
    UCSRA |= (1 << U2X);
#else
    UCSRA &= ~(1 << U2X);
#endif

    UCSRC = (1 << UCSZ1) | (1 << UCSZ0);     // 8-bit data
    UCSRB = (1 << RXEN) | (1 << TXEN);       // Enable RX and TX

    stdout = &USART0_OStream;
    stdin = &USART0_IStream;
}

unsigned char confirmAcknowledge (char *key, unsigned char size)
{
    unsigned char index = 0;
    unsigned char count = 0;

    while (index < size && count < CONFIRM_ACK_TIMEOUT) {
        if (key[index] == getchar()) {
            index++;
        } else {
            index = 0;
        }
        count++;
    }

    return (unsigned char)(count == CONFIRM_ACK_TIMEOUT);
}


/**
 * Initializes the bluetooth module.
 * @return  The initialization code.
 */
unsigned char BluetoothInitialize (void)
{
    static unsigned char bluetoothInitState = 0x00;
    if (bluetoothInitState) {
        return 0x00;
    }

    usartInitialize();

    char ack_baud [ACK_BLUETOOTH_BAUD_LENGTH] = ACK_BLUETOOTH_BAUD;
    puts(RQST_BLUETOOTH_BAUD);
    if (confirmAcknowledge(ack_baud, ACK_BLUETOOTH_BAUD_LENGTH)) {
        return (1 << RQST_BLUETOOTH_BAUD_INDEX);
    }

    char ack_name [ACK_BLUETOOTH_NAME_LENGTH] = ACK_BLUETOOTH_NAME;
    puts(RQST_BLUETOOTH_NAME);
    if (confirmAcknowledge(ack_name, ACK_BLUETOOTH_NAME_LENGTH)) {
        return (1 << RQST_BLUETOOTH_NAME_INDEX);
    }

    char ack_pin [ACK_BLUETOOTH_PIN_LENGTH] = ACK_BLUETOOTH_PIN;
    puts(RQST_BLUETOOTH_PIN);
    if (confirmAcknowledge(ack_pin, ACK_BLUETOOTH_PIN_LENGTH)) {
        return (1 << RQST_BLUETOOTH_PIN_INDEX);
    }

    bluetoothInitState = 0x01;
    return 0x00;
}

/**
 * If the bluetooth is paired.
 * @return  Is bluetooth paired.
 */
unsigned char BluetoothIsPaired (void)
{
    char ack [ACK_BLUETOOTH_PAIRED_LENGTH] = ACK_BLUETOOTH_PAIRED;
    puts(RQST_BLUETOOTH_PAIRED);
    if (confirmAcknowledge(ack, ACK_BLUETOOTH_PAIRED_LENGTH)) {
        return 0x00;
    }
    return 0x01;
}

/**
 * Sends data to the bluetooth module.
 * @param  data The data to be sent.
 * @return      Is sent.
 */
unsigned char BluetoothSend (char *data)
{
    if (!BluetoothIsPaired()) {
        return 0x00;
    }
    puts(data);
    return 0x01;
}

/**
 * Receives data from the bluetooth module.
 * @param  data The data received.
 * @return      Is received.
 */
unsigned char BluetoothReceive (char *data)
{
    if (!BluetoothIsPaired()) {
        return 0x00;
    }
    char d [BLUETOOTH_RECEIVE_SIZE];
    char input = getchar();
    unsigned char length = 0;
    while (input != BLUETOOTH_DELIMITER && length < BLUETOOTH_RECEIVE_SIZE) {
        d[length++] = input;
        input = getchar();
    }
    data = d;
    return 0x01;
}
