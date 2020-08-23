#include <RTOSDemoConfig.h>
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <Sharp128x128.h>

void showResult ( void );
void initLCD ( void );
void printProg ( int );
void printTemp ( float );
void printECG ( float );
void printVoltage ( float );
void printTime ( int );
void printDist ( float );
void printMeet ( float );
void printTempProg ( unsigned int );
void printECGProg ( unsigned int );
void printMultProg ( unsigned int );
void printCompProg ( unsigned int );
void printTranProg ( unsigned int );
void printSNum ( unsigned int );
void printFNum ( unsigned int );
void printCheckNum ( unsigned int );
void printWakeupNum ( unsigned int );
void printSleep ( void );
void printRun ( void );
