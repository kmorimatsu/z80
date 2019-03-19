/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "main.h"
#include "diskio.h"
#include "pff.h"

#define FILENAME_FLASH_ADDRESS (0x9D005800)

static char* g_error_str=0;


BYTE sd_connect(){
	BYTE res;
	static FATFS warea;
	// Connect to SD card
	res=disk_initialize();
	if (res) {
		g_error_str="DISK INIT";
		return res;
	}
	// Connect to FAT file system
	res=pf_mount(&warea);
	if (res) {
		g_error_str="DISK MOUNT";
		return res;
	}
	return 0;
}

void sd_loadMZF(){
	BYTE res;
	static WORD len;
	int start,size,i;
	char* appname;
	char fname[15];
	// Determine application names
	appname=(char*)FILENAME_FLASH_ADDRESS;
	/*if (appname[0]<0x30 || 0x7A<appname[0]) {
		appname="BUGFIRE";
	}//*/
	for (i=0;i<8;i++) {
		if (!appname[i]) break;
		fname[i]=appname[i];
	}
	fname[i]='.';
	// Connect to SD card
	res=sd_connect();
	if (res) return;
	// Create TXT file name
	for (i=0;i<9;i++) {
		if (fname[i]=='.') break;
	}
	fname[i+1]='T';
	fname[i+2]='X';
	fname[i+3]='T';
	fname[i+4]=0;
	// Load keymap script
	res=pf_open(fname);
	if (res) {
		g_error_str="OPEN TXT";
		return;
	}
	// Load script (512 bytes max).
	res=pf_read(&RAM[0x0000],512,&len);
	if (res) {
		g_error_str="READ TXT";
		return;
	}
	// Insert 0x00 at the end.
	RAM[len]=0x00;
	// Set keys settings
	keys_set(RAM);
	// Create MZF file name
	for (i=0;i<9;i++) {
		if (fname[i]=='.') break;
	}
	fname[i+1]='M';
	fname[i+2]='Z';
	fname[i+3]='F';
	fname[i+4]=0;
	// Load MZF
	res=pf_open(fname);
	if (res) {
		g_error_str="OPEN MZF";
		return;
	}
	// Load header.
	res=pf_read(&RAM[0x00F0],128,&len);
	if (res || len<128) {
		g_error_str="READ MZF";
		return;
	}
	size=RAM[0x0103];
	size<<=8;
	size|=RAM[0x0102];
	start=RAM[0x0105];
	start<<=8;
	start|=RAM[0x0104];
	// Check format
	if (start<0x1200 || 0x6DFF<size) {
		g_error_str="FORMAT MZF";
		return;
	}
	if (0x7FFF<start || 0x7FFF<start+size) {
		// If program size is less than 20 KB, locate such program
		// in any RAM address area between 0x1200 and 0xCFFF.
		// Share the same physical RAM area as follows:
		//    0x3000 - 0x7FFF (20 kb)
		//    0x8000 - 0xCFFF (20 kb)
		if (size<=0x5000 && start+size<=0xD000) {
			g_use_ram_last20k=1;
		} else {
			g_error_str="FORMAT MZF";
			return;
		}
	}//*/
	// Load body
	start-=0x1000;
	while (128<=size) {
		while(0x7000<=start) {
			start-=0x5000;
		}
		res=pf_read(&RAM[start],128,&len);
		if (res || len<128) {
			g_error_str="LOAD MZF";
			return;
		}
		size-=128;
		start+=128;
	}
	if (0<size) {
		while(0x7000<=start) {
			start-=0x5000;
		}
		res=pf_read(&RAM[start],size,&len);
		if (res || len<size) {
			g_error_str="LOAD MZF";
			return;
		}
	}
}

