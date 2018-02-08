/*
 * cpufreq.h
 *
 * Created: 11/18/2017 2:35:25 AM
 *  Author: nichh
 */ 


#ifndef CPUFREQ_H_
#define CPUFREQ_H_

// CLKPR - AVR clock prescaler register.
// CLKPS[3:0] - Div
//    0 0 0 0     1
//    0 0 0 1     2
//    0 0 1 0     4
//    0 0 1 1     8
//    0 1 0 0    16
//    0 1 0 1    32
//    0 1 1 0    64
//    0 1 1 1   128
//    1 0 0 0   256

#define CLKPR_INIT (1 << CLKPCE)
#define CLKPR_8MHZ (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0)	// DIV by 1
#define CLKPR_4MHZ (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (1 << CLKPS0)	// DIV by 2
#define CLKPR_2MHZ (0 << CLKPS3) | (0 << CLKPS2) | (1 << CLKPS1) | (0 << CLKPS0)	// DIV by 4
#define CLKPR_1MHZ (0 << CLKPS3) | (0 << CLKPS2) | (1 << CLKPS1) | (1 << CLKPS0)	// DIV by 8 (default)

// NOTE: Requires avr/interrupt.h for cli() and sei() calls.
#define CLKPR_SET(CLKPR_VAL) { cli(); CLKPR = CLKPR_INIT; CLKPR = CLKPR_VAL; sei(); }

#endif /* CPUFREQ_H_ */