/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

/*
Example of *.TXT file:

[I]0072
[U]22
[D]62
[L]51
[R]52
[S]7084
[F]60

"[I]" defines initial key press sequence, if required.
This must start from 00 (as dummy), followed by keys sequence.

"[U]", "[D]", "[L]", "[R]", "[S]", "[F]" defines the reactions when
UP, DOWN, LEFT, RIGHT, START, and FIRE are set, respectively.
It defines the key when the button pressed, followed by keys sequence
after releasing the button.

Each key info is consists of two numbers. First number must be between
0 and 9, and the second one must be between 0-7. These corresponds
to 8255 port A output and port B input.

"[K]" defines use or not use PS/2 keybord. To use, set "11". 
Not to use, set "00". Default: not to use. Do not use "[K]" and "[L]", 
or "[K]" and "[R]", simultaneously.

*/

#include "main.h"

#define KEY_DOWN  (1<<7)
#define KEY_LEFT  (1<<8)
#define KEY_RIGHT (1<<9)
#define KEY_UP    (1<<10)
#define KEY_START (1<<11)
#define KEY_FIRE  (1<<14)
#define KEYS_MASK (KEY_DOWN|KEY_LEFT|KEY_RIGHT|KEY_UP|KEY_START|KEY_FIRE)

static char g_init_keys[33]="";
static char g_up_keys[8]="";
static char g_down_keys[8]="";
static char g_left_keys[8]="";
static char g_right_keys[8]="";
static char g_start_keys[8]="";
static char g_fire_keys[8]="";

static char g_init_button=0xa0; // dummy (0xa0 does not work as key-press signal)
static char g_up_button=0xa0;
static char g_down_button=0xa0;
static char g_left_button=0xa0;
static char g_right_button=0xa0;
static char g_start_button=0xa0;
static char g_fire_button=0xa0;


void keys_set(char* str){
	int i;
	char b,b2;
	char* keys;
	char* button;
	b=b2=1;
	i=0;
	while(b!=0 && b2!=0) {
		// Seek "[X]"
		b=str[i++];
		if (b!='[') continue;
		b=str[i++];
		b2=str[i++];
		if (b2!=']') continue;
		// Set pointers for which result will be saved.
		switch (b) {
			case 'i': case 'I':
				keys=&g_init_keys[0];
				button=&g_init_button;
				break;
			case 'u': case 'U':
				keys=&g_up_keys[0];
				button=&g_up_button;
				break;
			case 'd': case 'D':
				keys=&g_down_keys[0];
				button=&g_down_button;
				break;
			case 'l': case 'L':
				keys=&g_left_keys[0];
				button=&g_left_button;
				break;
			case 'r': case 'R':
				keys=&g_right_keys[0];
				button=&g_right_button;
				break;
			case 's': case 'S':
				keys=&g_start_keys[0];
				button=&g_start_button;
				break;
			case 'f': case 'F':
				keys=&g_fire_keys[0];
				button=&g_fire_button;
				break;
			case 'k': case 'K':
				// Use or not use ps/2 keyboard.
				b=str[i];
				if (b<'0' || '9'<b) continue;
				useps2=(b!='0') ? 1:0;
				continue;
			default:
				continue;
		}
		// Set button
		b=str[i];
		if (b<'0' || '9'<b) continue;
		b2=str[i+1];
		if (b2<'0' || '7'<b2) continue;
		i+=2;
		button[0]=((b<<4)&0xf0) | (b2&0x07) | 0x08;
		// Set keys after releasing button
		b=str[i];
		b2=str[i+1];
		while ('0'<=b && b<='9' && '0'<=b2 && b2<='7') {
			i+=2;
			keys[0]=((b<<4)&0xf0) | (b2&0x07) | 0x08;
			keys++;
			keys[0]=0;
			b=str[i];
			b2=str[i+1];
		}
	}
}

void keys_init(){
	// Input
	TRISBSET=KEYS_MASK;
	// Digital
	ANSELBCLR=KEYS_MASK;
	// Pull-up
	CNPUBSET=KEYS_MASK;
}

static char* g_keys=&g_init_keys[0];

void keys_check(){
	short keys;
	int i;
	char b;
	static int counter=1;
	// First, clear all key press frags.
	for (i=0;i<10;i++) {
		keymap[i]=0xff;
	}
	// Initial keys or keys after button up.
	if (counter) {
		counter++;
		i=counter>>10;
		b=g_keys[i];
		if (b==0) {
			// End of string. Let's stop this.
			counter=0;
		} else if (counter&0x200) {
			// Upper 4 bits are 0-9 position (8255 port A)
			// Lower 4 bits are D0-D7 position (8255 port B).
			keymap[0x0f&(b>>4)]&=~(1<<(b&0x07));
		}
	}
	keys=PORTB;
	if (!(keys & KEY_LEFT)) {
		keymap[0x0f&(g_left_button>>4)]&=~(1<<(g_left_button&0x07));
		counter=1;
		g_keys=&g_left_keys[0];
	}
	if (!(keys & KEY_RIGHT)) {
		keymap[0x0f&(g_right_button>>4)]&=~(1<<(g_right_button&0x07));
		counter=1;
		g_keys=&g_right_keys[0];
	}
	if (!(keys & KEY_UP)) {
		keymap[0x0f&(g_up_button>>4)]&=~(1<<(g_up_button&0x07));
		counter=1;
		g_keys=&g_up_keys[0];
	}
	if (!(keys & KEY_DOWN)) {
		keymap[0x0f&(g_down_button>>4)]&=~(1<<(g_down_button&0x07));
		counter=1;
		g_keys=&g_down_keys[0];
	}
	if (!(keys & KEY_FIRE)) {
		keymap[0x0f&(g_fire_button>>4)]&=~(1<<(g_fire_button&0x07));
		counter=1;
		g_keys=&g_fire_keys[0];
	}
	if (!(keys & KEY_START)) {
		keymap[0x0f&(g_start_button>>4)]&=~(1<<(g_start_button&0x07));
		counter=1;
		g_keys=&g_start_keys[0];
	}
}


