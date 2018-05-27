
#include "ADC.h"
#include "EEPROMManager.h"
#include "EthanolSensorControl.h"

#include <avr/math.h>
#include <avr/avr/io.h>

/**
 * Initializes the ethanol sensor module.
 */
void EthanolSensorInitialize (void)
{
    static unsigned char ethanolInitState = 0x00;
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

double measureBAC (void)
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
        AddScore(bac);
    }
    return bac;
}

/**
 * Measures BAC from the ethanol sensor module.
 * @param  callback A callback for progress.
 * @return          The averaged BAC.
 */
double EthanolSensorMeasureBAC (void (*callback)(double bac))
{
    double sumWeightedBAC = 0.0;
    double sumWeights = 0.0;

    double t;
    double bac;
    for (char i = 0; i < SAMPLE_COUNT; i++) {
        t = (double)i * (double)SAMPLE_PERIOD / 1000.0;
        bac = measureBAC();
        if (bac < 0) {
            return 0.0;
        }

        double coef = 1.0 / M_PI * atan(t - SAMPLE_INFLECTIONPOINT) + 0.5;
        bac *= coef;
        sumWeightedBAC += bac;
        sumWeights += coef;

        double tempResult = sumWeightedBAC / sumWeights;
        callback(tempResult);
    }

    return sumWeightedBAC / sumWeights;
}
