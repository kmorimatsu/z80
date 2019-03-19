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

	// Timer3 (of PIC) setting
	TMR3=0;
	T3CONbits.ON=0;       // 8253 timer 0 is stopped first.
	T3CONbits.TCKPS=5;    // 32:1 prescaler for 1.79 MHz
	T3CONbits.TCS=0;      // Use internal clock
	T3CONbits.SIDL=1;     // Stop in idle mode

	// OC4 setting for RA0 pin
	OC4CONbits.ON=0;
	KMZ80_PPS_OC4_SOUND;  // Use RB13 for OC4
	OC4CONbits.SIDL=1;    // Stop in idle mode
	OC4CONbits.OC32=0;    // Use 16-bits timer
	OC4CONbits.OCTSEL=1;  // Timer3 is the clock source
	OC4CONbits.OCM=3;     // Toggle mode
	OC4R=1;               // When TMR3=1
	OC4CONbits.ON=1;
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
						//KMZ80_LAT_CMT=1;
						//KMZ80_TRIS_CMT=0;
					} else {
						read825x[2]&=0xfd;
						//KMZ80_TRIS_CMT=1;
					}
					return;
				case 4: // PC2: LED 1:green, 2: red
					if (data&1) {
						read825x[2]|=0x04;
						KMZ80_LAT_GLED=0;
						KMZ80_LAT_RLED=1;
					} else {
						read825x[2]&=0xfb;
						KMZ80_LAT_GLED=1;
						KMZ80_LAT_RLED=0;
					}
					return;
				case 6: // PC3: CMT motor ON/OFF
					if (data&1) {
						read825x[2]|=0x08;
						read825x[2]^=0x10; // Change motor status (ON/OFF)
			            if (read825x[2]&0x10) {
			                // motor on
			                //timer3_cmt();
			            } else {
			                // motor off; let's go to sound mode of Timer3
			                //timer3_sound();
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