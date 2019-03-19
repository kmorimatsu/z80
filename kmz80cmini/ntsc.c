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
	15734 hz corresponds to 3050.72 clocks of 48 Mhz CPU frequency.
	Use 3048 clocks, resulting in 15748 hz
	Note that 508 clocks of 8 MHz (=SPI clock speed) also produces the same frequency.
*/

/*	Minimum time between two interruption events is 2.3 usecs,
	that corresponds to 92 colcks.
*/


void initNtsc(){
	unsigned long i;
	for (i=0;i<0x0400;i++) {
		VRAM[i]=i&0xFF;
	}
	g_ntscVisible=1;
	g_Vsync=1;
	sync=1;

	// Set Hsync and video pins to output ones
	KMZ80_TRIS_HSYNC=0;
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
	// SPI clock=REFCLK/(2x(SPI2BRG+1))=8 Mhz. (REFCLK=48 Mhz; SPI2BRG=2)
	// All extended setting are off (SPI2CON2).

	SPI2CON=0x0100A83C;
	SPI2CON2=0x0300;
	SPI2BRG=2;
	// SDO2 pin selection
	KMZ80_PPS_SDO2;
}

/*	Sync signals:
	raster 1-3: L for 2.3 usecs, H for 29.5 usec, L for 2.3 usecs, H for 29.5 usec
	(110)         (110)
	2.3u__________2.3u__________                 110+1414+110+1414=3048
	|__|          |__|          
	       29.5u         29.5u
          (1414)        (1414)

	raster 4-6: L for 27.1 usecs, H for 4.7 usec, L for 27.1 usecs, H for 4.7 usec
      (1299)        (1299)
	   27.1u    __   27.1u    __                 1299+225+1299+225=3048
	|__________|  |__________|  
	           4.7u          4.7u
              (225)         (225)

	raster 7-9: L for 2.3 usecs, H for 29.5 usec, L for 2.3 usecs, H for 29.5 usec
	(110)         (110)
	2.3u__________2.3u__________                 110+1414+110+1414=3048
	|__|          |__|          
	       29.5u         29.5u
          (1414)        (1414)

	raster 10-262: H for 1.5 usecs, L for 4.7 usecs, H for 57.4 usecs
      (225)
	___4.7u_____________________                 72+225+2751=3048
	   |__|          
	1.5u          57.4u
    (72)         (2751)

	This must produces 16275 Hz sync signal.
	Following intterupt function takes about 1 micro second.
	This will be 0.5 micro second if IPL7SRS will be used.
*/

const unsigned short ntscSyncTiming[]={
// raster 1-3
	110-1,1414-1,110-1,1414-1,110-1,1414-1,110-1,1414-1,110-1,1414-1,110-1,1414-1,
// raster 4-6
	1299-1,225-1,1299-1,225-1,1299-1,225-1,1299-1,225-1,1299-1,225-1,1299-1,225-1,
// raster 7-9
	110-1,1414-1,110-1,1414-1,110-1,1414-1,110-1,1414-1,110-1,1414-1,110-1,1414+72-1,
// raster 10-259
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,225-1,2751+72-1,
// raster 260-262
	225-1,2751+72-1,225-1,2751+72-1,225-1,2751-1

};
void ntscSyncInt(){
	static unsigned long status=0;
	unsigned int i16;
	IFS0CLR=_IFS0_T2IF_MASK; //IFS0bits.T2IF=0;
	if (TMR2&1) asm volatile("nop");
	toggleSync();
	PR2=i16=ntscSyncTiming[status++];
	if (i16==(2751-1)) {
		// The end of the sequence of statuses.
		// Reset parameter.
		sync=1;
		status=0;
		// Trigger Timer4 for video signal
		// Note that video signal starts from raster 42.
		// Also note that timer for video works at 24 Mhz.
		TMR4=TMR2>>1;
		PR4=(2751+3048*41+72+225)/2+40-1; // Needs to be tuned to define horizontal positoin.
		if (g_Vgate) T4CONSET=_T4CON_ON_MASK; //T4CONbits.ON=1;
		g_Vsync=0; // Pseudo vertical sync signal starts here.
	}
}
/*	Video signal construction
	1 raster: 1525 cycles of timer working at 24 Mhz (24 clocks/usec; 24 clocks/8 dotes)
	status:  1    2    3    4    5    6    7 
	         |    |    |    |    |    |    |
              (113)
	       ___4.7u___________________________
	          |__|          
	       1.5u            57.4u
           (30)           (1381)
	// This routine takes about 2 micro seconds.
	// This will take about 1 micro second if IPL7SRS will be used.
*/
void ntscVideoInt(){
	static unsigned long status=1;
	static unsigned long addr=0;
	static unsigned long fontLine=0;
	unsigned long i1,i2,i3,i4;
	static const unsigned char* cgrom_line;
	IFS0CLR=_IFS0_T4IF_MASK; //IFS0bits.T4IF=0;
	switch(status){
		case 1:
			status=2;
			g_Vsync=1; // Pseudo vertical sync signal ends here.
			if (!g_ntscVisible) {
				T4CONCLR=_T4CON_ON_MASK; //T4CONbits.ON=0;
			} else {
				PR4=190; // Needs to be tuned (must be less than 192, more than 170.6).
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
					break;
				}
			}
			PR4=1524-(190+1)*6-1;
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