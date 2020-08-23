/*
 * Non-backup able task / Beneficial task
 * Sensing temperature on FR5969
 */
#include "tempSensor.h"
#include <msp430.h>
#include "stdio.h"
//ref 1.2
//#define CALADC12_12V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
//                                                      //See device datasheet for TLV table memory mapping
//#define CALADC12_12V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C
//ref 1.2
//ref 2
#define CALADC12_12V_30C  *((unsigned int *)0x1A1E)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_12V_85C  *((unsigned int *)0x1A20)   // Temperature Sensor Calibration-85 C
//ref 2
//ref 2.5
//#define CALADC12_12V_30C  *((unsigned int *)0x1A22)   // Temperature Sensor Calibration-30 C
//                                                      //See device datasheet for TLV table memory mapping
//#define CALADC12_12V_85C  *((unsigned int *)0x1A24)   // Temperature Sensor Calibration-85 C
////ref 2.5
unsigned int temp;
volatile float temperatureDegC;

/*
 * Get current temperature()
 */
void getTemp(float *ret)
{

	//ADC12CTL0 |= ADC12SC;               // Sampling and conversion start
	temp = ADC12MEM1;
	temperatureDegC = (float)(((long)temp - CALADC12_12V_30C) * (85 - 30)) /
	                (CALADC12_12V_85C - CALADC12_12V_30C) + 30.0f;
	*ret=temperatureDegC*100;
	return ;

}
