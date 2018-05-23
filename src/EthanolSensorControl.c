
#include "ADC.h"
#include "EthanolSensorControl.h"

#include <avr/math.h>

void EthanolSensorInitialize (void)
{
    static uint8_t ethanolInitState = 0x00;
    if (ethanolInitState) {
        return;
    }

    InitializeADC();
    DDRC &= ~0x02;

    ethanolInitState = 0x01;
}

double measureADC (void)
{
    ADMUX &= ~(0x0E);
    ADMUX |= 0x01;          // Select analog channel (ADC1)
    //Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) ;
    return (double)ADCW / 1023.0;
}

double convertDCToPPM (double v)
{
    double ppm = 150.4351049 * pow(v, 5);
    ppm += -2244.75988 * pow(v, 4);
    ppm += 13308.5139 * pow(v, 3);
    ppm += -39136.08594 * pow(v, 2);
    ppm += 57082.6258 * pow(v, 1);
    ppm += -32982.05333;
    return ppm;
}

double EthanolSensorMeasureBAC (void)
{
    double v = measureADC() * VREF;
    double ppm = convertDCToPPM(v);

    double bac = 0.0;
    if (ppm < MIN_PPM) {
        bac = 0.0;
    } else if (ppm > MAX_PPM) {
        bac = -1.0;
    } else {
        bac = ppm / 2600.0;
    }
    return bac;
}
