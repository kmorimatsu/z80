/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

/*
	When the linker script is used, hex product for SD-card boot loader
	is made. Otherwise, simple hex file is made and can be directly donloaded
	into chip.
	Program Memory (as shown in Memory Usage Gauge) must be
	equal to or less than 25084 words. To measure this, do not use linker script.
*/

#define KMZ80_MAIN
#include "main.h"

// Clock settings
// SYSCLK = 57272720 hz, PBCLK = 57272720 hz.

// Primary oscillator setting, XT for 3.58 MHz (between 3 and 10)
// This setting can be also used for external oscillator input
#pragma config POSCMOD = XT

// Primary Osc with PLL
#pragma config FNOSC = PRIPLL
// Div 1 to make 3.58 MHz
#pragma config FPLLIDIV = DIV_1
// Mul 16 to make 57 MHz
#pragma config FPLLMUL = MUL_16
// Div 1 to make 57 MHz
#pragma config FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_1

// MISC disabled
#pragma config FCKSM = CSDCMD
#pragma config IESO = OFF
#pragma config FSOSCEN = OFF
#pragma config OSCIOFNC = OFF
#pragma config JTAGEN = OFF
#pragma config DEBUG = OFF
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF

// WDT disabled
#pragma config WDTPS = PS1048576
#pragma config FWDTEN = OFF
#pragma config WINDIS = OFF
#pragma config FWDTWINSZ = WISZ_25

// protection all off
#pragma config PWP = OFF, BWP = OFF, CP = OFF

void main(){
	//SYSTEMConfigPerformance(57272720);
	// Load MZF file first (using SPI2 for SD-card)
	sd_loadMZF();
	// Initialization routines
	INTEnableSystemMultiVectoredInt();
	initNtsc();
	sd_error();
	keys_init();
	init8255();
	init8253();
	resetZ80();
	if (useps2) {
		// Use PS/2
		ps2_enable_tris=0;
		ps2_enable_out=1;
		ps2_init();
	} else {
		ps2_enable_tris=0;
		ps2_enable_out=0;
	}
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
	// Two output pins for green and red LEDs
	KMZ80_TRIS_GLED=0;
	KMZ80_TRIS_RLED=0;
	KMZ80_LAT_GLED=0; // Green
	KMZ80_LAT_RLED=1; // Red
 
	while(1){
		// Wait until next timing to execute Z80 code
		// Two timer values are compaired as either signed or unsigned ones.
		// This way will eliminate the problem when g_TMR1h|TMR1 exceeds either 0x7FFFFFFF or 0xFFFFFFFF but g_timer1 does not.
		if (g_timer1&0x80000000) {
			while((signed long)(g_TMR1h|TMR1)<(signed long)g_timer1);
		} else {
			while((unsigned long)(g_TMR1h|TMR1)<g_timer1);
		}
		// Now, execute the Z80 code.
		// Note that g_timer1 will increment due to each code's T cycles
		execZ80code();
	}
}

void timer1Int(){
	static unsigned int r_seed=0,r_data=0;
	// This occurs every 1.1443 msec (873 Hz).
	IFS0CLR=_IFS0_T1IF_MASK; //IFS0bits.T1IF=0;

	// Increment higher bits of timer
	g_TMR1h+=0x10000;

	// Environmental supports follow.

	// Two 555 timers for tempo and prompt
	if (!((++tempo_8255)&0x7)) {
		read825x[8]^=1; // tempo
	}
	if (!((++prompt_8255)&0xff)) {
		read825x[2]^=0x40; // prompt
	}

	// Check key inputs
	if (!useps2) keys_check();

	// Emulate 8253 timer 2
	if (g_8253_timer1_carry) inc8253timer2();

	// Vgate support
	// Video off when VGATE=0 for more than 20 msec.
	if (read825x[2]&0x01) {
		g_Vgate=15;
	} else if (g_Vgate) {
		g_Vgate--;
	}

	// PS2 key input
	if (useps2) {
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
}

