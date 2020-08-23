/*
 * RTOSDemoConfig.c
 *
 *  Created on: Aug 17, 2020
 *      Author: LeeKaiXuan
 */
#include <RTOSDemoConfig.h>

#pragma PERSISTENT(FRAM_buffer)
#pragma PERSISTENT(FRAM_buffer_empty)

unsigned char FRAM_buffer[Buff_Size]        = { 0 };    //Used to saved VM data
unsigned char FRAM_buffer_empty[Buff_Size]  = { 0 };    //Used to saved empty VM data

//-------------------------------------------- System Variables --------------------------------------------
#pragma PERSISTENT ( runningCheck )//for Default
#pragma PERSISTENT ( running )
#pragma PERSISTENT ( StopBit )
#pragma PERSISTENT ( recover )
#pragma PERSISTENT ( sending )
#pragma PERSISTENT ( CurrentTime )
#pragma PERSISTENT ( CurrentVoltage )
#pragma PERSISTENT ( function_switch )

BOOLEAN runningCheck        = false;
BOOLEAN running             = true;
BOOLEAN StopBit             = false;    //If StopBit=1 then System stop
BOOLEAN recover             = false;
BOOLEAN sending             = false;    // sending state
unsigned int CurrentTime    = 0;
float CurrentVoltage        = (float)(V_In * R_In / ( R_In + R_parallel( CI_Default_sec*R_Ls_Beta, R_Ls_Basic ) ) );
char function_switch        = 'a';
//----------------------------------------------------------------------------------------------------------

//------------------------------------------- Hardware Variables -------------------------------------------
#pragma PERSISTENT ( RLa )
#pragma PERSISTENT ( RLs )
#pragma PERSISTENT ( Vbac )
#pragma PERSISTENT ( Vres )
#pragma PERSISTENT ( ConnectionInterval )

float RLa                   = (float) R_parallel( CI_Default_sec*R_La_Alpha, R_La_Basic );
float RLs                   = (float) R_parallel( CI_Default_sec*R_Ls_Beta, R_Ls_Basic );
float Vbac                  = Backup_Voltage;           // Backup threshold
float Vres                  = Restore_Voltage;          // Restore threshold
float ConnectionInterval    = CI_Default_sec;           // (sec)
//----------------------------------------------------------------------------------------------------------

//------------------------------------------ Experiment Variables ------------------------------------------
#if ( METHOD == METHOD_DEFAULT )    //Because Default no backup data during runtime, so put these record data into NVM
    #pragma PERSISTENT ( Progress )
    #pragma PERSISTENT ( TempProg )
    #pragma PERSISTENT ( ECGProg )
    #pragma PERSISTENT ( MultProg )
    #pragma PERSISTENT ( CompProg )
    #pragma PERSISTENT ( TranProg )
#endif

#pragma PERSISTENT ( SuccesNum )
#pragma PERSISTENT ( FailNum )
#pragma PERSISTENT ( CheckNum )
#pragma PERSISTENT ( WakeupNum )

unsigned int Progress       = 0;
unsigned int TempProg       = 0;
unsigned int ECGProg        = 0;
unsigned int MultProg       = 0;
unsigned int CompProg       = 0;
unsigned int TranProg       = 0;

unsigned int SuccesNum      = 0;
unsigned int FailNum        = 0;
unsigned int CheckNum       = 0;
unsigned int WakeupNum      = 0;
//----------------------------------------------------------------------------------------------------------

//---------------------------------------------- Task Variables --------------------------------------------
#pragma PERSISTENT ( ECG_arrived )
#pragma PERSISTENT ( Temp_arrived )
#pragma PERSISTENT ( Metrix_arrived )
#pragma PERSISTENT ( ECGTransDeadline )
#pragma PERSISTENT ( TempTransDeadline )
#pragma PERSISTENT ( MatrixRealDeadline )
#pragma PERSISTENT ( ECGenough )
#pragma PERSISTENT ( Tempenough )
#pragma PERSISTENT ( MatrixCount )
#pragma PERSISTENT ( ECGtask_finish )
#pragma PERSISTENT ( Temptask_finish )
#pragma PERSISTENT ( Matrixtask_finish )

BOOLEAN ECG_arrived             = true;
BOOLEAN Temp_arrived            = true;
BOOLEAN Metrix_arrived          = true;
unsigned int ECGTransDeadline   = ECG_Deadline_tick;
unsigned int TempTransDeadline  = Temp_Deadline_tick;
unsigned int MatrixRealDeadline = Matrix_Deadline_tick;
BOOLEAN ECGenough               = false;
BOOLEAN Tempenough              = false;
unsigned int MatrixCount        = 0;
BOOLEAN ECGtask_finish          = false;
BOOLEAN Temptask_finish         = false;
BOOLEAN Matrixtask_finish       = false;
//----------------------------------------------------------------------------------------------------------

