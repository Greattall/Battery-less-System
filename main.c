#include <main.h>
/* The heap is allocated here so the "persistent" qualifier can be used.  This
requires configAPPLICATION_ALLOCATED_HEAP to be set to 1 in FreeRTOSConfig.h.
See http://www.freertos.org/a00111.html for more information. */
//#pragma PERSISTENT( ucHeap ) 	/* CCS version. */
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] = { 0 };
/*-----------------------------------------------------------*/

void App_ECG_Sensor()
{
    /* Initialize variables */
    float f_ECG_Data;

    /* Infinity loop */
    while ( 1 )
    {
        if ( ECG_arrived == true )
        {
#if ( METHOD == METHOD_REP )
            /* Check Atomic section. */
            if ( CurrentVoltage < Atomic_Voltage ) Vbac = Atomic_Voltage;   // Enter backup mode.
#endif

            ECG_arrived = false;        // Clear flag.

#if ( METHOD == METHOD_REP )
            decision_t i_ECG_SenDec = Energy_ResponseTime_Prediction ( CurrentTime,
                                                                ECGTransDeadline,
                                                                ECG_Weight,
                                                                ECG_Sample_Rate,
                                                                CurrentVoltage );

            if ( i_ECG_SenDec == Drop_Task ) continue;  // Drop task, back to infinity loop
#endif

            /* Sensing. */
            int i;
            for ( i = 0; i < ECG_Weight; i++ )
            {
                getECG ( &f_ECG_Data );
                __delay_cycles ( 8000000 / ECG_Sample_Rate ); //Sampling interval
            }

            ECGenough = true;

            /* Task finished. */
            ECGProg++;
#if ( Enable_LCD == 1 )
            printProg ( Progress );
            printECG ( f_ECG_Data );
            printECGProg ( ECGProg );
#endif
        }

        /* Enter low power mode. */
        while ( ( Tempenough + ECGenough + MatrixCount + Metrix_arrived + Temp_arrived + ECG_arrived ) == 0)
        {
            __bis_SR_register ( LPM0_bits + GIE );
            __no_operation();
        }

        /* Context switch. */
        taskYIELD();
    }
}

void App_Temp_Sensor()
{
    /* Initialize variables */
	float f_Temp_DegC;

	/* Infinity loop */
    while ( 1 )
    {
		if ( Temp_arrived == true )
		{
#if ( METHOD == METHOD_REP)
            while ( ( ECG_arrived + ECGenough ) != 0 ) taskYIELD();

            /* Check Atomic section. */
            if ( CurrentVoltage < Atomic_Voltage ) Vbac = Atomic_Voltage;   // Enter backup mode.
#endif

            Temp_arrived = false;       // Clear flag.

#if ( METHOD == METHOD_REP)
            decision_t i_Temp_SenDec = Energy_ResponseTime_Prediction ( CurrentTime,
                                                             TempTransDeadline,
                                                             Temp_Weight,
                                                             Temp_Sample_Rate,
                                                             CurrentVoltage );
            if ( i_Temp_SenDec == Drop_Task ) continue;  // Drop task, back to infinity loop
#endif

            /* Sensing. */
            int i;
			for ( i = 0; i < Temp_Weight; i++ )
			{
				getTemp ( &f_Temp_DegC );
				__delay_cycles ( 8000000 / Temp_Sample_Rate );  //Sensing interval
			}

            Tempenough = true;

            /* Task finished. */
            TempProg++;
#if ( Enable_LCD == 1 )
            printProg ( Progress );
            printTemp ( f_Temp_DegC );
            printTempProg ( TempProg );
#endif
		}

		/* Enter low power mode. */
		while ( ( Tempenough + ECGenough + MatrixCount + Metrix_arrived + Temp_arrived + ECG_arrived ) == 0 )
		{
			__bis_SR_register ( LPM0_bits + GIE );
			__no_operation();
		}

		/* Context switch. */
		taskYIELD();
	}
}

