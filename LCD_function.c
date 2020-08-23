#include <LCD_function.h>

Graphics_Context g_sContext = {0};
int8_t str[10];

void showResult()
{
    Sharp128x128_initDisplay();
    Graphics_initContext(&g_sContext, &g_sharp128x128LCD);
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    Graphics_flushBuffer(&g_sContext);

    // Clear the screen first
    Graphics_clearDisplay(&g_sContext);

    // Draw the title once
#if ( METHOD == METHOD_DEFAULT)
    Graphics_drawString(&g_sContext, "ESSLAB(1)",
                                AUTO_STRING_LENGTH, 50, 0,
                                GRAPHICS_OPAQUE_TEXT);
#elif ( METHOD == METHOD_MEMENTOS)
    Graphics_drawString(&g_sContext, "ESSLAB(2)",
                                AUTO_STRING_LENGTH, 50, 0,
                                GRAPHICS_OPAQUE_TEXT);
#elif ( METHOD == METHOD_HIBERNUS)
    Graphics_drawString(&g_sContext, "ESSLAB(3)",
                                AUTO_STRING_LENGTH, 50, 0,
                                GRAPHICS_OPAQUE_TEXT);
#elif ( METHOD == METHOD_REP)
    Graphics_drawString(&g_sContext, "ESSLAB(4)",
                        AUTO_STRING_LENGTH, 50, 0,
                        GRAPHICS_OPAQUE_TEXT);
#endif
    Graphics_drawString(&g_sContext, "Intermittent System",
                                    AUTO_STRING_LENGTH, 10, 15,
                                    GRAPHICS_OPAQUE_TEXT);

    Graphics_drawString(&g_sContext, "STOP",
                        AUTO_STRING_LENGTH, 50, 25,
                        GRAPHICS_OPAQUE_TEXT);

    Graphics_drawString(&g_sContext, "Progress:",
                                AUTO_STRING_LENGTH, 10,40,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "DistRate:",
                                AUTO_STRING_LENGTH, 10,50,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "MetRatio:",
                                AUTO_STRING_LENGTH, 10,60,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "TempProg:",
                                        AUTO_STRING_LENGTH, 10,70,
                                        GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "ECG_Prog:",
                                AUTO_STRING_LENGTH, 10,80,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "MultProg:",
                                AUTO_STRING_LENGTH, 10,90,
                                GRAPHICS_OPAQUE_TEXT);
//				Graphics_drawString(&g_sContext, "CompProg:",
//											AUTO_STRING_LENGTH, 10,100,
//											GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "TranProg:",
                                AUTO_STRING_LENGTH, 10,100,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "SNum:",
                                AUTO_STRING_LENGTH, 10,110,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "FNum:",
                                AUTO_STRING_LENGTH, 70,110,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "CheckNum:",
                                AUTO_STRING_LENGTH, 10,120,
                                GRAPHICS_OPAQUE_TEXT);
    printProg(Progress);

    float MeetRatio =  (float) SuccesNum / ( SuccesNum + FailNum );
    printMeet(MeetRatio);//printMeet();

    printTempProg(TempProg);
    printECGProg(ECGProg);
    printMultProg(MultProg);
    //printCompProg(CompProg);
    printTranProg(TranProg);
    printSNum(SuccesNum);
    printFNum(FailNum);
    printCheckNum(CheckNum);
    printWakeupNum(WakeupNum);
//		printCheckNum(WakeupNum);
    //		printCompProg(SuccesNum+FaillNum);
    //		printTranProg(FaillNum);

    // Flush Buffer to LCD
    Graphics_flushBuffer(&g_sContext);
}

