
#ifndef ETHANOLSENSORCONTROL_H
#define ETHANOLSENSORCONTROL_H

#ifndef VREF
#define VREF 5.0
#endif

#define MIN_PPM 25
#define MAX_PPM 500


#define SAMPLE_COUNT 20
#define SAMPLE_PERIOD 250
#define SAMPLE_INFLECTIONPOINT (SAMPLE_COUNT * SAMPLE_PERIOD / 1000.0)

/**
 * Initializes the ethanol sensor module.
 */
void EthanolSensorInitialize (void);
/**
 * Measures BAC from the ethanol sensor.
 * Has a progress callback.
 */
double EthanolSensorMeasureBAC (void (*callback)(double bac));

#endif
