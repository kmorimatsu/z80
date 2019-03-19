/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/
#define KMZ80_NTSC
#include "main.h"
#include "cgrom.h"

/*
	Raster frequency is 15734 hz, so one raster takes 63.56 usecs.
	Raster frequency cange between 15734-15750 hz (15750 hz was used for B/W TV)
	15734 hz corresponds to 3640.06 clocks of 57.2 Mhz CPU frequency.
	Use 3640 clocks, resulting in 15734 hz
	Note that 455 clocks of 7.16 MHz (=SPI clock speed) also produces the same frequency.
*/

/*	Minimum time between two interruption events is 2.3 usecs,
	that corresponds to 92 colcks.
*/

int checkAnalog(volatile unsigned int * output_set, volatile unsigned int * output_clr, int output_bit, int an_num){
	int i,vl,vh;
	// A/D converter settings follow
	// Internal counter ends sampling and starts conversion (auto convert) (SSRC=111)
	// Sampling begins when SAMP bit is set (ASAM=0)
	// ON=1, SIDL=1, FORM=000, SSRC=111
	// CLRASAM=0, ASAM=0, SAMP=0, DONE=0
	AD1CON1=0xa0e0;
	// VCFG=000, OFFCAL=0,
	// CSCNA=0, BUFS=0, SMPI=0000, NUFM=0,
	// ALTS=0
	AD1CON2=0x0000;
	// TAD=48/16=3 MHz
	// ADRC=0, SAMC=11111, ADCS=000000100
	// Sampling frequency: 94 KHz
	AD1CON3=0x1f04;
	// CH0NA=0, CH0SA=an_num (ANx)
	AD1CHS=0x00010000*an_num;

	for (i=0;i<100;i++) {
		// Lower voltage input
		output_clr[0]=1<<output_bit;
		AD1CON1bits.DONE=0;
		AD1CON1bits.SAMP =1;
		while(!AD1CON1bits.DONE);
		vl=ADC1BUF0;
		output_set[0]=1<<output_bit;
		// Higher voltage input
		AD1CON1bits.DONE=0;
		AD1CON1bits.SAMP =1;
		while(!AD1CON1bits.DONE);
		vh=ADC1BUF0;
		if (vh<vl+50) {
			// Voltage change was not detected.
			// RA1 is not connected to video line.
			AD1CON1=0x0000;
			return 0;
		}
	}
	AD1CON1=0x0000;
	return 1;
}
int checkRA1(){
	ANSELAbits.ANSA1=1; // Analog mode for RA1
	TRISAbits.TRISA1=1; // input from RA1
	// Use RB3 for output, use AN1 for input
	return checkAnalog(&LATBSET,&LATBCLR,3,1);
}

int checkRB6(){
	ANSELBbits.ANSB3=1; // Analog mode for RB3
	TRISBbits.TRISB3=1; // input from RB3
	// Use RB6 for output, use AN5 for input
	return checkAnalog(&LATBSET,&LATBCLR,6,5);
}


void initNtsc(){
	unsigned long i;
	for (i=0;i<0x0400;i++) {
		VRAM[i]=i&0xFF;
	}
	g_ntscVisible=1;
	g_Vsync=1;
	sync=1;

	// Check if RA1/RB6 is connected to video line
	// If true, use RA1/RB6 for output as video signal (SDO2)
	if (checkRA1()) {
		TRISAbits.TRISA1=0;
		RPA1R=4;
	} else if (checkRB6()) {
		TRISBbits.TRISB6=0;
		RPB6R=4;
	}
	// Set Hsync pin to output ones
	KMZ80_TRIS_HSYNC=0;
	// Set video pin to output ones
	KMZ80_TRIS_VIDEO=0;

	// Timer2 is used for sync signal
	// On, Stop in Idle Mode, PBCLK, 1:1 prescale, 16-bit mode
	// Enable interrupt as priority 7
	TMR2=0;
	PR2=0xFFFF;
	T2CON=0xA000;
	T2CONbits.ON=1;
	IPC2bits.T2IP=7;
	IPC2bits.T2IS=0;
	IFS0bits.T2IF=0;
	IEC0bits.T2IE=1;

	// Timer4 is used for video signal
	// Off, Stop in Idle Mode, PBCLK, 1:2 prescale, 16-bit mode
	// Enable interrupt as priority 6
	TMR4=0;
	PR4=0xFFFF;
	T4CON=0x2010;
	IPC4bits.T4IP=6;
	IPC4bits.T4IS=0;
	IFS0bits.T4IF=0;
	IEC0bits.T4IE=1;

	// SPI2 module settings follow
	// PBCLOCK, Enable SPI2, Stop in Idle Mode, Enable SDO, 32 bit mode,
	// SS pin not used, Master mode, Disable SDI
	// SPI clock=REFCLK/(2x(SPI2BRG+1))=7.16 MHz Mhz. (REFCLK=57.28 Mhz; SPI2BRG=3)
	// All extended setting are off (SPI2CON2).

	SPI2CON=0x0100A83C;
	SPI2CON2=0x0300;
	SPI2BRG=3;
	// SDO2 pin selection
	KMZ80_PPS_SDO2;
}

