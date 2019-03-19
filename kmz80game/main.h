/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include <xc.h>

/* Global variables follow */
#ifdef __32MX350F256H__
	#include "./pic32mx350f256h.h"
#elif __32MX150F128B__
	#include "./pic32mx150f128b.h"
#elif __32MX250F128B__
	#include "./pic32mx250f128b.h"
#endif

#ifdef KMZ80_MAIN
	volatile char g_dummy;
	unsigned char RAM[KMZ80_RAMSIZE];
	volatile unsigned char VRAM[0x0400];
	volatile unsigned char read825x[0x10];
	char g_use_ram_last20k=0;
	unsigned long g_ntscVisible;
	unsigned long g_Vsync;
	unsigned long g_timer1;
	char g_8253_timer1_carry=0;
	unsigned short g_TMR5;
	unsigned short g_PR5;
	volatile unsigned long g_TMR1h;
	volatile unsigned long g_Vgate=1;
	volatile unsigned int prompt_8255=0;
	volatile unsigned int tempo_8255=0;
	volatile unsigned char keymap[0x10];
	volatile unsigned char useps2=0;
	volatile unsigned long ps2data=0;
	volatile unsigned long ps2status=1;
	volatile unsigned short g_break_point=0x0001;
	extern const unsigned char monitor[]; // Used as random seed
#else
	extern volatile char g_dummy;
	extern unsigned char RAM[KMZ80_RAMSIZE];
	extern volatile unsigned char VRAM[0x0400];
	extern volatile unsigned char read825x[0x10];
	extern char g_use_ram_last20k;
	extern unsigned long g_ntscVisible;
	extern unsigned long g_Vsync;
	extern unsigned long g_timer1;
	extern char g_8253_timer1_carry;
	extern unsigned short g_TMR5;
	extern unsigned short g_PR5;
 	extern volatile unsigned long g_TMR1h;
	extern volatile unsigned long g_Vgate;
	extern volatile unsigned int prompt_8255;
	extern volatile unsigned int tempo_8255;
	extern volatile unsigned char keymap[0x10];
	extern volatile unsigned char useps2;
	extern volatile unsigned long ps2data;
	extern volatile unsigned long ps2status;
	extern volatile unsigned short g_break_point;
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

void sd_loadMZF();
void sd_error();
char try_sdcard(unsigned short regPC);

void keys_set(char* str);
void keys_init();
void keys_check();

const unsigned char ascii2char[256];

// Debug macro
#define showHex8(pos,val)\
	VRAM[pos]=(((val)&15)<10)?(0x20+((val)&15)):(0x01-10+((val)&15))
#define showHex16(pos,val)\
	VRAM[pos]=(((val)>>4)<10)?(0x20+((val)>>4)):(0x01-10+((val)>>4));\
	VRAM[(pos)+1]=(((val)&15)<10)?(0x20+((val)&15)):(0x01-10+((val)&15))
