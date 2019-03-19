/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

/*	Environment(machine)-specific basic functions follow 
	These are specified to access to MZ-80K (with 28 KB RAM) memory map.
	I/O ports are connected to PORTB of PIC32MX150F128B.
	To emulate the other computer, these must be modified.
*/

#include "main.h"
#include "monitor.h"

// _getCode() macro (as inline function) that fetches a code using PC, and increment PC.
#define _getCode() ((z80PC<0x1000)\
	?(\
		monitor[(z80PC++)]\
	):(\
		(z80PC<(KMZ80_RAMSIZE+0x1000))\
		?(\
			RAM[(z80PC++)-0x1000]\
		):(\
			(z80PC<0xD000)\
			?(\
				0xC7\
			):(\
				(z80PC<0xE000)\
				?(\
					VRAM[(z80PC++)&0x03FF]\
				):(\
					0xC7\
				)\
			)\
		)\
	))

// readMemory() function uses 34 cycles max, 21 cycles typical
UINT8 readMemory(UINT16 addr){
	addr&=0xffff;
	if (addr<0x1000) return monitor[addr];
	if (addr<(KMZ80_RAMSIZE+0x1000)) return RAM[addr-0x1000];
	if (addr<0xD000) return 0xC7;
	if (addr<0xE000) return VRAM[addr & 0x03FF];
	if (addr<0xE004) {
		// 8255
		if (addr==0xE002) {
			// /VBLANK support
			read825x[2]&=0x5f;
			read825x[2]|=g_Vsync?0x80:0;
			// Actually, CMT reading is not required here.
			read825x[2]|=(16<ADC1BUF0)?0x20:0; 
		}
		return read825x[addr & 0x000F];
	}
	if (addr<0xE008) {
		// 8253
		return read8253(addr);
	}
	if (addr<0xE009) {
		// Tempo
		return read825x[8];
	}
	return 0xC7; // RST00
}

// writeMemory() function takes 44 cycles max, 31 cycles typical.
void writeMemory(UINT16 addr, UINT8 data){
	addr&=0xffff;
	data&=0xff;
	if (addr<0x1000) return; // ROM region cannot be written.
	if (addr<0xD000) {
		RAM[addr-0x1000] = data;
		return;
	}
	//if (addr<0xD000) return;
	if (addr<0xE000) {
		VRAM[addr & 0x03FF] = data;
		return;
	}
	if (addr<0xE004) {
		write8255(addr,data);
		return;
	}
	if (addr<0xE008) {
		write8253(addr,data);
		return;
	}
	if (addr<0xE009) {
		// on-off sound
		timer3_sound();
		T3CONbits.ON=(data&0x01);
		return;
	}
}
// readIO() function
UINT8 readIO(UINT8 addrL, UINT8 addrH){
	// Do nothing
	return 0x00;
}
// writeIO() function
void writeIO(UINT8 addrL, UINT8 addrH, UINT8 data){
	// Do nothing
}

// To support CMT read function, "AND 20" assmbly in monitor program will be overrided.
#define PRECODEE6 \
	if (regPC<0x1000) {\
		if (monitor[regPC]==0x20) {\
			loadA(readCMT(regPC));\
		}\
	}

// To support CMT write function, "PUSH AF" assembly in monitor program will be overrided.
// "RET" assembly code will be executed after reading from CMT.
#define PRECODEF5 \
	if (regPC==0x0768 || regPC==0x0781) {\
		writeCMT(regPC);\
		codeC9();\
		return;\
	}

// Used to truck C3 command for CMT load/save from/to SD-card.
void codeC9();
#define PRECODEC3 \
	if (0x22<=regPC && regPC<=0x2e) {\
		if (try_sdcard(regPC)) {\
			codeC9();\
			return;\
		}\
	}

/*	End of environment-specific basic functions */
