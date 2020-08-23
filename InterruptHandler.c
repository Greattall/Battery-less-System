#include <InterruptHandler.h>

#ifdef __OFFLINE_PROFILE_CI__
    #pragma PERSISTENT ( CI )
    float CI = Static_CI_sec;
#endif

#pragma vector = PORT5_VECTOR
__interrupt void Port_5 ( void )
{
	if ( ( P5IFG & BIT5 ) == BIT5 )
	{
	    // pin interrupt to wake up BLE
		P1OUT &=~ BIT0;
		P1OUT |= BIT0;
		// pin interrupt to wake up BLE
		runningCheck = true;

#ifdef __OFFLINE_PROFILE_CI__
		__delay_cycles ( 8000000 );     // Wait UART startup
		SetNewConnectionInterval ( CI );
		ConnectionInterval = CI;
		CI += ms2sec(CI_Minor_Unit_f);
		if ( ConnectionInterval > CI_Max_sec ) CI = CI_Min_sec;
#endif

        P5IFG &= ~BIT5 + ~BIT6;         // Clear flag
		__bic_SR_register_on_exit ( LPM0_bits );    // Exit low power mode.
	}
	else if ( ( P5IFG & BIT6 ) == BIT6 )
	{
		P5IFG &= ~BIT5 + ~BIT6;         // Clear flag
		System_initial();
	}
}

#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR ( void )
{
    switch ( __even_in_range ( UCA1IV, USCI_UART_UCTXCPTIFG ) )
    {
        case USCI_NONE:             break;
        case USCI_UART_UCRXIFG:     break;
        case USCI_UART_UCTXIFG:     break;
        case USCI_UART_UCSTTIFG:    break;
        case USCI_UART_UCTXCPTIFG:  break;
    }
}

