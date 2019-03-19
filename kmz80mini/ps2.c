/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "main.h"
#include "charcode.h"

/*
	PORTB8: data
	PORTB9: clock (INT1)
*/

#define ps2_clock_out TRISBbits.TRISB9
#define ps2_data_out  TRISBbits.TRISB8
#define ps2_clock_in  PORTBbits.RB9
#define ps2_data_in   PORTBbits.RB8
#define ps2_clock_lat LATBbits.LATB9
#define ps2_data_lat  LATBbits.LATB8

void ps2_init(){
	// Weak pull-up
	CNPUBbits.CNPUB8=1;
	CNPUBbits.CNPUB9=1;
	// IDLE mode
	ps2_clock_lat=0;
	ps2_data_lat=0;
	ps2_clock_out=1;
	ps2_data_out=1;

	// INT1 is assigned to RB9 pin (clock in)
	// Enable interrupt as priority 5
	INT1R=4;
	INTCONbits.INT1EP=0; // Falling edge
	IPC1bits.INT1IP=5;
	IPC1bits.INT1IS=0;
	IFS0bits.INT1IF=0;
	IEC0bits.INT1IE=1;

}

//#pragma interrupt ps2Int IPL5SOFT vector 15
#pragma interrupt ps2Int IPL5SOFT vector 7

void ps2Int() {
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

