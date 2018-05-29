
#include "ADC.h"

#include <avr/io.h>

/**
 * Initialize the on-board ADC module for taking measurements.
 */
void InitializeADC (void)
{
    static unsigned char adcInitState = 0x00;

    if (adcInitState) {
        return;
    }

    ADMUX = (1 << REFS0);   // Use AVCC (5V) reference source
    //Set clocking for ADC between 50 and 200kHz by prescalling the system clock (16MHz / 128)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADEN);  //Enable ADC module

    adcInitState = 0x01;
}
