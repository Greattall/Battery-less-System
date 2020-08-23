/*
 * RTOSDemoConfig.h
 *
 *  Created on: Aug 17, 2020
 *      Author: LeeKaiXuan
 */

#ifndef RTOSDEMOCONFIG_H_
#define RTOSDEMOCONFIG_H_

/* *********************************************************************************************************
 *
 *                                            Type Definition
 *
 * *********************************************************************************************************
 */

/* Boolean */
typedef unsigned int Bool;
#define BOOLEAN    Bool
#define false   0
#define true    1

/* Return value of Energy_ResponseTime_Prediction() */
typedef enum {
    Permit_Task,
    Drop_Task,
} decision_t;

/* *********************************************************************************************************
 *
 *                                            Macro Declaration
 *
 * *********************************************************************************************************
 */

/* Methods declaration */
#define METHOD_DEFAULT              0
#define METHOD_MEMENTOS             1
#define METHOD_HIBERNUS             2
#define METHOD_REP                  3

/* Workload declaration */
#define WORKLOAD_25                 0   // LowLoad      ( 25 % )
#define WORKLOAD_50                 1   // MediumLoad   ( 50 % )
#define WORKLOAD_75                 2   // HighLoad     ( 75 % )
#define WORKLOAD_LIGHT              3   // LightLoad    ( U < Bound )
#define WORKLOAD_MEDIUM             4   // MediumLoad   ( U = Bound )
#define WORKLOAD_HEAVY              5   // HeavyLoad    ( U > Bound )

/* Macro function declaration */
#define ms2sec(X)                   (X / 1000.0)
#define sec2ms(X)                   (X * 1000.0)
#define tick2sec(X)                 (X / RTC_Clock_f)
#define sec2tick(X)                 (X * RTC_Clock_i)
#define R_parallel(R1,R2)           ((R1*R2)/(R1+R2))

/* *********************************************************************************************************
 *
 *                                              Configuration
 *
 * *********************************************************************************************************
 */
#define Buff_Size                   8192                    // (bytes)

//-------------------------------------------- System Variables --------------------------------------------

/* Timing Configuration */
#define Experiment_Time_sec         60                      // Experiment duration (sec)
#define RTC_Clock_f                 128.0                   // 128.0Hz for RTC
#define RTC_Clock_i                 128                     // 128Hz for RTC
#define T_bac                       0.1                     // Backup time overhead (sec)
#define T_res                       0.16                    // Restore time overhead (sec)

/* Voltage Configuration */
#define V_In                        3.3                     // (V)
#define Atomic_Voltage              2.25                    // (V)
#define Backup_Voltage              2.2                     // (V)
#define Restore_Voltage             2.8                     // (V)

/* Resistor Configuration */
#define R_In                        330                     // (ohm)
#define R_Ls_Basic                  3179                    // (ohm)
#define R_Ls_Beta                   265786                  // (ohm/sec)
#define R_La_Alpha                  578682                  // (ohm/sec)

/* Power Storage Configuration */
#define Capacitor_f                 0.0047                  // (farad)
#define Capacitor_i                 Capacitor_f*1000*1000   // (micro farad)

/* UART Configuration */
#define UART_Throughput             115200                  // (baudrate)

/* BLE Configuration */
#define CI_Minor_Unit_f             5.0
#define CI_Min_sec                  0.02
#define CI_Max_sec                  0.2
#define CI_Default_sec              0.02
#define N_PPI                       6                       // N-Packet Per connection Interval (default: 6)

//----------------------------------------------------------------------------------------------------------

//---------------------------------------------- Task Variables --------------------------------------------

/* Task properties */
#define ECG_Deadline_sec            2                       // (sec)
#define Temp_Deadline_sec           3                       // (sec)
#define Matrix_Deadline_sec         4                       // (sec)

#define ECG_Deadline_tick           sec2tick(ECG_Deadline_sec)       // ECGTask period (tick)
#define Temp_Deadline_tick          sec2tick(Temp_Deadline_sec)      // TempTask period (tick)
#define Matrix_Deadline_tick        sec2tick(Matrix_Deadline_sec)    // MetrixTask period (tick)
#define ECG_Sample_Rate             1000.0  // Hz
#define Temp_Sample_Rate            500.0   // Hz

/* Off-line profiling */
//#define __OFFLINE_PROFILE_CI__        // Uncomment to enable off-line profiling feature.
#ifdef __OFFLINE_PROFILE_CI__
    #define Static_CI_sec           0.140    // Set connection interval (sec), range: 0.020-0.2 sec
#endif

/* Method configuration */
#define METHOD                      METHOD_REP

/* Mementos details configuration */
#if ( METHOD == METHOD_MEMENTOS )
    #define Backup_Interval_tick    96      // Backup interval (tick)
#endif

/* REP details configuration */
#if ( METHOD == METHOD_REP )
    #define Wakeup_Strategy         1       // '1' for enable wake-up method
    #define CI_Adjustment           1       // '1' for enable dynamic connection interval