void initLCD()
{
    // Set up the LCD
    Sharp128x128_initDisplay();
    Graphics_initContext(&g_sContext, &g_sharp128x128LCD);
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    Graphics_flushBuffer(&g_sContext);

    // Clear the screen first
//    Graphics_clearDisplay(&g_sContext);


    // Draw the title once
#if ( METHOD == METHOD_DEFAULT)
    Graphics_drawString(&g_sContext, "ESSLAB(1)",
                                AUTO_STRING_LENGTH, 50, 0,
                                GRAPHICS_OPAQUE_TEXT);
#elif ( METHOD == METHOD_MEMENTOS)
    Graphics_drawString(&g_sContext, "ESSLAB(2)",
                                AUTO_STRING_LENGTH, 50, 0,
                                GRAPHICS_OPAQUE_TEXT);
#elif ( METHOD == METHOD_HIBERNUS)
    Graphics_drawString(&g_sContext, "ESSLAB(3)",
                                AUTO_STRING_LENGTH, 50, 0,
                                GRAPHICS_OPAQUE_TEXT);
#elif ( METHOD == METHOD_REP)
    Graphics_drawString(&g_sContext, "ESSLAB(4)",
                        AUTO_STRING_LENGTH, 50, 0,
                        GRAPHICS_OPAQUE_TEXT);
#endif
    Graphics_drawString(&g_sContext, "Intermittent System",
                                    AUTO_STRING_LENGTH, 10, 15,
                                    GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "Running",
                        AUTO_STRING_LENGTH, 50, 25,
                        GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "Progress:",
                                AUTO_STRING_LENGTH, 10,40,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "TempDegC:",
                                AUTO_STRING_LENGTH, 10,50,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "ECG_data:",
                                AUTO_STRING_LENGTH, 10,60,
                                GRAPHICS_OPAQUE_TEXT);
//    Graphics_drawString(&g_sContext, "CurrVolt:",
//                                AUTO_STRING_LENGTH, 10,70,
//                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "TempProg:",
                                AUTO_STRING_LENGTH, 10,70,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "ECG_Prog:",
                                AUTO_STRING_LENGTH, 10,80,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "MultProg:",
                                AUTO_STRING_LENGTH, 10,90,
                                GRAPHICS_OPAQUE_TEXT);
//    Graphics_drawString(&g_sContext, "CompProg:",
//                                AUTO_STRING_LENGTH, 10,100,
//                                GRAPHICS_OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, "TranProg:",
                                AUTO_STRING_LENGTH, 10,100,
                                GRAPHICS_OPAQUE_TEXT);
    // Flush Buffer to LCD
    Graphics_flushBuffer(&g_sContext);
}

void printProg(int x)
{
    sprintf(str,"%d",x);
    Graphics_drawString(&g_sContext, str,
                                AUTO_STRING_LENGTH, 65, 40,
                                GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printTemp(float y)
{
    sprintf(str,"%f",y);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 50,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printECG(float y)
{
    sprintf(str,"%f",y);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 60,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printVoltage(float z)
{
    sprintf(str,"%f",z);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 70,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}
void printTime(int a)
{
    sprintf(str,"%d",a);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 90,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printDist(float j)
{
    sprintf(str,"%f",j);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 50,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}
void printMeet(float k)
{
    sprintf(str,"%f",k);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 60,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printTempProg( unsigned int b)
{
    sprintf(str,"%d",b);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 70,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printECGProg( unsigned int b)
{
    sprintf(str,"%d",b);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 80,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printMultProg( unsigned int c)
{
    sprintf(str,"%d",c);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 90,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printCompProg( unsigned int d)
{
    sprintf(str,"%d",d);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 100,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printTranProg( unsigned int e)
{
    sprintf(str,"%d",e);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 100,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printSNum( unsigned int e)
{
    sprintf(str,"%d",e);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 40, 110,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printFNum( unsigned int e)
{
    sprintf(str,"%d",e);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 100, 110,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printCheckNum( unsigned int e)
{
    sprintf(str,"%d",e);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 65, 120,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printWakeupNum( unsigned int e)
{
    sprintf(str,"%d",e);
    Graphics_drawString(&g_sContext, str,
    	                        AUTO_STRING_LENGTH, 100, 120,
    	                        GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printSleep(void)
{
    Graphics_drawString(&g_sContext, "Sleeping",
   		    		    AUTO_STRING_LENGTH, 50, 25,
   		    		    GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void printRun(void)
{
    Graphics_drawString(&g_sContext, "Running~",
   		    		    AUTO_STRING_LENGTH, 50, 25,
   		    		    GRAPHICS_OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}
