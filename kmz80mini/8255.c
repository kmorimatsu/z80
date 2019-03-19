/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "main.h"

void init8255(){
	int i;
	for (i=0;i<16;i++) {
		keymap[i]=0xff;
	}
	// Prepare for CMT
	TRISBbits.TRISB2=1;
	ANSELBbits.ANSB2=1;
	LATBbits.LATB2=1;
	// A/D converter (for CMT reading) settings follow
	// ON=1, SIDL=1, FORM=000, SSRC=111
	// CLRASAM=0, ASAM=1, SAMP=0, DONE=0
	AD1CON1=0xa0e4;
	// VCFG=000, OFFCAL=0,
	// CSCNA=0, BUFS=0, SMPI=0000, NUFM=0,
	// ALTS=0
	AD1CON2=0x0000;
	// TAD=48/16=3 MHz
	// ADRC=0, SAMC=11111, ADCS=000000100
	// Sampling frequency: 94 KHz (enough for 1400 baud CMT)
	AD1CON3=0x1f04;
	// CH0NA=0, CH0SA=0100
	AD1CHS=0x00040000;
}

void write8255(unsigned int addr, unsigned char data){
	int i;
	switch (addr&0x03) {
		case 0: // PORT A
			if (!(data&0x80)) {
				// reset 555 timer
				prompt_8255=0;
				read825x[2]&=0xbf;
			}
			read825x[1]=keymap[data&0x0f];
			return;
		case 1: // PORT B (read only)
			return;
		case 2: // PORT C (0-3: write, 4-7: read)
			for (i=0;i<4;i++) {
				write8255(3,(i<<1)|((data&(1<<i))?1:0));
			}
			return;
		case 3: // Control
		default:
			if (data&0x80) return; // Ignore I/O settings
			switch(data&0x0e){
				case 0: // PC0: /VGATE
					if (data&1) {
						read825x[2]|=0x01;
					} else {
						read825x[2]&=0xfe;
					}
					return;
				case 2: // PC1: CMT out
					if (data&1) {
						read825x[2]|=0x02;
						TRISBbits.TRISB2=0;
					} else {
						read825x[2]&=0xfd;
						TRISBbits.TRISB2=1;
					}
					return;
				case 4: // PC2: LED 1:green, 2: red
					if (data&1) {
						read825x[2]|=0x04;
						LATBbits.LATB5=0;
						LATBbits.LATB4=1;
					} else {
						read825x[2]&=0xfb;
						LATBbits.LATB5=1;
						LATBbits.LATB4=0;
					}
					return;
				case 6: // PC3: CMT motor ON/OFF
					if (data&1) {
						read825x[2]|=0x08;
						read825x[2]^=0x10; // Change motor status (ON/OFF)
			            if (read825x[2]&0x10) {
			                // motor on
			                timer3_cmt();
			            } else {
			                // motor off; let's go to sound mode of Timer3
			                timer3_sound();
			            }
					} else {
						read825x[2]&=0xf7;
					}
					return;
				default:
					return;
			}
	}
}