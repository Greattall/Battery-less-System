#include "Sensing.h"
#include <msp430.h>
float A4_value=0;
unsigned int  A5_value=0;
/*void sampling(){

        ADC12CTL0 |=ADC12SC;
        A4_value=ADC12MEM0;
         A4_value=A4_value*3.3/4095;
        printf("Finish Sensing A4=%.2f \n",A4_value);
        return;
       // A5_value=ADC12MEM1;
       // A5_value=A5_value*3.3/4095;
    //    printf("Finish Sensing A4=%.0f A5=%.0f \n",A4_value,A5_value);
}*/
void getVoltage(float *ret){
	//ADC12CTL0 |=ADC12SC;
	A4_value=ADC12MEM0;
	A4_value=A4_value*3.3/4095;
	*ret=A4_value;
	return ;
}

void getECG(float *ret){
	//ADC12CTL0 |=ADC12SC;

	A5_value=ADC12MEM2;
	//A5_value=A5_value*3.3/4095;
	*ret=A5_value;
	return ;
}
