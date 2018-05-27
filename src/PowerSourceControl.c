
#include "ADC.h"
#include "PowerSourceControl.h"

#include <avr/avr/io.h>

void PowerSourceInitialize (void)
{
    static unsigned char pwrInitState = 0x00;
    if (pwrInitState) {
        return;
    }

    InitializeADC();
    DDRC &= ~0x01;          // Set up input register

    pwrInitState = 0x01;
}

double PowerSourceMeasureBattery (void)
{
    ADMUX &= ~(0x0F);       // Select analog channel
    ADCSRA |= (1 << ADSC);  // Start ADC conversion (ADC1)
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) ;
    return (double)ADCW / 1023.0;
}