#pragma vector = RTC_C_VECTOR
__interrupt void RTC_ISR ( void )
{
	switch ( __even_in_range ( RTCIV , RTCIV__RT1PSIFG ) )
	{
		case RTCIV__NONE:       break;
		case RTCIV__RTCOFIFG:   break;
		case RTCIV__RTCRDYIFG:  break;
		case RTCIV__RTCTEVIFG:

		    CurrentTime++;

            if ( ( CurrentTime % 16 ) != 0 ) break; // Enter RTC_ISR every 0.125 sec

            getVoltage ( &CurrentVoltage );

//~~~~~~~~~~~~~~~~~~~~~Deadline test~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /* ECG task */
			if ( ( CurrentTime % (ECG_Deadline_tick) ) == 0 )
			{
				if ( running == true ) __bic_SR_register_on_exit ( LPM0_bits );
				ECG_arrived = true;
				ECGenough = false;
				vFinishCounter ( &ECGtask_finish );
				ECGtask_finish = false;                 // Clear flag
				ECGTransDeadline += ECG_Deadline_tick;
			}

			/* Temperature task */
			if ( ( CurrentTime % (Temp_Deadline_tick) ) == 0 )
			{
				if ( running == true ) __bic_SR_register_on_exit ( LPM0_bits );
				Temp_arrived = true;
				Tempenough = false;
				vFinishCounter ( &Temptask_finish );
				Temptask_finish = false;                 // Clear flag
				TempTransDeadline += Temp_Deadline_tick;
			}

			/* Matrix multiplication task */
			if ( ( CurrentTime % (Matrix_Deadline_tick) ) == 0 )
			{
				if ( running == true ) __bic_SR_register_on_exit ( LPM0_bits );
				Metrix_arrived = true;
				vFinishCounter ( &Matrixtask_finish );
				Matrixtask_finish = false;              // Clear flag
				MatrixRealDeadline += Matrix_Deadline_tick;

			}
//~~~~~~~~~~~~~~~~~~~~~Deadline test~~~~~~~~~~~~~~~~~~~~~~~~~~~



//~~~~~~~~~~~~~~~~~~~~~System execution finished~~~~~~~~~~~~~~~
			if ( CurrentTime >= sec2tick(Experiment_Time_sec) )
			{
				if ( ( CurrentVoltage < 2.6 )
				        && running == true )
				{
                    running = false;
                    while ( CurrentVoltage < 2.6 )
                    {
                        __bis_SR_register ( LPM4_bits + GIE );
                        __no_operation();
                    }
				}
				else
				{
					__bic_SR_register_on_exit ( LPM4_bits );
					if ( StopBit == false )
					{
						StopBit = true;
						recover = true;
						function_switch = 'b';
						Backup_or_Restore();
					}
					showResult();
					while ( 1 )
					{
						RTCCTL0_L &= ~RTCTEVIE_L;  //Real-time clock time event interrupt Disable
						__bis_SR_register ( LPM4_bits + GIE );
						__no_operation();
					}
				}
			}
//~~~~~~~~~~~~~~~~~~~~~System execution finished~~~~~~~~~~~~~~~

#if ( METHOD == METHOD_DEFAULT )
			if ( CurrentVoltage <= Vbac
			        && running == true )
			{
                if ( sending == true )
                {
                    /* Send STOP signal to BLE */
                    while ( !( UCA1IFG & UCTXIFG ) );
                    UCA1TXBUF = 'E';
                }

                /* Sleep BLE */
                P4OUT &=~ BIT1;
                P4OUT |= BIT1;

                /* Stop running */
                running = false;
                CheckNum++;
                printSleep();

                while ( 1 )
                {
                    /* Enter low power mode */
                    __bis_SR_register ( LPM4_bits + GIE );
                    __no_operation();
                }
			}
			else if ( CurrentVoltage >= Vres
			        && running == false )
            {   // Start restore.
                /* Reset threshold voltage */
                Vbac = Backup_Voltage;

                /* Start running */
                running = true;
                printRun();

                /* Wake up BLE */
                P1OUT &=~ BIT0;
                P1OUT |= BIT0;

                /* Start restore */
                function_switch = 'r';
                Backup_or_Restore();
            }
#endif

#if ( METHOD == METHOD_MEMENTOS )
			//Backup interval
			//0.25sec :%32
			//0.50sec :%64
			//0.75sec :%96
			//1.00sec :%128
			if ( ( CurrentTime % Backup_Interval_tick ) == 0 )
			        && running == true )
			{
				recover = true;
				function_switch = 'b';
				Backup_or_Restore();
			}
			if ( function_switch == 'r' )
			{
				__bic_SR_register_on_exit ( LPM4_bits );
			}

			if ( CurrentVoltage <= Vbac
			        && running == true )
			{
                if ( sending == true )
                {
                    /* Send STOP signal to BLE */
                    while ( !( UCA1IFG & UCTXIFG ) );
                    UCA1TXBUF = 'E';
                }

                /* Sleep BLE */
                P4OUT &=~ BIT1;
                P4OUT |= BIT1;

                /* Stop running */
                running = false;
                CheckNum++;
                printSleep();

                /* Backup */
                function_switch = 'b';
                while ( function_switch == 'b' )
                {
                    /* Enter low power mode */
                    __bis_SR_register ( LPM4_bits + GIE );
                    __no_operation();
                }
			}
			else if ( CurrentVoltage >= Vres
			        && running == false )
			{   // Start restore.
                /* Reset threshold voltage */
                Vbac = Backup_Voltage;

                /* Start running */
                running = true;
                printRun();

                /* Wake up BLE */
                P1OUT &=~ BIT0;
                P1OUT |= BIT0;

                /* Erase SRAM */
                function_switch = 'e';
                Backup_or_Restore();
            }
#endif

#if ( METHOD == METHOD_HIBERNUS )
			if ( CurrentVoltage <= Vbac )
			{
				recover = true;

				if ( running == true )
				{
				    if ( sending == true )
                    {
                        /* Send STOP signal to BLE */
                        while ( !( UCA1IFG & UCTXIFG ) );
                        UCA1TXBUF = 'E';
                    }

				    /* Sleep BLE */
                    P4OUT &=~ BIT1;
                    P4OUT |= BIT1;
                    /*Status signal*/

                    P7OUT |= BIT0;
                    /* Stop running */
                    running = false;
                    CheckNum++;
                    printSleep();

					/* Backup */
					function_switch = 'b';
					Backup_or_Restore();
					while ( function_switch == 'b' )
					{
					    /* Enter low power mode */
						__bis_SR_register ( LPM4_bits + GIE );
						__no_operation();
					}

					if ( function_switch == 'r' )
					{
					    /* Exit low power mode */
						__bic_SR_register_on_exit ( LPM4_bits );
					}
				}
			}
			else if ( CurrentVoltage >= Vres
			        && running == false )
			{   // Start restore.
                /* Reset threshold voltage */
                Vbac = Backup_Voltage;

                /* Start running */
                running = true;
                printRun();

                /* Wake up BLE */
                P1OUT &=~ BIT0;
                P1OUT |= BIT0;
                P7OUT &= ~BIT0;
                /* Erase SRAM */
                function_switch = 'e';
                Backup_or_Restore();
            }
#endif

#if ( METHOD == METHOD_REP )
			if ( CurrentVoltage <= Vbac )
			{
				recover = true;

				if ( running == true )
				{
				    if ( sending == true )
				    {
				        /* Send STOP signal to BLE */
                        while ( !( UCA1IFG & UCTXIFG ) );
                        UCA1TXBUF = 'E';
				    }

				    /* Sleep BLE */
					P4OUT &=~ BIT1;
					P4OUT |= BIT1;

					/* Stop running */
					running = false;
					CheckNum++;
					printSleep();

					/* Backup */
					function_switch = 'b';
					Backup_or_Restore();
					while ( function_switch == 'b' )
					{
					    /* Enter low power mode */
						__bis_SR_register ( LPM4_bits + GIE );
						__no_operation();
					}

					if ( function_switch == 'r' )
					{
					    /* Exit low power mode */
					    __bic_SR_register_on_exit ( LPM4_bits );
					}
				}
			}
			else if ( CurrentVoltage >= Vres
			        && running == false)
			{   // Start restore.
                /* Reset threshold voltage */
                Vbac = Backup_Voltage;
                Vres = Restore_Voltage;

                /* Start running */
                running = true;
                printRun();

                /* Wake up BLE */
                P1OUT &=~ BIT0;
                P1OUT |= BIT0;

                /* Erase SRAM */
                function_switch = 'e';
                Backup_or_Restore();
			}
#endif
			break;
		case RTCIV__RTCAIFG:    break;
		case RTCIV__RT0PSIFG:   break;
		case RTCIV__RT1PSIFG:   break;
		default:                break;
	}
}

