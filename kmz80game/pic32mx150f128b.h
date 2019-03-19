/*
	PIC32MX150F128b specific settings
*/

#ifndef KMZ80_PROFILE_H
#define KMZ80_PROFILE_H

#define PROGRAM_FLASH_END_ADRESS 0x9D01F7FF

#define KMZ80_RAMSIZE 0x7000 // 28 KB RAM

// Config setting(s) specific to this chip
#ifdef KMZ80_MAIN
	#pragma config ICESEL = ICS_PGx1
#endif

// Interrupt vectors
#ifdef KMZ80_MAIN // Timer 1
	#pragma interrupt timer1Int IPL4SOFT vector 4
#endif
#ifdef KMZ80_NTSC // Timer 2 and Timer 4
	#pragma interrupt ntscSyncInt IPL7SOFT vector 8
	#pragma interrupt ntscVideoInt IPL6SOFT vector 16
#endif
#ifdef KMZ80_PS2 // INT1
	#pragma interrupt ps2Int IPL5SOFT vector 7
#endif

// green/red LEDs are not connected
#define KMZ80_TRIS_GLED g_dummy
#define KMZ80_TRIS_RLED g_dummy
#define KMZ80_LAT_GLED g_dummy
#define KMZ80_LAT_RLED g_dummy

// NTSC Hsync will be RB3
#define sync LATBbits.LATB3
#define toggleSync() (LATBINV=0x08)
#define KMZ80_TRIS_HSYNC TRISBbits.TRISB3
// Video signal will be RA1/RB1/RB2 (SDO2)
// TRISAbits.TRISA1 will be set in initNtsc()
#define KMZ80_TRIS_VIDEO TRISBbits.TRISB1=TRISBbits.TRISB2
#define KMZ80_PPS_SDO2 RPB1R=RPB2R=4

// PS2 keyboard; PORTB8: data, PORTB9: clock
#define ps2_clock_out TRISBbits.TRISB9
#define ps2_data_out  TRISBbits.TRISB8
#define ps2_clock_in  PORTBbits.RB9
#define ps2_data_in   PORTBbits.RB8
#define ps2_clock_lat LATBbits.LATB9
#define ps2_data_lat  LATBbits.LATB8
#define ps2_enable_tris TRISAbits.TRISA1
#define ps2_enable_out LATAbits.LATA1
// INT1 interrupt is set to PORTB9
#define KMZ80_PPS_INT1 INT1R=4


// REFCLKO not used
// Timers 3 and 5 use internal clock

// Sound settings using OC4 (RB13)
#define KMZ80_PPS_OC4_SOUND RPB13R=5
#define KMZ80_TRIS_SOUND TRISBbits.TRISB13

#endif