#pragma PERSISTENT ( MatrixlocalDeadline )
unsigned int MatrixlocalDeadline = 0;
void App_Matrix_Multi()
{
    /* Initialize variables */
#if ( WORKLOAD == WORKLOAD_25 )   //20%
    int iteration = 80;
#elif ( WORKLOAD == WORKLOAD_50 ) //40%
    int iteration = 160;
#elif ( WORKLOAD == WORKLOAD_75 )  //60%
    int iteration = 240;
#elif ( WORKLOAD == WORKLOAD_LIGHT )  //light load
    int iteration = 9;
#elif ( WORKLOAD == WORKLOAD_MEDIUM )  //Middle Load
    int iteration = 118;
#elif ( WORKLOAD == WORKLOAD_HEAVY )  //HeavyLoad
    int iteration = 320;
#endif

	/* Infinity loop */
    while ( 1 )
    {
        /* Task arrived */
		if ( Metrix_arrived == true )
		{
		    MatrixlocalDeadline = MatrixRealDeadline;   // Latch current deadline value.
			Metrix_arrived = false;                     // Clear flag.

#if ( METHOD == METHOD_REP )
			decision_t i_Metrix_Dec = Energy_ResponseTime_Prediction ( CurrentTime,
                                                                MatrixlocalDeadline,
                                                                0,
                                                                0,
                                                                CurrentVoltage );

            if ( i_Metrix_Dec == Drop_Task ) continue;  // Drop task, back to infinity loop
#endif

            /* Do matrix multiplication */
            for ( MatrixCount = iteration; 0 < MatrixCount; MatrixCount-- )
            {
#if ( METHOD == METHOD_REP )
                /* Context switch. */
                while ( ( Tempenough + ECGenough + Temp_arrived + ECG_arrived ) != 0 ) taskYIELD();
#endif
                MatrixMultiplication();
            }

            /* Task finished. */
            Progress++;
            MultProg++;

#if ( Print_CIO == 1 )
            printf("Finish MatrixMultiplication\n");
#endif

#if ( Enable_LCD == 1 )
            printProg ( Progress );
            printMultProg ( MultProg );
#endif

            /* Check meet deadline or not. */
            Matrixtask_finish = ( CurrentTime <= MatrixlocalDeadline ) ? true : false;
		}

		/* Enter low power mode. */
		while ( ( Tempenough + ECGenough + MatrixCount + Metrix_arrived + Temp_arrived + ECG_arrived ) == 0 )
		{
			__bis_SR_register ( LPM0_bits + GIE );
			__no_operation();
		}

		/* Context switch. */
		taskYIELD();
    }
}

