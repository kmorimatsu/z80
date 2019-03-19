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
						KMZ80_LAT_CMT=1;
						KMZ80_TRIS_CMT=0;
					} else {
						read825x[2]&=0xfd;
						KMZ80_TRIS_CMT=1;
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