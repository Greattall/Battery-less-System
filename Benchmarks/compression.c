/*
 * compression.c
 *
 *  Created on: 2019年10月23日
 *      Author: greattall
 */
#include <stdio.h>
#include "compression.h"
#include <stdlib.h>
#if(Mementos==0)
#pragma PERSISTENT(DistorRate)
#pragma PERSISTENT(NumofData)
#endif //在週期備份中 如果把它放到NVM 那他回朔的時候會一直重複+
extern float DistorRate=0;
extern float NumofData=0;

extern int TransDataSize;


void RLE_encoder(unsigned short *input,int isize,unsigned short *output ,unsigned short *osize,unsigned short K){

	unsigned short count=0;//RLE count
	int source=0;
	int index=source;
	int output_index=0;
	while(source<=isize-1){

		while(input[source]-K<=input[source+index]&&input[source+index]<=input[source]+K&& index<=isize-1){
			count++;
			index++;
		}
		output[output_index]=input[source];
		output[output_index+1]=count;
		output_index+=2;
		*osize=output_index;
		source+=index;
		count=0;
		index=0;
	}

}
void Compression(unsigned short *Input , unsigned short inputsize){

	//1個array占4Byte
	//memcpy(Input,Input1,sizeof(Input1));
//	selector=(rand() % (3-0+1)) +0;

	unsigned short Output[200];
	unsigned short Rinput[200];
	//unsigned short inputsize=sizeof(Input)/sizeof(Input[0]);
	unsigned short outputsize=sizeof(Output)/sizeof(Output[0]);
	printf("inputsize=%hu\n",inputsize);//100筆
	printf("outputsize=%hu\n",outputsize);//200筆
	int i;
	//RLE input~~~~~~~~~~~~~~~~
//	for(i=0;i<inputsize;i++){
//		printf("%.1f\n",Input[i]);
//	}
	//RLE input~~~~~~~~~~~~~~~~
//	unsigned short K_value=(rand() % (3-0+1)) +0;
	unsigned short K_value=1;

	RLE_encoder(Input,inputsize,Output,&outputsize,K_value);

	//RLE output~~~~~~~~~~~~~~~~
////	printf("Compressed size=%.0f\n",outputsize);
//	for(i=0;i<outputsize;i+=2){
//		printf("%.1f:%.0f\n",Output[i],Output[i+1]);//壓縮後結果
//	}
	//RLE output~~~~~~~~~~~~~~~~

	//compressed ratio
	float CR;
	CR=(float)outputsize/inputsize;
	CR=1-CR;
	//printf("Finish compression Compressed ratio=%f K_value=%d \n",CR,K_value);
	//compressed ratio

	//restore data
	int j;
	int Rindex=0;

	for(i=0;i<outputsize;i+=2){

		for(j=0;j<Output[i+1];j++){
			Rinput[Rindex]=Output[i];
			Rindex++;
		}
	}
	//restore data

	//count distortion rate
	float DS=0;
	float diff=0;
	for(i=0;i<inputsize;i++){
		if(Input[i]!=Rinput[i]){
			diff++;
		}

	}
	DS=diff/inputsize;
	DistorRate+=DS;
	NumofData++;
	TransDataSize=outputsize*sizeof(Output[0]);
	printf("Finish compression Compressed ratio=%f K_value=%hu Distortion rate=%f\n",CR,K_value ,DS);
}