void vFinishCounter ( BOOLEAN* finishFlag )
{
    if ( *finishFlag == true )
    {
        SuccesNum++;
    }
    else
    {
        FailNum++;
    }
}

void System_initial ( void )
{
    /* System Variable */
    runningCheck            = false;
    running                 = true;
    StopBit                 = false;
    recover                 = false;
    sending                 = false;
    CurrentTime             = 0;
    function_switch         = 'a';
    CurrentVoltage          = (float)(V_In * R_In / ( R_In + R_parallel( CI_Default_sec*R_Ls_Beta, R_Ls_Basic ) ) );
    /* System Variable */

    /* Hardware setting */
    RLa                     = (float) R_parallel( CI_Default_sec*R_La_Alpha, R_La_Basic );
    RLs                     = (float) R_parallel( CI_Default_sec*R_Ls_Beta, R_Ls_Basic );
    Vbac                    = Backup_Voltage;           // Backup threshold
    Vres                    = Restore_Voltage;          // Restore threshold
    ConnectionInterval      = CI_Default_sec;           // (sec)
    /* Hardware setting */

    /* Experiment result */
    Progress                = 0;
    TempProg                = 0;
    ECGProg                 = 0;
    MultProg                = 0;
    CompProg                = 0;
    TranProg                = 0;

    SuccesNum               = 0,
    FailNum                 = 0;
    CheckNum                = 0;
    WakeupNum               = 0;
    /* Experiment result */

    /* Task variable */
    ECG_arrived             = true;
    Temp_arrived            = true;
    Metrix_arrived          = true;

    ECGTransDeadline        = ECG_Deadline_tick;
    TempTransDeadline       = Temp_Deadline_tick;
    MatrixRealDeadline      = Matrix_Deadline_tick;

    ECGenough               = false;
    Tempenough              = false;
    MatrixCount             = 0;

    ECGtask_finish          = false;
    Temptask_finish         = false;
    Matrixtask_finish       = false;
    /* Task variable */

    PMMCTL0 |= PMMSWBOR; // reset
}
