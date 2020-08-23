#include <RTOSDemoConfig.h>
#include <msp430.h>
#include <stdio.h>
#include <LCD_function.h>
#include <Sensing.h>
#include <Energy_ResponseTime_Prediction.h>
#include <BackupRestoreHandler.h>

void vFinishCounter ( BOOLEAN* );
void System_initial ( void );
