#include <Energy_ResponseTime_Prediction.h>

decision_t Energy_ResponseTime_Prediction ( int curTick, int deadlineTick, int dataSize, float SampleRate, float Vcur )
{
    /* Initialize variable */
	float Tcur      = tick2sec ( curTick );         // Current time (sec)
	float deadline  = tick2sec ( deadlineTick );    // Deadline (sec)
	float CT        = Tcur;                         // Completion time (sec)
	float Tlead;                                    // Leading time (sec)
	float Tdis, Tdis2, Tfdis;                       // Discharging time (sec)
	float Tcha, Tcha2;                              // Charging time (sec)
	float Vwake     = Restore_Voltage;              // Wakeup voltage (V)
	BOOLEAN changeCI   = false;                     // Request change CI flag
	int BackupTimes = 0;                            // N-times backup
    int Ncha        = 0;                            // N-times charging periods
    float CI        = ConnectionInterval;           // Connection interval (sec)
	float Trans_Throughput = N_PPI * 20 / CI;

    /* ECG execution time */
    float ECGsensExeTime    = (float)( ECG_Weight / ECG_Sample_Rate );
    float ECGtransExeTime   = (float)( ECG_Trans_Data_Size / ( N_PPI * 20 ) ) * CI;
    float ECGuartExeTime    = (float)( ECG_Trans_Data_Size / UART_Throughput );
    float E_ECG             = ECGsensExeTime + ECGtransExeTime + ECGuartExeTime;

    /* Temperature execution time */
    float TempsensExeTime    = (float)( Temp_Weight / Temp_Sample_Rate );
    float TemptransExeTime   = (float)( Temp_Trans_Data_Size / ( N_PPI * 20 ) ) * CI;
    float TempuartExeTime    = (float)( Temp_Trans_Data_Size / UART_Throughput );
    float E_Temp             = TempsensExeTime + TemptransExeTime + TempuartExeTime;

    /* Matrix multiplication execution time */
    float E_Matrix = Matrix_Exe_Time;

	/* Matrix multiplication task */
	if ( dataSize == 0 )
	{
	    /* Response time test */
	    CT += E_Matrix
	            + floor ( ( deadline - Tcur ) / ECG_Deadline_sec ) * E_ECG
	            + floor ( ( deadline - Tcur ) / Temp_Deadline_sec ) * E_Temp;

		if ( CT <= deadline )
		{   // Meet deadline
_EnergyTest:
            /* Equation (4) */
            Tdis = xCalculate_Delta_Time ( RLa, Vcur, Vbac );

			if ( CT + BackupTimes * ( T_bac + T_res ) <= Tcur + Tdis + Ncha * Tfdis )
			{   // Enough energy

#if ( CI_Adjustment == 1 )
			    SetNewConnectionInterval ( CI );
#endif
				return Permit_Task;
			}
			else
			{   // Not enough energy
				if ( BackupTimes == 0 )
				{   // Haven't calculate charging period
					Ncha = xCalculate_N_Charging_Times ( Tcur, Tdis, CT );
					BackupTimes = Ncha;

					Vres = Restore_Voltage; // Reset restore voltage
					Tcha = xCalculate_Delta_Time ( RLs, Vbac, Vres );

					if ( CT + BackupTimes * ( T_bac + T_res ) + Ncha * Tcha <= deadline )
					{   // Meet deadline with N-times charging period
                        Tfdis = xCalculate_Delta_Time ( RLa, Vres, Vbac );
                        goto _EnergyTest;
                    }
                    else
                    {   // Not enough energy && miss deadline

#if ( Wakeup_Strategy == 1 )
                        Tlead = CT + BackupTimes * ( T_bac + T_res ) + Ncha * Tcha - deadline;
                        Tcha2 = Tcha - Tlead;
                        Vwake = xCalculate_Wake_Up_Voltage ( Tcha2 );
                        Tdis2 = xCalculate_Delta_Time ( RLa, Vwake, Vbac );
                        Ncha--;
                        if ( Tcur + Tdis + Ncha * Tfdis + Tdis2 >= CT + BackupTimes * ( T_bac + T_res ) )
                        {//Wakup early , test Energy
                            //Wake up early success , energy pass
                            Vres = Vwake;
                            WakeupNum++;

    #if ( CI_Adjustment == 1 )
                            SetNewConnectionInterval ( CI );
    #endif
                            return Permit_Task;
                        }
#endif
                    }
                }
			}
		}
		else
		{   // Miss deadline

#if ( CI_Adjustment == 1 )
		    if ( CI > CI_Min_sec )
            {   // Decreasing connection interval
                /* Use MinCI and plus Tadjust to calculate CT */
                Trans_Throughput = N_PPI * 20 / CI_Min_sec;
                CT += E_Matrix + 6 * CI
                        + floor ( ( deadline - Tcur ) / ( ECG_Deadline_tick / RTC_Clock_i ) )
                        * ( ECGsensExeTime+ECG_Trans_Data_Size / Trans_Throughput + ECG_Trans_Data_Size / UART_Throughput )
                        + floor ( ( deadline - Tcur ) / ( Temp_Deadline_tick / RTC_Clock_i ) )
                        * ( TempsensExeTime + Temp_Trans_Data_Size / Trans_Throughput + Temp_Trans_Data_Size / UART_Throughput );

                if ( CT <= deadline )
                {   // Meet deadline with minimal connection interval
                    CI = CI_Min_sec;
                    vEquivalent_RL_Estimator ( CI );
                    goto _EnergyTest;
                }
            }
#endif
		}
	}

	else if ( dataSize == ECG_Weight )
	{	// High priority task
	    CT += E_ECG;

	    if ( CT <= deadline )
	    {   // Meet deadline
_EnergyTest1:
	        Tdis = xCalculate_Delta_Time ( RLa, Vcur, Vbac );

            if ( Tcur + Tdis + Ncha * Tfdis >= CT + BackupTimes * ( T_bac + T_res ) )
            {   // Enough energy

#if ( CI_Adjustment == 1 )
                if ( BackupTimes == 0 )
                {   // Haven't calculate charging period
                    CI = Slowdown_CI_handler ( ECG_Trans_Data_Size, Tcur, deadline, ConnectionInterval, Tdis );
                    SetNewConnectionInterval ( CI );
                    vEquivalent_RL_Estimator ( CI );
                }
                if ( changeCI == true ) SetNewConnectionInterval ( CI );
#endif
                return Permit_Task;
            }
            else
            {   // Not enough energy
                if ( BackupTimes == 0 )
                {   // Haven't calculate charging period
                    Ncha = xCalculate_N_Charging_Times ( Tcur, Tdis, CT );
                    BackupTimes = Ncha;
                }
                else
                {
                    return Drop_Task;
                }

                Vres = Restore_Voltage; // Reset restore voltage
                Tcha = xCalculate_Delta_Time ( RLs, Vbac, Vres );

				if ( CT + BackupTimes * ( T_bac + T_res ) + Ncha * Tcha <= deadline )
				{   // Meet deadline
                    Tfdis = xCalculate_Delta_Time ( RLa, Vres, Vbac );
                    goto _EnergyTest1;
				}
				else
				{
				    //Response Fail
#if ( Wakeup_Strategy == 1 )
				    Tlead = CT + BackupTimes * ( T_bac + T_res ) + Ncha * Tcha - deadline;
				    Tcha2 = Tcha - Tlead;
				    Vwake = xCalculate_Wake_Up_Voltage ( Tcha2 );
				    Tdis2 = xCalculate_Delta_Time ( RLa, Vwake, Vbac );
				    Ncha--;
				    if ( Tcur + Tdis + Ncha * Tfdis + Tdis2 >= CT + BackupTimes * ( T_bac + T_res ) )
				    {
                        Vres = Vwake;
                        WakeupNum++;
    #if ( CI_Adjustment == 1 )
                        if ( changeCI == true ) SetNewConnectionInterval ( CI );
    #endif
                        return Permit_Task;
                    }
                    else
                    {
    #if ( CI_Adjustment == 1 )
                        if ( changeCI == false )
                        {
                            vEquivalent_RL_Estimator ( ConnectionInterval );//We need restore old RL
                        }
    #endif
                    }

#else
    #if ( CI_Adjustment == 1 )
                    if ( changeCI == false )
                    {   //Speedup to high fail restore oldCI
                        vEquivalent_RL_Estimator ( ConnectionInterval );//Because CI not yet adjust
                    }
    #endif
#endif
				}
            }
        }
        else
        {
            //ResponseTime Prediction Fail
#if ( CI_Adjustment == 1 )
            //Speed up Connection Interval
            if ( CI > CI_Min_sec )
            {
                //Use MinCI and plus Tadjust  to Calculate CT;
                Trans_Throughput = N_PPI * 20 / CI_Min_sec; //use MinCI to test Response Time
                CT += dataSize / SampleRate + ( ECG_Trans_Data_Size / Trans_Throughput + ECG_Trans_Data_Size / UART_Throughput ) + 6 * CI;
                if ( CT <= deadline )
                {   //ResponseTime Prediction
                    changeCI = true;
                    CI = CI_Min_sec;
                    vEquivalent_RL_Estimator ( CI );
                    goto _EnergyTest1;
                }

            }
#endif
            Vbac = Restore_Voltage; // Charge now
        }
	}

	else if(dataSize==Temp_Weight){ //low pririty task
			CT +=dataSize/SampleRate+(Temp_Trans_Data_Size/Trans_Throughput+Temp_Trans_Data_Size/UART_Throughput)+floor((deadline-Tcur)/(ECG_Deadline_tick/RTC_Clock_i))*(ECGsensExeTime+ECG_Trans_Data_Size/Trans_Throughput+ECG_Trans_Data_Size/UART_Throughput);
			//Response Time Prediction Test
			if(CT<=deadline){
ResponseTimePass2:
			//Response Time Prediction Pass
			//Next Energy Test
				Tdis = xCalculate_Delta_Time ( RLa, Vcur, Vbac );
				EnergyPrediction2:
				if(Tcur+Tdis+Ncha*Tfdis>=CT+BackupTimes*(T_bac+T_res)){//EnergyPrediction
					//Energy Pass
					#if (CI_Adjustment==1)
						if(BackupTimes==0){
							//Speedupn dowm CI
							CI=Slowdown_CI_handler(Temp_Trans_Data_Size,Tcur,deadline,ConnectionInterval,Tdis);
							if(CI!=ConnectionInterval){//Slowdown Success
								SetNewConnectionInterval(CI);
								vEquivalent_RL_Estimator(CI);
								changeCI=false;//set speedup =0  because slowdown success
							}//Slowdown Success
						}
						if(changeCI==true){//Speedup to high success
							changeCI=false;
							SetNewConnectionInterval(CI);//Because CI not yet adjust
						}
					#endif
					return Permit_Task;

					}
				else{
					/*for reduce energy test complexity*/
					if(BackupTimes==0){
					    Tfdis = xCalculate_Delta_Time ( RLa, Vres, Vbac );
						Ncha=ceil((CT-(Tcur+Tdis))/Tfdis);
						BackupTimes=Ncha;
					}
					else{return Drop_Task;}
					/*for reduce energy test complexity*/
					Vres=2.8;
					Tcha = xCalculate_Delta_Time ( RLs, Vbac, Vres );
						if(CT+BackupTimes*(T_bac+T_res)+Ncha*Tcha<=deadline){//ResponseTime Prediction
						    Tfdis = xCalculate_Delta_Time ( RLa, Vres, Vbac );
								goto EnergyPrediction2;
						}
						else{
						//	Response Fail
							#if(Wakeup_Strategy==1)
								Tlead=CT+BackupTimes*(T_bac+T_res)+Ncha*Tcha-deadline;
								Tcha2=Tcha-Tlead;
								Vwake=xCalculate_Wake_Up_Voltage(Tcha2);
								Tdis2 = xCalculate_Delta_Time ( RLa, Vwake, Vbac );
								Ncha--;
								if(Tcur+Tdis+Ncha*Tfdis+Tdis2>=CT+BackupTimes*(T_bac+T_res)){//Wakup early , test Energy
									//Wake up early , energy pass
									Vres=Vwake;
									WakeupNum++;
									#if(CI_Adjustment==1)
										if(changeCI==true){//Speedup to high success
											changeCI=false;
											SetNewConnectionInterval(CI);//Because CI not yet adjust
										}
									#endif
								return Permit_Task;
								}
								else{
									#if(CI_Adjustment==1)
										if(changeCI==true){//Speedup to high fail restore oldCI
											CI=ConnectionInterval;//We need restore old  CI
											vEquivalent_RL_Estimator(ConnectionInterval);//We need restore old RL
										}
									#endif
								return Drop_Task;
								}

							#else
								#if(CI_Adjustment==1)
									if(changeCI==0){//Speedup to high fail restore oldCI
									CI=ConnectionInterval;
									vEquivalent_RL_Estimator(ConnectionInterval);//Because CI not yet adjust
									}
								#endif
								return Drop_Task;
							#endif
						}
				 }
			}
			else{
				//ResponseTime Prediction Fail
				#if(CI_Adjustment==1)
					//Speed up Connection Interval
					if(CI!=CI_Min_sec){
						//Use MinCI and plus Tadjust  to Calculate CT;
						Trans_Throughput=N_PPI * 20 / CI_Min_sec;//use MinCI to test Response Time
						CT += dataSize/SampleRate+(Temp_Trans_Data_Size/Trans_Throughput+Temp_Trans_Data_Size/UART_Throughput)+floor((deadline-Tcur)/(ECG_Deadline_tick/RTC_Clock_i))*(ECGsensExeTime+ECG_Trans_Data_Size/Trans_Throughput+ECG_Trans_Data_Size/UART_Throughput)+6*CI;
						//6 ConnectionEvent is adjustment overhead
						if(CT<=deadline){//ResponseTime Prediction
							changeCI=true;
							CI=CI_Min_sec;
							vEquivalent_RL_Estimator(CI);
							goto ResponseTimePass2;
						}
						else{return Drop_Task;}//drop task
					}
					else{return Drop_Task;}
				#else
					return Drop_Task;
				#endif
			}
	}

	return Drop_Task;
}