#endif

/* Other features */
#define Enable_LCD                  0
#define Print_CIO                   0

/* Workload configuration */
#define WORKLOAD                    WORKLOAD_MEDIUM

/* Workload details configuration */
#if ( WORKLOAD == WORKLOAD_25 )
    #define R_La_Basic              666     // (ohm)
    #define Matrix_Exe_Time         0.085
    #define Temp_Weight             50
    #define ECG_Weight              100
    #define Temp_Trans_Data_Size    102
    #define ECG_Trans_Data_Size     204

#elif ( WORKLOAD == WORKLOAD_50 )
    #define R_La_Basic              600     // (ohm)
    #define Matrix_Exe_Time         0.17
    #define Temp_Weight             100
    #define ECG_Weight              200
    #define Temp_Trans_Data_Size    204
    #define ECG_Trans_Data_Size     408

#elif ( WORKLOAD == WORKLOAD_75 )
    #define R_La_Basic              596     // (ohm)
    #define Matrix_Exe_Time         0.255
    #define Temp_Weight             150
    #define ECG_Weight              300
    #define Temp_Trans_Data_Size    306
    #define ECG_Trans_Data_Size     612

#elif ( WORKLOAD == WORKLOAD_LIGHT )    //light load utilization equal to utilization bound
    #define R_La_Basic              600     // (ohm)
    #define Matrix_Exe_Time         0.010
    #define Temp_Weight             50
    #define ECG_Weight              100
    #define Temp_Trans_Data_Size    102 // 1 records = 2Bytes data , each transmission only can transmit 1 Byte data  , every 100 byte need add 1 start bit and 1 stop bit
    #define ECG_Trans_Data_Size     204

#elif ( WORKLOAD == WORKLOAD_MEDIUM )   //middle load utilization equal to utilization bound
    #define R_La_Basic              535     // (ohm)
    #define Matrix_Exe_Time         0.125
    #define Temp_Weight             50
    #define ECG_Weight              100
    #define Temp_Trans_Data_Size    102 // 1 records = 2Bytes data , each transmission only can transmit 1 Byte data  , every 100 byte need add 1 start bit and 1 stop bit
    #define ECG_Trans_Data_Size     204

#elif ( WORKLOAD == WORKLOAD_HEAVY )    //heavy load utilization equal to utilization bound
    #define R_La_Basic              600     // (ohm)
    #define Matrix_Exe_Time         0.340
    #define Temp_Weight             50
    #define ECG_Weight              100
    #define Temp_Trans_Data_Size    102 // 1 records = 2Bytes data , each transmission only can transmit 1 Byte data  , every 100 byte need add 1 start bit and 1 stop bit
    #define ECG_Trans_Data_Size     204
#endif
//----------------------------------------------------------------------------------------------------------

/* *********************************************************************************************************
 *
 *                                      Global Variables Declaration
 *
 * *********************************************************************************************************
 */
extern unsigned char FRAM_buffer[Buff_Size];        //Used to saved VM data
extern unsigned char FRAM_buffer_empty[Buff_Size];  //Used to saved empty VM data

//-------------------------------------------- System Variables --------------------------------------------
extern BOOLEAN runningCheck;
extern BOOLEAN running;
extern BOOLEAN StopBit;      //If StopBit=1 then System stop
extern BOOLEAN recover;
extern BOOLEAN sending;     // sending state
extern unsigned int CurrentTime;
extern float CurrentVoltage;
extern char function_switch;
//----------------------------------------------------------------------------------------------------------

//------------------------------------------- Hardware Variables -------------------------------------------
extern float RLa;
extern float RLs;
extern float Vbac;           //Backup threshold
extern float Vres;           //Restore threshold
extern float ConnectionInterval;//(s)
//----------------------------------------------------------------------------------------------------------

//------------------------------------------ Experiment Variables ------------------------------------------
extern unsigned int Progress;
extern unsigned int TempProg;
extern unsigned int ECGProg;
extern unsigned int MultProg;
extern unsigned int CompProg;
extern unsigned int TranProg;

extern unsigned int SuccesNum;
extern unsigned int FailNum;
extern unsigned int CheckNum;
extern unsigned int WakeupNum;
//----------------------------------------------------------------------------------------------------------

//---------------------------------------------- Task Variables --------------------------------------------
extern BOOLEAN ECG_arrived;
extern BOOLEAN Temp_arrived;
extern BOOLEAN Metrix_arrived;
extern unsigned int ECGTransDeadline;
extern unsigned int TempTransDeadline;
extern unsigned int MatrixRealDeadline;
extern BOOLEAN ECGenough;
extern BOOLEAN Tempenough;
extern unsigned int MatrixCount;
extern BOOLEAN ECGtask_finish;
extern BOOLEAN Temptask_finish;
extern BOOLEAN Matrixtask_finish;
//----------------------------------------------------------------------------------------------------------

#endif /* RTOSDEMOCONFIG_H_ */

