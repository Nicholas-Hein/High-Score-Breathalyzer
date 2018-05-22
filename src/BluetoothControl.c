
#include "BluetoothControl.h"

#include <avr/stdio.h>
#include <avr/avr/io.h>
#include <avr/util/setbaud.h>



uint8_t BluetoothInitialize (void)
{
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

    return 0x00;
}

uint8_t BluetoothIsPaired (void)
{
    char ack [ACK_BLUETOOTH_PAIRED_LENGTH] = ACK_BLUETOOTH_PAIRED;
    puts(RQST_BLUETOOTH_PAIRED);
    if (confirmAcknowledge(ack, ACK_BLUETOOTH_PAIRED_LENGTH)) {
        return 0x00;
    }
    return 0x01;
}

uint8_t BluetoothSend (char *data)
{
    if (!BluetoothIsPaired()) {
        return 0x00;
    }
    puts(*data);
    return 0x01;
}

uint8_t BluetoothReceive (char *data)
{
    if (!BluetoothIsPaired()) {
        return 0x00;
    }
    char d [BLUETOOTH_RECEIVE_SIZE];
    char input = getchar();
    uint8_t length = 0;
    while (input != BLUETOOTH_DELIMITER && length < BLUETOOTH_RECEIVE_SIZE) {
        d[length++] = input;
        input = getchar();
    }
    data = d;
    return 0x01;
}


uint8_t confirmAcknowledge (char *key, uint8_t size)
{
    char input;
    uint8_t index = 0;
    uint8_t count = 0;

    while (index < size && count < CONFIRM_ACK_TIMEOUT) {
        if (*key[index] == getchar()) {
            index++;
        } else {
            index = 0;
        }
        count++;
    }

    return (uint8_t)(count == CONFIRM_ACK_TIMEOUT);
}

void usartInitialize (void)
{
    UBRR0H = UBRRH_VALUE;
    UBRROL = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);     // 8-bit data
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);       // Enable RX and TX

    stdout = &USART0_OStream;
    stdin = &USART0_IStream;
}

void usart0SendByte (char data, FILE *stream)
{
    if (data == '\n') {
        usart0SendByte('\r', stream);
    }
    while (!(UCSR0A & (1 << UDRE0))) ;
    UDR0 = data;
    return 0;
}

char usart0ReceiveByte (FILE *stream)
{
    while (!(UCSR0A & (1 << RXC0))) ;
    return UDR0;
}