float Slowdown_CI_handler ( float dataSize, float Tcur, float deadline, float CI_sec, float Tdischar )
{
	float f_CT_Virtual;;

	/* Initialize variable */
	float f_CI_Target = CI_sec;

	/* CI adjustment overhead */
	float f_T_ove = 6 * CI_sec;

    /* Equations (9) */
    float f_slack = xSlack ( Tdischar, deadline - Tcur);

    /* ECG execution time */
    float ECGsensExeTime    = (float)( ECG_Weight / ECG_Sample_Rate );
    float ECGtransExeTime   = (float)( ECG_Trans_Data_Size / ( N_PPI * 20 ) ) * CI_sec;
    float ECGuartExeTime    = (float)( ECG_Trans_Data_Size / UART_Throughput );
    float E_ECG             = ECGsensExeTime + ECGtransExeTime + ECGuartExeTime;

    /* Temperature execution time */
    float TempsensExeTime    = (float)( Temp_Weight / Temp_Sample_Rate );
    float TemptransExeTime   = (float)( Temp_Trans_Data_Size / ( N_PPI * 20 ) ) * CI_sec;
    float TempuartExeTime    = (float)( Temp_Trans_Data_Size / UART_Throughput );
    float E_Temp             = TempsensExeTime + TemptransExeTime + TempuartExeTime;

    /* Matrix multiplication execution time */
    float E_Matrix = Matrix_Exe_Time;

    /* Calculate target CI based on each task */
	if ( dataSize == ECG_Trans_Data_Size )
	{
	    f_CT_Virtual = E_ECG
	            + f_T_ove
	            + ceil ( f_slack / Matrix_Deadline_sec ) * E_Matrix
	            + ceil ( f_slack / Temp_Deadline_sec ) * E_Temp;

        if ( f_CT_Virtual <= f_slack )  //Can i have adjust space
        {
            f_CI_Target = ( N_PPI * 20 ) / ( ECG_Trans_Data_Size + ceil ( f_slack / Temp_Deadline_sec ) * Temp_Trans_Data_Size ) * (
                    f_slack
                    - ceil ( f_slack / Matrix_Deadline_sec ) * E_Matrix
                    - ECGsensExeTime
                    - ceil ( f_slack / Temp_Deadline_sec ) * TempsensExeTime
                    - f_T_ove
                    - ( ECG_Trans_Data_Size + ceil ( f_slack / Temp_Deadline_sec ) * Temp_Trans_Data_Size ) / UART_Throughput
                    );
        }
	}
	else if ( dataSize == Temp_Trans_Data_Size )
	{
	    f_CT_Virtual = E_Temp
                + f_T_ove
                + ceil ( f_slack / Matrix_Deadline_sec ) * E_Matrix
                + ceil ( f_slack / ECG_Deadline_sec ) * E_ECG;

        if ( f_CT_Virtual <= f_slack ) //Can i have adjust space
        {
            f_CI_Target = ( N_PPI * 20 ) / ( Temp_Trans_Data_Size + ceil ( f_slack / ECG_Deadline_sec ) * ECG_Trans_Data_Size ) * (
                    f_slack
                    - ceil ( f_slack / Matrix_Deadline_sec ) * E_Matrix
                    - TempsensExeTime
                    - ceil ( f_slack / ECG_Deadline_sec ) * ECGsensExeTime
                    - f_T_ove
                    - ( Temp_Trans_Data_Size + ceil ( f_slack / ECG_Deadline_sec ) * ECG_Trans_Data_Size ) / UART_Throughput
                    );
        }
	}

	/* Round Connection Interval */
	f_CI_Target = floor ( sec2ms(f_CI_Target) / CI_Minor_Unit_f ) * CI_Minor_Unit_f;
	f_CI_Target = ms2sec(f_CI_Target);
	if ( f_CI_Target >= CI_Max_sec ) f_CI_Target = CI_Max_sec;    // Saturate protection

	return f_CI_Target;
}

