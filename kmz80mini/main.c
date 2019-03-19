/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#define MAIN
#include "main.h"

// Clock settings
// SYSCLK = 48 Mhz, PBCLK = 48 Mhz.

// Primary oscillator setting, XT for 4 MHz (between 3 and 10)
// This setting can be also used for external oscillator input
#pragma config POSCMOD = XT
// Primary Osc with PLL
#pragma config FNOSC = PRIPLL
// Div 1 to make 4 MHz
#pragma config FPLLIDIV = DIV_1
// Mul 24 to make 96 MHz
#pragma config FPLLMUL = MUL_24
// Div 2 to make 48 MHz
#pragma config FPLLODIV = DIV_2
#pragma config FPBDIV = DIV_1

// Test with internal RC oscillator
//#pragma config FNOSC = FRCPLL
//#pragma config FPLLIDIV = DIV_2


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

void main(){
	// Output 2 MHz from RPB2 as REFCLKO
	// REFCON: RODIV=0x18 (divide by 48), ON=1, SIDL=1, OE=1, RSLP=0, 
	// DIVSWEN=1, ACTIVE=1, ROSEL=0111 (System PLL)
	// Output REFCLKO to RPB2
	REFOTRIMbits.ROTRIM=0;
	REFOCON=0x0018B307;
	RPB2R=7;
	// Initialization routines
	loadTape();
	ps2_init();
	initNtsc();
	init8255();
	init_cmt();
	init8253();
	resetZ80();
	INTEnableSystemSingleVectoredInt();
	INTEnableSystemMultiVectoredInt();
	// Timer1 settings, ON=1, SIDL=1, TWDIS=0, TGATE=0, 1:1 prescale, TCS=0
	PR1=0xFFFF;
	TMR1=0;
	g_TMR1h=0;
	g_timer1=0;
	IFS0bits.T1IF=0;
	T1CON=0xA000;
	// Timer1 interrupt: priority 4
	IPC1bits.T1IP=4;
	IPC1bits.T1IS=0;
	IEC0bits.T1IE=1;
	// RB4 and RB5 are output pins for LEDs
	TRISBbits.TRISB5=0;
	TRISBbits.TRISB4=0;
	LATBbits.LATB5=0; // Green
	LATBbits.LATB4=1; // Red
	
	// Main loop follows.
	while(1){
		// Wait until next timing to execute Z80 code
		while((unsigned long)(g_TMR1h|TMR1)<g_timer1);
		// Now, execute the Z80 code.
		// Note that g_timer1 will increment due to each code's T cycles
		execZ80code();
	}
}

#pragma interrupt timer1Int IPL4SOFT vector 4

void timer1Int(){
	static unsigned int r_seed=0,r_data=0;
	// This occurs every 1.3653 msec (732 Hz).
	IFS0CLR=_IFS0_T1IF_MASK; //IFS0bits.T1IF=0;

	// periodically but randomly change output current for the stable oscillation
	if (0xffff<r_data) {
		if (read825x[2]&0x04) {
			LATBbits.LATB5=0;
			LATBbits.LATB4=1;
		} else {
			LATBbits.LATB5=1;
			LATBbits.LATB4=0;
		}
		r_data=((unsigned short*)monitor)[r_seed];
		r_data|=0xf000;
		r_data&=0xfeff;
		r_seed++;
		r_seed&=0x7ff;
	} else {
		r_data++;
		if (0xffff<r_data) {
			LATBbits.LATB5=1;
			LATBbits.LATB4=1;		
		}
	}

/* Debugging codes follow (comment out for releasing).
VRAM[500]=(read825x[2]&0x20)?0x21:0x20;
VRAM[501]=(read825x[2]&0x10)?0x21:0x20;
VRAM[502]=(AD1CON1bits.ON)?0x21:0x20;

/*
TRISBbits.TRISB2=1;
ANSELBbits.ANSB2=1;
//AD1CON1bits.DONE=0;
//AD1CON1bits.SAMP=0;
//while (!AD1CON1bits.DONE);
VRAM[502]=(ADC1BUF0&0x00f)|0x20;
VRAM[501]=((ADC1BUF0&0x0f0)>>4)|0x20;
VRAM[500]=((ADC1BUF0&0xf00)>>8)|0x20;
VRAM[540]=AD1CON1bits.SAMP?0x21:0x20;
VRAM[541]=AD1CON1bits.DONE?0x21:0x20;
//*/
	g_TMR1h+=0x10000;
	if (g_TMR1h==0) g_timer1&=0xffff;

	// Environmental supports follow.

	// Two 555 timers for tempo and prompt
	if (!((++tempo_8255)&0x7)) {
		read825x[8]^=1; // tempo
	}
	if (!((++prompt_8255)&0xff)) {
		read825x[2]^=0x40; // prompt
	}

	// Emulate 8253 timer 2
	if (IFS0bits.T5IF) inc8253timer2();

	// Vgate support
	// Video off when VGATE=0 for more than 20 msec.
	if (read825x[2]&0x01) {
		g_Vgate=15;
	} else if (g_Vgate) {
		g_Vgate--;
	}

	// PS2 key input
	if (0x200000<ps2status) {
		if ((ps2data&0x200800)==0x200000) {
			// Parity check is not done.
			ps2_received((ps2data>>12)&0xff);			
		}
		ps2status=0x800;
		ps2data&=0x7ff;
	}
	if (ps2status==0x800) {
		if ((ps2data&0x401)==0x400) {
			// Parity check is not done.
			ps2_received((ps2data>>1)&0xff);
		}
		ps2status=1;
		ps2data=0;
	}
}