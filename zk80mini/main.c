/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#define MAIN
#include "main.h"

// Clock settings
// SYSCLK = 40 Mhz, PBCLK = 40 Mhz.

// Primary oscillator not used.
#pragma config POSCMOD = OFF
// Fast RC Osc with PLL (~8 MHz)
#pragma config FNOSC = FRCPLL
// Div 2 to make 4 MHz
#pragma config FPLLIDIV = DIV_2
// Mul 24 to make 80 MHz
#pragma config FPLLMUL = MUL_20
// Div 2 to make 40 MHz
#pragma config FPLLODIV = DIV_2
// PBCLK = SYSCLK
#pragma config FPBDIV = DIV_1

// MISC disabled
#pragma config FSOSCEN = OFF
#pragma config IESO = OFF
#pragma config OSCIOFNC = OFF
#pragma config FCKSM = CSDCMD
#pragma config JTAGEN = OFF
#pragma config ICESEL = ICS_PGx1
#pragma config DEBUG = OFF
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF
#pragma config FUSBIDIO = OFF
#pragma config FVBUSONIO = OFF

// WDT disabled
#pragma config WDTPS = PS1, FWDTEN = OFF, WINDIS = OFF, FWDTWINSZ = WISZ_25

// protection all off
#pragma config PWP = OFF, BWP = OFF, CP = OFF

// Tiny test program
static const unsigned char PROG[]={
0x01,0x00,0x49,0x21,0xf8,0x83,0xb5,0x6f,0x70,0x06,0x09,0xcd,0xef,0x02,0x05,0xc2,
0x0b,0x80,0x3c,0x71,0xc3,0x00,0x80
/*
(8000)
loop:
LD    BC,4900
LD    HL,83F8
OR    L
LD    L,A
LD    (HL),B
LD    B,09
wait:
CALL  02EF
DEC   B
JP    NZ,wait:
INC   A
LD    (HL),C
JP    loop:
*/
};

void main(){
	long i;
	// Weak pull up for RB3
	CNPUB=0x08;
	// RA0, RA1, RB2, RB3: digital inputs but not analog ones
	ANSELA=0x1C;
	ANSELB=0xF3;
	TRISA=0x1F;
	// Initialize interface (TRISB will be also set here.)
	init_led();
	// Other initializations
	INTEnableSystemSingleVectoredInt();
	INTEnableSystemMultiVectoredInt();
	resetZ80();
	// Load test program
	// See main.h; RAM is set as persistent.
	if (RCONbits.POR){
		RCONbits.POR=0;
		for(i=0;i<sizeof PROG;i++){
			RAM[i]=PROG[i];
		}
	}

	while(1){
		// Wait until next timing to execute Z80 code
		while((unsigned long)(g_TMR1h|TMR1)<g_timer1);
		// Now, execute the Z80 code.
		// Note that g_timer1 will increment due to each code's T cycles
		execZ80code();
		// Interrupt
		if (!PORTBbits.RB3) {
			triggerInt();
		}
	}
}