void SetNewConnectionInterval ( float CI )
{
    if ( ConnectionInterval != CI )
    {
        ConnectionInterval = CI;
        float SwitchCI = ( sec2ms(CI) - sec2ms(CI_Min_sec) ) / CI_Minor_Unit_f;
        printf("Hleoo");
        switch ( (int)round( SwitchCI) )
        {
            case 0:
                UCA1TXBUF = 'a';    //20ms
                break;
            case 1:
                UCA1TXBUF = 'b';    //25ms
                break;
            case 2:
                UCA1TXBUF = 'c';    //30ms
                break;
            case 3:
                UCA1TXBUF = 'd';    //35ms
                break;
            case 4:
                UCA1TXBUF = 'e';    //40ms
                break;
            case 5:
                UCA1TXBUF = 'f';    //45ms
                break;
            case 6:
                UCA1TXBUF = 'g';    //50ms
                break;
            case 7:
                UCA1TXBUF = 'h';    //55ms
                break;
            case 8:
                UCA1TXBUF = 'i';    //60ms
                break;
            case 9:
                UCA1TXBUF = 'j';    //65ms
                break;
            case 10:
                UCA1TXBUF = 'k';    //70ms
                break;
            case 11:
                UCA1TXBUF = 'l';    //75ms
                break;
            case 12:
                UCA1TXBUF = 'm';    //80ms
                break;
            case 13:
                UCA1TXBUF = 'n';    //85ms
                break;
            case 14:
                UCA1TXBUF = 'o';    //90ms
                break;
            case 15:
                UCA1TXBUF = 'p';    //95ms
                break;
            case 16:
                UCA1TXBUF = 'q';    //100ms
                break;
            case 17:
                UCA1TXBUF = 'r';    //105ms
                break;
            case 18:
                UCA1TXBUF = 's';    //110ms
                break;
            case 19:
                UCA1TXBUF = 't';    //115ms
                break;
            case 20:
                UCA1TXBUF = 'u';    //120ms
                break;
            case 21:
                UCA1TXBUF = 'v';    //125ms
                break;
            case 22:
                UCA1TXBUF = 'w';    //130ms
                break;
            case 23:
                UCA1TXBUF = 'x';    //135ms
                break;
            case 24:
                UCA1TXBUF = 'y';    //140ms
                break;
            case 25:
                UCA1TXBUF = 'z';    //145ms
                break;
            case 26:
                UCA1TXBUF = 'Z';    //150ms
                break;
            case 27:
                UCA1TXBUF = 'Y';    //155ms
                break;
            case 28:
                UCA1TXBUF = 'X';    //160ms
                break;
            case 29:
                UCA1TXBUF = 'W';    //165ms
                break;
            case 30:
                UCA1TXBUF = 'V';    //170ms
                break;
            case 31:
                UCA1TXBUF = 'U';    //175ms
                break;
            case 32:
                UCA1TXBUF = 'T';    //180ms
                break;
            case 33:
                UCA1TXBUF = 'S';    //185ms
                break;
            case 34:
                UCA1TXBUF = 'R';    //190ms
                break;
            case 35:
                UCA1TXBUF = 'Q';    //195ms
                break;
            case 36:
                UCA1TXBUF = 'P';    //200ms
                break;
            case 246:
                UCA1TXBUF = 'O';    //1250ms
                break;
        }
    }
}

