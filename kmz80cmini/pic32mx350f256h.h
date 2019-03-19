/*
	PIC32MX350F256H specific settings
*/

#ifndef KMZ80_PROFILE_H
#define KMZ80_PROFILE_H

#define KMZ80_RAMSIZE 0xC000 // 48 KB RAM

// Config setting(s) specific to this chip
#ifdef KMZ80_MAIN
	#pragma config FSRSSEL=PRIORITY_7
	#pragma config ICESEL = ICS_PGx1
#endif

// Interrupt vectors
#ifdef KMZ80_MAIN // Timer 1
	#pragma interrupt timer1Int IPL4SOFT vector 4
#endif
#ifdef KMZ80_PS2 // INT1
	#pragma interrupt ps2Int IPL5SOFT vector 7
#endif
#ifdef KMZ80_NTSC // Timer 2 and Timer 4
	#pragma interrupt ntscSyncInt IPL7SOFT vector 8
	#pragma interrupt ntscVideoInt IPL6SOFT vector 16
#endif

// RD3 to green LED, RD4 to red LED
#define KMZ80_TRIS_GLED TRISDbits.TRISD3
#define KMZ80_TRIS_RLED TRISDbits.TRISD4
#define KMZ80_LAT_GLED LATDbits.LATD3
#define KMZ80_LAT_RLED LATDbits.LATD4

// NTSC Hsync will be RG7
#define sync LATGbits.LATG7
#define toggleSync() (LATGINV=1<<7)
#define KMZ80_TRIS_HSYNC TRISGbits.TRISG7
// Video signal will be RG8 (SDO2)
#define KMZ80_TRIS_VIDEO TRISGbits.TRISG8
#define KMZ80_PPS_SDO2 RPG8R=6

// PS2 keyboard; PORTD10: data, PORTD8: clock
#define ps2_clock_out TRISDbits.TRISD8
#define ps2_data_out  TRISDbits.TRISD10
#define ps2_clock_in  PORTDbits.RD8
#define ps2_data_in   PORTDbits.RD10
#define ps2_clock_lat LATDbits.LATD8
#define ps2_data_lat  LATDbits.LATD10
// INT1 interrupt is set to PORTD8
#define KMZ80_PPS_INT1 INT1R=4

// Output REFCLKO to RPB15
#define KMZ80_PPS_REFCLKO RPB15R=3
// Timer 3 settings: T3CK from RPB14
#define KMZ80_PPS_TMR3CLK T3CKR=2
#define KMZ80_TRIS_TMR3CLK TRISBbits.TRISB14
#define KMZ80_ANSEL_TMR3CLK ANSELBbits.ANSB14
// Timer 5 settings: T5CK from RPF5
#define KMZ80_PPS_TMR5CLK T5CKR=2
#define KMZ80_TRIS_TMR5CLK TRISFbits.TRISF5
#define KMZ80_ANSEL_TMR5CLK g_dummy

// Sound/CMT settings (using OC1 and AN24)
#define KMZ80_PPS_OC1_CMT RPD1R=12
#define KMZ80_PPS_OC1_SOUND RPG9R=12
#define KMZ80_TRIS_CMT TRISDbits.TRISD1
#define KMZ80_LAT_CMT LATDbits.LATD1
#define KMZ80_TRIS_SOUND TRISGbits.TRISG9
#define KMZ80_ANSEL_CMT ANSELDbits.ANSD1
#define KMZ80_AN_CMT 0x00180000

// SPI1 setting for SD-card
#define KMZ80_PPS_SDO1 RPD11R=8
#define KMZ80_PPS_SDI1 SDI1R=14
#define KMZ80_TRIS_SCK1 TRISFbits.TRISF6
#define KMZ80_TRIS_SDO1 TRISDbits.TRISD11
#define KMZ80_TRIS_SDI1 TRISFbits.TRISF2
#define KMZ80_ANSEL_SDI1 g_dummy
#define KMZ80_TRIS_SDCS TRISFbits.TRISF3
#define KMZ80_LAT_SDCS LATFbits.LATF3

// Settings for Microchip library follow

#define SYS_FREQ (48000000L)

// Common macros 
// Clock frequency values
// These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
#define GetSystemClock()		SYS_FREQ			// Hz
#define GetInstructionClock()	(GetSystemClock()/1)	// 
#define GetPeripheralClock()	(GetSystemClock()/1)	// Divisor is dependent on the value of FPBDIV set(configuration bits).

// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI

//SPI Configuration
#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
#define SPI_START_CFG_2     (SPI_ENABLE)

// Define the SPI frequency
#define SPI_FREQUENCY			(10000000)

// Description: SD-SPI Chip Select Output bit
#define SD_CS               LATFbits.LATF3
// Description: SD-SPI Chip Select TRIS bit
#define SD_CS_TRIS          TRISFbits.TRISF3

// Description: SD-SPI Card Detect Input bit
#define SD_CD               (0)
// Description: SD-SPI Card Detect TRIS bit (not used; same for SDI)
#define SD_CD_TRIS          TRISFbits.TRISF2

// Description: SD-SPI Write Protect Check Input bit
#define SD_WE               (0)
// Description: SD-SPI Write Protect Check TRIS bit (not used; same for SDI)
#define SD_WE_TRIS          TRISFbits.TRISF2

// Description: The main SPI control register
#define SPICON1             SPI1CON
// Description: The SPI status register
#define SPISTAT             SPI1STAT
// Description: The SPI Buffer
#define SPIBUF              SPI1BUF
// Description: The receive buffer full bit in the SPI status register
#define SPISTAT_RBF         SPI1STATbits.SPIRBF
// Description: The bitwise define for the SPI control register (i.e. _____bits)
#define SPICON1bits         SPI1CONbits
// Description: The bitwise define for the SPI status register (i.e. _____bits)
#define SPISTATbits         SPI1STATbits
// Description: The enable bit for the SPI module
#define SPIENABLE           SPICON1bits.ON
// Description: The definition for the SPI baud rate generator register (PIC32)
#define SPIBRG			    SPI1BRG

// Description: The TRIS bit for the SCK pin
#define SPICLOCK            TRISFbits.TRISF6
// Description: The TRIS bit for the SDI pin
#define SPIIN               TRISFbits.TRISF2
// Description: The TRIS bit for the SDO pin
#define SPIOUT              TRISDbits.TRISD11

	//SPI library functions
#define putcSPI             putcSPI1
#define getcSPI             getcSPI1
#define OpenSPI(config1, config2)   OpenSPI1(config1, config2)

#endif