#include <msp430.h>
#include "driverlib.h"

void prvSetupHardware( void );
void initClock ( void );
void initUART ( void );
void initInterBut ( void );
void init_ADC ( void );
void initRTC ( void );
