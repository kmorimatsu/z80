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
#define _getCode() (\
	(regPC<0x0300) ? ROM[regPC++] :(\
	(0x7FFF<regPC && regPC<0x9800) ? RAM[(regPC++)&0x1FFF] : 0xFF\
	))
//#define getCode() _getCode()

UINT8 getCode(){
	// Using getCode() as inline function for all will use much memory to exceed the maximum.
	return _getCode();
}
//*/

unsigned char readMemory(UINT16 addr){
	addr&=0xffff;
	if (addr<0x0300) {
		return ROM[addr];
	} else if (addr<0x8000) {
		return 0xFF; // RST 38
	} else if (addr<0x9800) {
		return RAM[addr&0x1FFF];
	} else {
		return 0xFF; // RST 38
	}
}

void writeMemory(UINT16 addr, UINT8 data){
	addr&=0xffff;
	data&=0xff;
	if (addr<0x8000) {
		return;
	} else if (addr<0x9800) {
		RAM[addr&0x1FFF]=data;
	} else {
		return;
	}
}

// Global 8255 values
UINT8 g_porta, g_portb, g_portc;

// readIO() function
unsigned char readIO(UINT8 addrL, UINT8 addrH){
	// Key input support (Port A)
	switch (addrL) {
		case 0xF8:
			return g_porta;
		case 0xF9:
			return g_portb;
		case 0xFA:
			return g_portc;
		default:
			return 0xFF;
	}
}
// writeIO() function
void writeIO(UINT8 addrL, UINT8 addrH, UINT8 data){
	// Key input support
	switch (addrL) {
		case 0x9e: // Event handler 
			switch (data) {
				case 0x00: // STORE DATA
					return;
				case 0x01: // LOAD DATA
					return;
				default:
					return;
			}
		case 0xfa: // Set row in key matrix (TK-80)
			g_portc=data;
			g_porta=0xff;
			if (!(data&0x10)) g_porta&=g_keymatrix[0];
			if (!(data&0x20)) g_porta&=g_keymatrix[1];
			if (!(data&0x40)) g_porta&=g_keymatrix[2];
			return;
		case 0xfb: // 8255 control (TK-80)
			if (data&0x80) return; // Ignore mode change.
			data&=0x0f; // Bit change in Port C follows.
			g_portc&=(1<<(data>>1))^0xff;
			g_portc|=(data&0x01)<<(data>>1);
			data=g_portc;
			g_porta=0xff;
			if (!(data&0x10)) g_porta&=g_keymatrix[0];
			if (!(data&0x20)) g_porta&=g_keymatrix[1];
			if (!(data&0x40)) g_porta&=g_keymatrix[2];
			return;
		default:
			return;
	}
}

void triggerInt(){
	if (0x8000<=z80PC) intZ80(0xFF);
}

/*	End of environment-specific basic functions */
