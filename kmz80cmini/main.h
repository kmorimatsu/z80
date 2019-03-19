/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include <xc.h>
#include <plib.h>
#include "sd/FSIO.h"

/* Global variables follow */
#ifdef __32MX350F256H__
	#include "./pic32mx350f256h.h"
#elif __32MX150F256H__
	#include "./pic32mx150f256h.h"
#elif __32MX250F256H__
	#include "./pic32mx250f256h.h"
#endif

#ifdef KMZ80_MAIN
	volatile char g_dummy;
	unsigned char RAM[KMZ80_RAMSIZE];
	volatile unsigned char VRAM1[0x0400];
	volatile unsigned char VRAM2[0x0400];
	volatile unsigned char* VRAM=&VRAM1[0];
	volatile unsigned char read825x[0x10];
	unsigned long g_ntscVisible;
	unsigned long g_Vsync;
	unsigned long g_timer1;
	volatile unsigned long g_TMR1h;
	volatile unsigned long g_Vgate;
	volatile unsigned int prompt_8255=0;
	volatile unsigned int tempo_8255=0;
	volatile unsigned char keymap[0x10];
	volatile unsigned long ps2data=0;
	volatile unsigned long ps2status=1;
	extern const unsigned char monitor[]; // Used as random seed
#else
	extern volatile char g_dummy;
	extern unsigned char RAM[KMZ80_RAMSIZE];
	extern volatile unsigned char VRAM1[0x0400];
	extern volatile unsigned char VRAM2[0x0400];
	extern volatile unsigned char* VRAM;
	extern volatile unsigned char read825x[0x10];
	extern unsigned long g_ntscVisible;
	extern unsigned long g_Vsync;
	extern unsigned long g_timer1;
	extern volatile unsigned long g_TMR1h;
	extern volatile unsigned long g_Vgate;
	extern volatile unsigned int prompt_8255;
	extern volatile unsigned int tempo_8255;
	extern volatile unsigned char keymap[0x10];
	extern volatile unsigned long ps2data;
	extern volatile unsigned long ps2status;
#endif

/* Prototyping */

void main();

void execZ80infinite();
void execZ80code();
void resetZ80();
void intZ80(unsigned char i8);
void nmrZ80();

void initNtsc();
void ntscVisible(unsigned long visible);
unsigned long ntscVsync();

void init8253();
void write8253(unsigned int addr, unsigned char data);
unsigned char read8253(unsigned int addr);
void inc8253timer2();

void init8255();
void write8255(unsigned int addr, unsigned char data);

void ps2_init();
void ps2_received(unsigned char data);

void loadTape();

void init_cmt();
void _timer3_sound();
void _timer3_cmt();
#define timer3_sound() { if (AD1CON1bits.ON)  _timer3_sound(); }
#define timer3_cmt()   { if (!AD1CON1bits.ON) _timer3_cmt(); }
unsigned char readCMT(unsigned short regPC);
void writeCMT(unsigned short regPC);

void init_sd();
char try_sdcard(unsigned short regPC);

const unsigned char ascii2char[256];

// Debug macro
#define showHex(pos,val)\
	VRAM[pos]=(((val)>>4)<10)?(0x20+((val)>>4)):(0x01-10+((val)>>4));\
	VRAM[(pos)+1]=(((val)&15)<10)?(0x20+((val)&15)):(0x01-10+((val)&15))

