#include "main.h"

// Local grobal variables
SearchRec g_sr_sd;
FSFILE* g_sd_file;

// Local macro
#define reset_g_timer1() \
	T1CONbits.ON=0;\
	g_timer1=g_TMR1h|TMR1;\
	T1CONbits.ON=1

void init_sd(){
	// Input and output pins for SDI1, SDO1, SCK1, and SDCS.
	KMZ80_TRIS_SDI1=1;
	KMZ80_ANSEL_SDI1=0;
	KMZ80_TRIS_SCK1=0;
	KMZ80_TRIS_SDO1=0;
	KMZ80_TRIS_SDCS=0;

	// Disable CS-card, first
	KMZ80_LAT_SDCS=1;

	// SDO1 and SDI1 pin selections
	KMZ80_PPS_SDO1;
	KMZ80_PPS_SDI1;
}


void filelist_sd(){
	int i,pos;
	// Clear screen
	for(i=0;i<1000;i++){
		VRAM2[i]=0;
	}
	for(i=0;i<9;i++){
		VRAM2[i+1]=ascii2char["[USE CMT]"[i]];
	}
	for(i=0;i<40;i++){
		VRAM2[i+960]=ascii2char["-----SELECT FILE AND HIT SPACE KEY.-----"[i]];
	}
	// Get file names
	pos=21;
	if (!FindFirst("*.MZT",ATTR_MASK,&g_sr_sd)) {
		do {
			for(i=0;g_sr_sd.filename[i];i++){
				VRAM2[pos+i]=ascii2char[g_sr_sd.filename[i]];
				//VRAM2[pos+i]=g_sr_sd.filename[i];
			}
			pos+=20;
		} while (pos<960 && !FindNext(&g_sr_sd));
	}
}

int key_sd(){
	int i;
	if (!(keymap[9]&0x02)) {
		// space key
		return 1;
	}
	if (!(keymap[8]&0x08)) {
		// right/left key
		if ((keymap[8]&0x21)==0x21) {
			// right
			return 2;
		} else {
			// left
			return 3;
		}
	}
	if (!(keymap[9]&0x04)) {
		// down/up key
		if ((keymap[8]&0x21)==0x21) {
			// down
			return 4;
		} else {
			// up
			return 5;
		}
	}
	return 0;
}

int fileselect_sd(){
	int pos;
	char b;
	pos=0;
	while(1) {
		// Wait until key will be released.
		while (key_sd()) {
			if (read825x[2]&0x40) {
				VRAM2[pos]=0xC3;
			} else {
				VRAM2[pos]=0x00;
			}
		}
		// Wait until key will be pressed.
		do {
			if (read825x[2]&0x40) {
				VRAM2[pos]=0xC3;
			} else {
				VRAM2[pos]=0x00;
			}
			b=key_sd();
		} while (b==0);
		// Clear cursor indicator.
		VRAM2[pos]=0x00;
		switch (b) {
			case 2: // right
				if (pos<940) {
					if (VRAM[pos+21]) pos+=20;
				}
				break;
			case 3:	// left
				if (20<=pos) {
					if (VRAM[pos-19]) pos-=20;
				}
				break;
			case 4: // down
				if (pos<920) {
					if (VRAM[pos+41]) pos+=40;
				}
				break;
			case 5: // up
				if (40<=pos) {
					if (VRAM[pos-39]) pos-=40;
				}
				break;
			case 0: // select (space key)
			default:
				return pos;
		}
	}
}

int openfile_sd(int pos){
	// The first one is not file from SD-card but from CMT.
	pos-=20;
	// Start finding file
	if (FindFirst("*.MZT",ATTR_MASK,&g_sr_sd)) {
		// Any MZT file not found.
		return 0;
	}
	while (0<pos) {
		if (FindNext(&g_sr_sd)) {
			// Reached the end of list.
			return 0;
		}
		pos-=20;
	}
	return 1;
}

char try_sdcard(unsigned short regPC){
	int i,len,pos;
	if (!FSInit()) {
		// Ignore the time spent for SD-card connection.
		reset_g_timer1();
		return 0;
	}
	switch(regPC){
		case 0x22: // Save header
			// Show file list
			VRAM=&VRAM2[0];
			filelist_sd();
			// Select file from list
			pos=fileselect_sd();
			VRAM=&VRAM1[0];
			if (!pos) break; // Use CMT
			// Determine file name from cursor position
			if (!openfile_sd(pos)) break;
			// Open file
			g_sd_file = FSfopen(g_sr_sd.filename,"w");
			if (!g_sd_file) break;
			// Write header information (128 bytes)
			len = FSfwrite((void *)&RAM[0x00f0],1,128,g_sd_file);
			FSfclose(g_sd_file);
			if (len!=128) break;
			reset_g_timer1();
			return 1;
			break;
		case 0x25: // Save body
			// Open file
			g_sd_file = FSfopen(g_sr_sd.filename,"w");
			if (!g_sd_file) break;
			// Write header information (128 bytes)
			len = FSfwrite((void *)&RAM[0x00f0],1,128,g_sd_file);
			if (len!=128) {
				FSfclose(g_sd_file);
				break;
			}
			// Determine size and address to read
			len=RAM[0x0102];
			len+=RAM[0x0103]<<8;
			pos=RAM[0x0104];
			pos+=RAM[0x0105]<<8;
			pos-=0x1000;
			while (0<len) {
				if (128<len) {
					i = FSfwrite((void *)&RAM[pos],1,128,g_sd_file);
					if (i!=128) {
						FSfclose(g_sd_file);
						break;
					}
					pos+=128;
					len-=128;
				} else {
					i = FSfwrite((void *)&RAM[pos],1,len,g_sd_file);
					FSfclose(g_sd_file);
					if (i!=len) break;
					reset_g_timer1();
					return 1;
				}
			}
			break;
		case 0x28: // Load header
			// Show file list
			VRAM=&VRAM2[0];
			filelist_sd();
			// Select file from list
			pos=fileselect_sd();
			VRAM=&VRAM1[0];
			if (!pos) break; // Use CMT
			// Determine file name from cursor position
			if (!openfile_sd(pos)) break;
			// Open file
			g_sd_file = FSfopen(g_sr_sd.filename,"r");
			if (!g_sd_file) break;
			// Read header information (128 bytes)
			len = FSfread((void *)&RAM[0x00f0],1,128,g_sd_file);
			FSfclose(g_sd_file);
			if (len!=128) break;
			// Store header information to RAM.
			reset_g_timer1();
			return 1;
		case 0x2B: // Load body
			// Open file
			g_sd_file = FSfopen(g_sr_sd.filename,"r");
			if (!g_sd_file) break;
			// Skip header information (128 bytes)
			if (FSfseek(g_sd_file,128,SEEK_SET)) {
				FSfclose(g_sd_file);
				break;
			}
			// Determine size and address to store
			len=RAM[0x0102];
			len+=RAM[0x0103]<<8;
			pos=RAM[0x0104];
			pos+=RAM[0x0105]<<8;
			pos-=0x1000;
			while (0<len) {
				if (128<len) {
					i = FSfread((void *)&RAM[pos],1,128,g_sd_file);
					if (i!=128) {
						FSfclose(g_sd_file);
						break;
					}
					pos+=128;
					len-=128;
				} else {
					i = FSfread((void *)&RAM[pos],1,len,g_sd_file);
					FSfclose(g_sd_file);
					if (i!=len) break;
					reset_g_timer1();
					return 1;
				}
			}
			break;
		case 0x2E: // Verify body
			break;
		default:
			break;
	}
	VRAM=&VRAM1[0];
	reset_g_timer1();
	return 0;
}
