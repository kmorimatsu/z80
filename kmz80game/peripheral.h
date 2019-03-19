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
				g_use_ram_last20k?RAM[(z80PC++)-0x6000]:0xC7\
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
UINT8 getCode(){
	// Using getCode() as inline function for all will use much memory to exceed the maximum.
	return _getCode();
}

// readMemory() function uses 34 cycles max, 21 cycles typical
unsigned char readMemory(UINT16 addr){
	addr&=0xffff;
	if (addr<0x1000) return monitor[addr];
	if (addr<(KMZ80_RAMSIZE+0x1000)) return RAM[addr-0x1000];
	if (addr<0xD000) return g_use_ram_last20k?RAM[addr-0x6000]:0xC7;
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
	if (addr<(KMZ80_RAMSIZE+0x1000)) {
		RAM[addr-0x1000] = data;
		return;
	}
	if (addr<0xD000) {
		if (g_use_ram_last20k) RAM[addr-0x6000] = data;
		return;
	}
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
		//timer3_sound();
		T3CONbits.ON=(data&0x01);
		return;
	}
}
// readIO() function
unsigned char readIO(UINT8 addrL, UINT8 addrH){
	// Do nothing
	return 0x00;
}
// writeIO() function
void writeIO(UINT8 addrL, UINT8 addrH, UINT8 data){
	// Do nothing
}

// Modify C3 function of Z80 (jump)
// Track MZ-80K API of 0x0003 (GETL)
// Once this API is called, jump to start address of loaded program.
// 0x22, 0x25, ... 0x2E are for LOAD/SAVE functions.
// Call the SD card function, and execute RET (0xC9) of Z80. 
void codeC9();
#define PRECODEC3 \
	static char start=1;\
	switch (regPC){ \
		case 0x0004: \
			if (!start) break; \
			start=0; \
			i8=readMemory(0x1106); \
			i16=readMemory(0x1107)<<8; \
			i16|=i8; \
			loadPC(i16); \
			return; \
		case 0x0022: case 0x0025: case 0x0028: case 0x002B: case 0x002E: \
			if (try_sdcard(regPC)) { \
				codeC9();\
				return; \
			} \
			break; \
		default: \
			break; \
	}

/*	End of environment-specific basic functions */
