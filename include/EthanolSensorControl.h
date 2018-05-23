
#ifndef ETHANOLSENSORCONTROL_H
#define ETHANOLSENSORCONTROL_H

#ifndef VREF
#define VREF 5.0
#endif

#define MIN_PPM 25
#define MAX_PPM 500



void EthanolSensorInitialize (void);
double EthanolSensorMeasureBAC (void);

#endif
