/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/
#define KMZ80_PS2
#include "main.h"
#include "charcode.h"

void ps2_init(){
	// IDLE mode
	ps2_clock_lat=0;
	ps2_data_lat=0;
	ps2_clock_out=1;
	ps2_data_out=1;

	// INT1 is assigned some pin (clock in)
	// Enable interrupt as priority 5
	KMZ80_PPS_INT1;
	INTCONbits.INT1EP=0; // Falling edge
	IPC1bits.INT1IP=5;
	IPC1bits.INT1IS=0;
	IFS0bits.INT1IF=0;
	IEC0bits.INT1IE=1;

}

void ps2Int() {
	// See also timer1Int() in main.c
	IFS0CLR=_IFS0_INT1IF_MASK; //IFS0bits.INT1IF=0;
	if (ps2_data_in) ps2data|=ps2status;
	ps2status<<=1;
}

void ps2_received(unsigned char data){
	static unsigned char breakcode=0;
	if (data==0xF0) {
		breakcode=1;
	} else if (0<data && data<0x90) {
		data=charcode[data];
		// change 8255 status here
		if (breakcode) {
			// Key up
			breakcode=0;
			keymap[data>>3]|=1<<(data&0x07);
		} else {
			// Key down
			keymap[data>>3]&=0xff^(1<<(data&0x07));
		}
	} else {
		breakcode=0;
	}
}