/*	Sync signals:
	raster 1-3: L for 2.3 usecs, H for 29.5 usec, L for 2.3 usecs, H for 29.5 usec
	(132)         (132)
	2.3u__________2.3u__________                 132+1688+132+1688=3640
	|__|          |__|          
	       29.5u         29.5u
          (1688)        (1688)

	raster 4-6: L for 27.1 usecs, H for 4.7 usec, L for 27.1 usecs, H for 4.7 usec
      (1551)        (1551)
	   27.1u    __   27.1u    __                 1551+269+1551+269=3640
	|__________|  |__________|  
	           4.7u          4.7u
              (269)         (269)

	raster 7-9: L for 2.3 usecs, H for 29.5 usec, L for 2.3 usecs, H for 29.5 usec
	(132)         (132)
	2.3u__________2.3u__________                 132+1688+132+1688=3640
	|__|          |__|          
	       29.5u         29.5u
          (1688)        (1688)

	raster 10-262: H for 1.5 usecs, L for 4.7 usecs, H for 57.4 usecs
      (269)
	___4.7u_____________________                 86+269+3285=3640
	   |__|          
	1.5u          57.4u
    (86)         (3285)

	This must produces 16275 Hz sync signal.
	Following intterupt function takes about 1 micro second.
	This will be 0.5 micro second if IPL7SRS will be used.
*/

const unsigned short ntscSyncTiming[]={
// raster 1-3
	132-1,1688-1,132-1,1688-1,132-1,1688-1,132-1,1688-1,132-1,1688-1,132-1,1688-1,
// raster 4-6
	1551-1,269-1,1551-1,269-1,1551-1,269-1,1551-1,269-1,1551-1,269-1,1551-1,269-1,
// raster 7-9
	132-1,1688-1,132-1,1688-1,132-1,1688-1,132-1,1688-1,132-1,1688-1,132-1,1688+86-1,
// raster 10-259
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,269-1,3285+86-1,
// raster 260-262
	269-1,3285+86-1,269-1,3285+86-1,269-1,3285-1

};
void ntscSyncInt(){
	static unsigned long status=0;
	unsigned int i16;
	IFS0CLR=_IFS0_T2IF_MASK; //IFS0bits.T2IF=0;
	if (TMR2&1) asm volatile("nop");
	toggleSync();
	PR2=i16=ntscSyncTiming[status++];
	if (i16==(3285-1)) {
		// The end of the sequence of statuses.
		// Reset parameter.
		sync=1;
		status=0;
		// Trigger Timer4 for video signal
		// Note that video signal starts from raster 42.
		// Also note that timer for video works at 28.6 Mhz.
		TMR4=TMR2>>1;
		PR4=(3285+3640*11+86+269)/2-70-1;// Needs to be tuned to define horizontal positoin.
		if (g_Vgate) T4CONSET=_T4CON_ON_MASK; //T4CONbits.ON=1;
		g_Vsync=0; // Pseudo vertical sync signal starts here.
	}
	// 8253 support
	if ((i16!=1551-1) && (i16!=132-1)) {
		// 31468Hz timer
		if (g_TMR5==g_PR5) {
			g_TMR5=0;
			g_8253_timer1_carry=1;
		} else {
			g_TMR5++;
		}
	}
}
/*	Video signal construction
	1 raster: 1820 cycles of timer working at 28.6 Mhz (24 clocks/usec; 24 clocks/8 dotes)
	status:  1    2    3    4    5    6    7 
	         |    |    |    |    |    |    |
              (135)
	       ___4.7u___________________________
	          |__|          
	       1.5u            57.4u
           (43)           (1642)
	// This routine takes about 2 micro seconds.
	// This will take about 1 micro second if IPL7SRS will be used.
*/
void ntscVideoInt(){
	static unsigned long status=0;
	static unsigned long addr=0;
	static unsigned long fontLine=0;
	unsigned long i1,i2,i3,i4;
	static const unsigned char* cgrom_line;
	IFS0CLR=_IFS0_T4IF_MASK; //IFS0bits.T4IF=0;
	switch(status){
		case 0:
			PR4=(3640*30)/2-1;
			status=1;
			break;
		case 1:
			status=2;
			g_Vsync=1; // Pseudo vertical sync signal ends here.
			if (!g_ntscVisible) {
				T4CONCLR=_T4CON_ON_MASK; //T4CONbits.ON=0;
			} else {
				PR4=255; // Needs to be tuned.
				cgrom_line=&cgrom[fontLine];
			}
			break;
		case 7:
			status=1;
			if (fontLine<7*256) {
				fontLine+=256;
				addr-=40;
			} else {
				fontLine=0;
				if (999<addr) {
					// Video signal was already end. Let's stop timer
					T4CONCLR=_T4CON_ON_MASK; //T4CONbits.ON=0;
					// Reset parameters
					addr=0;
					fontLine=0;
					status=0;
					break;
				}
			}
			PR4=1820-(PR4+1)*6-1;
			break;
		default:
			status++;
			i1=(cgrom_line[VRAM[addr+0]])<<24;
			i2=(cgrom_line[VRAM[addr+1]])<<16;
			i3=(cgrom_line[VRAM[addr+2]])<<8;
			i4=(cgrom_line[VRAM[addr+3]]);
			SPI2BUF=i1|i2|i3|i4;
			addr+=4;
			i1=(cgrom_line[VRAM[addr+0]])<<24;
			i2=(cgrom_line[VRAM[addr+1]])<<16;
			i3=(cgrom_line[VRAM[addr+2]])<<8;
			i4=(cgrom_line[VRAM[addr+3]]);
			SPI2BUF=i1|i2|i3|i4;
			addr+=4;
			break;
	}
}