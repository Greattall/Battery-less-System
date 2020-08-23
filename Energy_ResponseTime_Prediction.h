#include <RTOSDemoConfig.h>
#include <math.h> //user for ceiling function
#include <stdio.h>
#include <string.h>
#include <msp430.h>

decision_t Energy_ResponseTime_Prediction ( int, int, int, float, float );

float Slowdown_CI_handler ( float, float, float, float, float );

void SetNewConnectionInterval ( float );

/* Local function */
float   xCalculate_Delta_Time       ( float, float, float );
int     xCalculate_N_Charging_Times ( float, float, float );
float   xCalculate_Wake_Up_Voltage  ( float );
void    vEquivalent_RL_Estimator    ( float );
float   xSlack                      ( float, float );