#pragma PERSISTENT ( ECGlocalTransDeadline )
#pragma PERSISTENT ( TemplocalTransDeadline )
unsigned int ECGlocalTransDeadline = 0;
unsigned int TemplocalTransDeadline = 0;
void App_Transmission()
{
    /* Initialize variables */
    int TtransTimes, TtransTimess;
    char ECGTransData = '1';
    char TempTransData = '2';

    /* Infinity loop */
	while ( 1 )
	{


//**************************************Trans ECG******************************************
		if ( ECGenough == true )
		{
		    ECGenough = false;                              // Clear flag.
			ECGlocalTransDeadline = ECGTransDeadline;   // Latch deadline value.

#if ( Print_CIO == 1 )
            printf("trans\n");
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~transmission~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			sending = true;                                    // Set flag.
			for ( TtransTimes = 0; TtransTimes < ECG_Weight * 2 / 100; TtransTimes++ )
			{
//			    EUSCI_A_UART_transmitData ( EUSCI_A1_BASE , 'H' );
			    UCA1TXBUF='H';
                while(!(UCA1IFG & UCTXIFG));
				for ( TtransTimess = 0; TtransTimess < 100; TtransTimess++ )
				{
//				    EUSCI_A_UART_transmitData ( EUSCI_A1_BASE , ECGTransData );
				    UCA1TXBUF=ECGTransData;
                    while(!(UCA1IFG & UCTXIFG));
				}
//				EUSCI_A_UART_transmitData ( EUSCI_A1_BASE , 'E' );
				UCA1TXBUF='E';
                while(!(UCA1IFG & UCTXIFG));
			    Mydelay ( ceil ( 102.0 * ConnectionInterval * 1000 / 120.0 ) );
			}
			sending = false;                                    // Clear flag.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~transmission~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~trans finish check deadline~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if ( CurrentTime <= ECGlocalTransDeadline ) ECGtask_finish = true;
#if ( Print_CIO == 1 )
            printf("transmission\n");
#endif
		   	Progress++;
		   	TranProg++;
#if ( Enable_LCD == 1 )
            printProg ( Progress );
            printTranProg ( TranProg );
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~trans finish check deadline~~~~~~~~~~~~~
		}
//************************************Trans ECG******************************************

//************************************Trans Temp******************************************
        if ( Tempenough == true )
        {
            Tempenough = false;                                 // Clear flag.
            TemplocalTransDeadline = TempTransDeadline;     // Latch deadline value.
#if ( Print_CIO == 1 )
            printf("trans\n");
#endif
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~transmission~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            sending = true;                                    // Set flag.
            for ( TtransTimes = 0; TtransTimes < Temp_Weight * 2 / 100; TtransTimes++ )
            {
//                EUSCI_A_UART_transmitData ( EUSCI_A1_BASE , 'H' );
                UCA1TXBUF='H';
                while(!(UCA1IFG & UCTXIFG));
                for ( TtransTimess = 0; TtransTimess < 100; TtransTimess++ )
                {
//                    EUSCI_A_UART_transmitData ( EUSCI_A1_BASE , TempTransData );
                    UCA1TXBUF=TempTransData;
                    while(!(UCA1IFG & UCTXIFG));
                }
//                EUSCI_A_UART_transmitData ( EUSCI_A1_BASE , 'E' );
                UCA1TXBUF='E';
                while(!(UCA1IFG & UCTXIFG));
                Mydelay ( ceil ( 102.0 * ConnectionInterval * 1000 / 120.0 ) );
            }
            sending = false;                                    // Clear flag.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~transmission~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~trans finish check deadline~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if ( CurrentTime <= TemplocalTransDeadline ) Temptask_finish = true;
#if ( Print_CIO == 1 )
            printf("transmission\n");
#endif
            Progress++;
            TranProg++;
#if ( Enable_LCD == 1 )
            printProg ( Progress );
            printTranProg ( TranProg );
#endif
        }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~trans finish check deadline~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //***********************************Trans Temp******************************************
        /* Enter low power mode. */
        while ( ( Tempenough + ECGenough + MatrixCount + Metrix_arrived + Temp_arrived + ECG_arrived ) == 0 )
        {
            __bis_SR_register ( LPM0_bits + GIE );
            __no_operation();
        }

        /* Context switch. */
        taskYIELD();
    }
}

int main ( void )
{
#if ( METHOD == METHOD_DEFAULT )
    function_switch = 'b';
    Backup_or_Restore();
#endif

#if ( METHOD != METHOD_DEFAULT )
    if ( recover == false )
    {
        function_switch = 's';//save empty sram state
        Backup_or_Restore();
        //restore to here to erase Sram
        if ( recover == true )
        {
            function_switch = 'r';//restore to old program state
            Backup_or_Restore();
        }
    }
#endif

	prvSetupHardware();
	P4OUT |= BIT1;//Ble interrupt pin , pull up , used for sleep ble
	P1OUT |= BIT0;//Ble interrupt pin , pull up , used for wake up ble

	initClock();
	initLCD();
    initUART();
    initInterBut();
    init_ADC();

    if ( runningCheck == false )
    {
        // pin interrupt to sleep ble
        P4OUT &= ~BIT1;
		P4OUT |= BIT1;
		// pin interrupt to sleep ble

		while ( runningCheck == false )
		{
			__bis_SR_register ( LPM0_bits + GIE );
			__no_operation();
		}
    }

    initRTC();
    if ( recover == true )
    {
        running = true;
        function_switch = 'r';  // Restore to old program state.
        Backup_or_Restore();
    }
    else
    {
        xTaskCreate ( (TaskFunction_t) App_ECG_Sensor, "App_ECG_Sensor", 200, NULL, 1, NULL );
        xTaskCreate ( (TaskFunction_t) App_Temp_Sensor, "App_Temp_Sensor", 200, NULL, 1, NULL );
        xTaskCreate ( (TaskFunction_t) App_Matrix_Multi, "App_Matrix_Multi", 200, NULL, 1, NULL );
        xTaskCreate ( (TaskFunction_t) App_Transmission, "App_Transmission", 668, NULL, 1, NULL );
        vTaskStartScheduler();
    }

    printf("test\n");
    return 0;
}
