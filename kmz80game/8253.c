/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "main.h"

/*
Timer3: 
	Emulates 8253 timer 0 using 2 Mhz input from REFCLKO. 
	Prescaler=1:1. Use OC1 with toggle mode.
Timer5: 
	Emulates 8253 timer 1 using 8 Mhz input from REFCLKO. 
	Prescaller=1:64, resulting in 31.25 Khz.
*/

/*	global variables
*/
static unsigned int g_countTimer2;
static unsigned int g_cacheTimer0,g_cacheTimer1,g_cacheTimer2;
static unsigned int g_lsbTimer0,g_lsbTimer1,g_lsbTimer2;

void init8253(){
	// Timer 5 isn't used.
	// Initialize global vers.
	g_TMR5=0;
	g_PR5=0xFFFF;
}

void inc8253timer2(){
	g_8253_timer1_carry=0;
	g_countTimer2--;
	if (g_countTimer2) return;
	g_countTimer2=0x00010000;
	// Interrupt on Z80 CPU
	intZ80(0xff);
}

// Interruption occurs every 1833 clocks (31245 Hz)
void int8253(){
	IFS0bits.T5IF=0;
	if (g_TMR5==g_PR5) {
		g_TMR5=0;
		g_8253_timer1_carry=1;
		return;
	}
	g_TMR5++;
}


unsigned char read8253(unsigned int addr){
	switch(addr&0x03){
		case 0:
			if (g_lsbTimer0) {
				g_lsbTimer0=0;
				g_cacheTimer0=PR3-TMR3+1;
				return g_cacheTimer0 & 0xFF;
			} else {
				g_lsbTimer0=1;
				return (g_cacheTimer0>>8) & 0xFF;
			}
		case 1:
			if (g_lsbTimer1) {
				g_lsbTimer1=0;
				g_cacheTimer1=0xffff-g_TMR5+1;
				return g_cacheTimer1 & 0xFF;		
			} else {
				g_lsbTimer1=1;
				return (g_cacheTimer1>>8) & 0xFF;
			}
		case 2:
			if (g_lsbTimer2) {
				g_lsbTimer2=0;
				g_cacheTimer2=g_countTimer2;
				return g_cacheTimer2 & 0xFF;		
			} else {
				g_lsbTimer2=1;
				return (g_cacheTimer2>>8) & 0xFF;
			}
		default:
			return 0;
	}
}

void write8253(unsigned int addr, unsigned char data){
	unsigned int i32;
	switch(addr&0x03){
		case 0:
			if (g_lsbTimer0) {
				g_lsbTimer0=0;
				g_cacheTimer0=data;
				return;
			} else {
				/*	8253 mode 2:
					When the timer in 8253 is, for example, set to 5, the 8253 counter changes as follows:
					5, 4, 3, 2, 1, 5, 4, 3, 2, 1, 5, 4, 3, 2, 1, 5, 4, 3, 2, 1, ...
					OUT is L when counter=1, otherwise, it is H.
					On the other hand, when PR3 is, for example, set to 4, TMR3 changes as follows:
					0, 1, 2, 3, 4, 0, 1, 2, 3, 4, ...
					Therefore, PR3 will be set as follows:
					PR3=value-1;
					Counter can be read as follows:
					value=PR3-TMR3+1;
				*/
				g_lsbTimer0=1;
				i32=data;
				i32<<=8;
				i32|=g_cacheTimer0;
				PR3=i32-1;
				return;
			}
		case 1:
				/* 8253 mode 0:
					Then the timer reach zero, it will be always 0xffff after this event.
					Therefore, the PR5 is fixed at 0xffff.
					Writing valut changes TMR5 instead of PR5.
				*/
			if (g_lsbTimer1) {
				g_lsbTimer1=0;
				g_cacheTimer1=data;
				return;
			} else {
				g_lsbTimer1=1;
				i32=data;
				i32<<=8;
				i32|=g_cacheTimer1;
				i32=0xffff-i32;
				g_TMR5=i32;
				return;
			}
		case 2:
				/* 8253 mode 0:
					This timer is emulated by software.
				*/
			if (g_lsbTimer2) {
				g_lsbTimer2=0;
				g_cacheTimer2=data;
				return;
			} else {
				g_lsbTimer2=1;
				i32=data;
				i32<<=8;
				i32|=g_cacheTimer2;
				g_countTimer2=i32;
				return;
			}
		default:
			switch(data&0xC0){
				case 0x00:
					g_lsbTimer0=1;
					g_cacheTimer0=PR3-TMR3+1;
					return;
				case 0x40:
					g_lsbTimer1=1;
					g_cacheTimer1=0xffff-g_TMR5+1;
 					return;
				case 0x80:
					g_lsbTimer2=1;
					g_cacheTimer2=g_countTimer2;
					return;
				default:
					return;
			}
	}
}

