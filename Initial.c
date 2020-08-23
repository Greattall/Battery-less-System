/*
 * Initial.c
 *
 *  Created on: Aug 17, 2020
 *      Author: LeeKaiXuan
 */
#include <Initial.h>

void prvSetupHardware ( void )
{
    /* Stop Watchdog timer. */
    WDT_A_hold( __MSP430_BASEADDRESS_WDT_A__ );
    P1OUT = 0x00;
    P1DIR = 0xFF;
    P2OUT = 0x00;
    P2DIR = 0xFF;
    P3OUT = 0x00;
    P3DIR = 0xFF;
    P4OUT = 0x00;
    P4DIR = 0xFF;
    P5OUT = 0x00;
    P5DIR = 0xFF;
    P6OUT = 0x00;
    P6DIR = 0xFF;
    P7OUT = 0x00;
    P7DIR = 0xFF;
    P8OUT = 0x00;
    P8DIR = 0xFF;
    PJOUT = 0x00;
    PJDIR = 0xFF;
    PMM_unlockLPM5();
}

void initClock ( void )
{
    PJSEL0 |= BIT4 | BIT5;                  // For XT1

    // XT1 Setup
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL1 = DCOFSEL_6;                     // Set DCO to 8MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__2 | DIVM__1;   // set all dividers
   // CSCTL3 = DIVA__4 | DIVS__4 | DIVM__4;
    //DIVA:ACLK DIVS:SMCLK DIVM MCLD
    CSCTL4 &= ~LFXTOFF;

    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    }
    while ( SFRIFG1 & OFIFG );              // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers
}

void initUART ( void )
{
     P2SEL1 |= BIT5| BIT6;                     // USCI_A1 UART operation //SET RX TX  2.5Tx 2.6Rx
     P2SEL0 &= ~(BIT5| BIT6);                 // USCI_A1 UART operation   //SET RX TX
//     P1DIR|=BIT0;
//     P1DIR|=BIT1;
     // Configure USCI_A1 for UART mode
     UCA1CTLW0 = UCSWRST;                      // Put eUSCI in reset
     UCA1CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK

   // UCA1BR0 = 26;                             // 4000000/16/9600 BRF_0
     // UCA1BR0 = 4;                             // 4000000/16/57600 BRF_5
  //    UCA1BR0 = 4;                             // 4000000/16/60000 BRF_2
     UCA1BR0 = 2;                             // 4000000/16/115200 BRF_2

     UCA1BR1 = 0x00;
     UCA1MCTLW |= UCOS16 | UCBRF_2;
     UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
     UCA1IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt
    // __bis_SR_register(GIE);

   //  __bis_SR_register(LPM3_bits | GIE);       // Enter LPM3, interrupts enabled
}

void initInterBut ( void )
{
//  P1DIR |= 0X03;
    P5DIR &= ~BIT5 + ~BIT6;
    P5REN |= BIT5 + BIT6;
    P5OUT |= BIT5 +BIT6;
    P5IES |= BIT5 +BIT6;
    P5IFG &= ~BIT5 + ~BIT6;
    P5IE |= BIT5+BIT6;
    __bis_SR_register(GIE);
}

void init_ADC ( void )
{
    //If we want to use multiADC must +ADC12MSC , ADC12CONSEQ_1 , ADC12EOS
    //Select P1.4(A4) and P1.5(A5) pin
    REFCTL0 |= REFVSEL_1 | REFON;//Set ref voltage to 2V , default = 1.2V
    P1SEL1 |= BIT4+BIT5;
    P1SEL0 |= BIT4+BIT5;
    ADC12CTL0 |=ADC12SHT0_4 | ADC12ON | ADC12MSC;//SHT: sample and hold time
    ADC12CTL1 |=ADC12SSEL_0 |ADC12SHP|ADC12CONSEQ_3;//SHP :select source of the sample signal  0:sample input signal 1:sampling timer
    //MODCLK: Internal low-power oscillator with 5-MHz typical frequency.
    ADC12CTL2 |=ADC12RES_2|ADC12PWRMD; //resolution 0:8bit 1:10bit 2:12bit
    ADC12CTL3 = ADC12TCMAP;
   ADC12MCTL0 |= ADC12INCH_4;//Select A4 ref=VCC,default
//    ADC12MCTL0 |= ADC12VRSEL_1 | ADC12INCH_4;//Select A4 ref=2.5V
    ADC12MCTL1 |= ADC12VRSEL_1 | ADC12INCH_30 ;
    ADC12MCTL2 |= ADC12INCH_5 | ADC12EOS;//Select A5
  //  while(!(REFCTL0 & REFGENRDY));          // Wait for reference generator
    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 |= ADC12SC;               // Sampling and conversion start
}

void initRTC ( void )
{
    RTCCTL0_H = RTCKEY_H;
    RTCCTL0_L = RTCTEVIE_L;//Real-time clock time event interrupt enable. In modules supporting LPM3.5 this interrupt can be used as LPM3.5 wake-up event.
    RTCCTL13 = RTCSSEL_0 | RTCTEV_0 | RTCHOLD ;//Use 32768Hz crystal oscillator clock

    RTCPS0CTL = RT0PSDIV_0;// Divide 2
    RTCPS1CTL = RT1SSEL_2 | RT1PSDIV_6;//Divide 128

    //32768 /2 /128 =128Hz
    //So my system Time tick resolution is 1/128 sec = 7.8125ms

    RTCCTL13 &= ~(RTCHOLD);
}
