/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include <p32xxxx.h>
#include <plib.h>

//#define TIMER1LEN 13
#define TIMER1LEN 13
#define TIMR1HSTEP (1<<TIMER1LEN)

/* Global variables follow */

#ifdef MAIN
	unsigned char RAM[0x1800] __attribute__((persistent)); // 6KB RAM
	volatile unsigned long g_timer1;
	volatile unsigned long g_TMR1h;
	volatile unsigned char g_keymatrix[3];
#else
	extern unsigned char RAM[0x1800];
	extern volatile unsigned long g_timer1;
	extern volatile unsigned long g_TMR1h;
	extern volatile unsigned char g_keymatrix[3];
#endif

/* Prototyping */

void main();

void init_led();
void timer1Int();

void triggerInt();

void execZ80infinite();
void execZ80code();
void resetZ80();
void intZ80(unsigned char i8);
void nmrZ80();

