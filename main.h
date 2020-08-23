#include <RTOSDemoConfig.h>
#include <Benchmarks.h>
//#include <InterruptHandler.h>
#include <LCD_function.h>
#include <Energy_ResponseTime_Prediction.h>
#include <BackupRestoreHandler.h>
#include <math.h> //use for ceiling function
#include <Sharp128x128.h>
#include <Initial.h>
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* TI includes. */
#include "driverlib.h"
#include <stdio.h>
#include <string.h>
#include <msp430.h>
#include <stdlib.h>
#include <time.h>

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook ( void );
void vApplicationIdleHook ( void );
void vApplicationStackOverflowHook ( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook ( void );
void App_ECG_Sensor ( void );
void App_Temp_Sensor ( void );
void App_Matrix_Multi ( void );
void App_Transmission ( void );

extern void Mydelay( float );
extern void System_initial( void );
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.
	See http://www.freertos.org/Stacks-and-stack-overflow-checking.html */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    printf("time\n");
	printf("time:%d idle\n",(int)xTaskGetTickCount());
	//printf("cur %f \n",1.5);
    __bis_SR_register ( LPM4_bits + GIE );
    __no_operation();
}
/*-----------------------------------------------------------*/


/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
//    const unsigned short usACLK_Frequency_Hz = 32768;

	/* Ensure the timer is stopped. */
	TA0CTL = 0;

	/* Run the timer from the ACLK. */
	TA0CTL = TASSEL_1;

	/* Clear everything to start with. */
	TA0CTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
//	TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

	/* Enable the interrupts. */
	TA0CCTL0 = CCIE;

	/* Start up clean. */
	TA0CTL |= TACLR;

	/* Up mode. */
	TA0CTL |= MC_1;
}
/*-----------------------------------------------------------*/

int _system_pre_init( void )
{
    /* Stop Watchdog timer. */
    WDT_A_hold( __MSP430_BASEADDRESS_WDT_A__ );

    /* Return 1 for segments to be initialised. */
    return 1;
}

void Mydelay ( float delay_ms )
{
	static int Globaldelaytime;  //used to My_delay function
	Globaldelaytime = delay_ms;
	do
	{
	    Globaldelaytime--;
		__delay_cycles ( 7000 ); //it should 8000 but use 7000 is closed to 1ms
    }
	while ( Globaldelaytime != 0 );
}