void sd_error(){
	int i;
	char b;
	if (!g_error_str) return;
	// Clear screen
	for (i=0;i<1000;i++) {
		VRAM[i]=0;
	}
	// Copy strings to VRAM
	for (i=0;b="ERROR;"[i];i++) {
		VRAM[40*11+i]=ascii2char[b];
	}
	for (i=0;b=g_error_str[i];i++) {
		VRAM[40*12+i]=ascii2char[b];
	}
	// Infinite loop.
	while(1){
		asm volatile("nop");
	}
}

char try_sdcard_main(unsigned short regPC){
	BYTE res;
	static WORD len;
	char* appname;
	char fname[15];
	int start,size,i;
	// File selection
	switch(regPC){
		case 0x22: // Save header
		case 0x25: // Save body
			break;
		case 0x28: // Load header
		case 0x2B: // Load body
			// Determine application names
			appname=(char*)FILENAME_FLASH_ADDRESS;
			for (i=0;i<8;i++) {
				if (appname[i]=='.' || !appname[i]) break;
				fname[i]=appname[i];
			}
			// Create MZ2 file name
			fname[i]='.';
			fname[i+1]='M';
			fname[i+2]='Z';
			fname[i+3]='2';
			fname[i+4]=0;
			// Connect to SD card
			res=sd_connect();
			if (res) return 0;
			// Open file
			res=pf_open(fname);
			if (res) {
				g_error_str="OPEN MZ2";
				return 0;
			}
			break;
		case 0x2E: // Verify body
			break;
		default:
			return 0;
	}
	// Read from file
	switch(regPC){
		case 0x22: // Save header
			return 0;
		case 0x25: // Save body
			return 0;
		case 0x28: // Load header
			res=pf_read(&RAM[0x00F0],128,&len);
			if (res || len<128) {
				g_error_str="READ MZ2";
				return 0;
			}
			return 1;
		case 0x2B: // Load body
			// Fetch information
			size=RAM[0x0103];
			size<<=8;
			size|=RAM[0x0102];
			start=RAM[0x0105];
			start<<=8;
			start|=RAM[0x0104];
			// Check format
			if (start<0x1000 || 0x7FFF<start || 0x7FFF<start+size) {
				g_error_str="FORMAT MZ2";
				return 0;
			}
			// Skip first 128 bytes
			res=pf_lseek (128);
			if (res) {
				g_error_str="SEEK MZ2";
				return 0;
			}
			// Load body
			while (128<=size) {
				res=pf_read(&RAM[start-0x1000],128,&len);
				if (res || len<128) {
					g_error_str="LOAD MZ2";
					return 0;
				}
				size-=128;
				start+=128;
			}
			if (0<size) {
				res=pf_read(&RAM[start-0x1000],size,&len);
				if (res || len<size) {
					g_error_str="LOAD MZ2";
					return 0;
				}
			}
			return 1;
		case 0x2E: // Verify body
			return 0;
		default:
			return 0;
	}
}

char try_sdcard(unsigned short regPC){
	// Store SPI2 setting
	int SPI2CON_ntsc=SPI2CON;
	int SPI2CON2_ntsc=SPI2CON2;
	int SPI2BRG_ntsc=SPI2BRG;
	// Don't output to SDO2 pin
	int RPA1R_ntsc=RPA1R;
	RPA1R=0;
	int RPB6R_ntsc=RPB6R;
	RPB6R=0;
	KMZ80_PPS_SDO2&0;
	// Stop timer 4 interrupt
	IEC0bits.T4IE=0;
	// Call main function
	BYTE res=try_sdcard_main(regPC);
	// Restore SPI2 setting
	SPI2CON=SPI2CON_ntsc;
	SPI2CON2=SPI2CON2_ntsc;
	SPI2BRG=SPI2BRG_ntsc;
	// Output to SDO2 pin
	RPA1R=RPA1R_ntsc;
	RPB6R=RPB6R_ntsc;
	KMZ80_PPS_SDO2;
	// Start timer 4 interrupt
	IEC0bits.T4IE=1;
	sd_error();
	return res;
}