/*
 * @ Brief: Calculate time duration of capacitor voltage changed between Vinit to Vend.
 *          Equation (2).
 *
 * @ Note:  Require macro function: R_parallel(R1,R2).
 *
 */
float xCalculate_Delta_Time ( float RL, float Vinit, float Vend )
{
    float Vin_2 = V_In * RL / ( R_In + RL );
    float DeltaT = R_parallel( R_In, RL ) * Capacitor_i * logf ( ( Vin_2 - Vinit ) / ( Vin_2 - Vend ) );
    return DeltaT;
}

/*
 * @ Brief: Calculate N-times charging period based on task completion time.
 *          Equation (5).
 *
 */
int xCalculate_N_Charging_Times ( float Tcur, float Tdis, float CT )
{
    float Tfdis = xCalculate_Delta_Time ( RLa, Vres, Vbac );
    int Ncha = ceil ( ( CT - ( Tcur + Tdis ) ) / Tfdis );
    return Ncha;
}

/*
 * @ Brief: Calculate early wake up voltage based on charging duration.
 *          Equation (1).
 *
 * @ Note:  Require global variables: Vbac, RLs.
 *          Require macro function: R_parallel(R1,R2).
 *
 */
float xCalculate_Wake_Up_Voltage ( float SutiableChargingTime )
{
    float V_in2 = V_In * RLs / ( R_In + RLs );
    float V_wake = Vbac + ( V_in2 - Vbac ) * ( 1 - exp ( - SutiableChargingTime / ( R_parallel( R_In, RLs ) * Capacitor_i ) ) );
    return V_wake;
}

/*
 * @ Brief: Calculate equivalent RL based on connection interval (CI).
 *          Equation (7), (8).
 *
 */
void vEquivalent_RL_Estimator ( float CI )
{
    float Ra_ble = CI * R_La_Alpha;
    float Rs_ble = CI * R_Ls_Beta;

    RLa = R_parallel( Ra_ble, R_La_Basic );
    RLs = R_parallel( Rs_ble, R_Ls_Basic );
}

/*
 * @ Brief: Find minimal between Delta_T_1 and Delta_T_2.
 *          Equation (9).
 *
 */
float xSlack ( float Delta_T_1, float Delta_T_2)
{
    if ( Delta_T_1 <= Delta_T_2 )
    {
        return Delta_T_1;
    }
    else
    {
        return Delta_T_2;
    }
}

