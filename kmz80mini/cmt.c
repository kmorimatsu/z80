/*
   This program is provided under the LGPL license ver 2.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "main.h"

void init_cmt(){
	// Timer3 (of PIC) setting
	TMR3=0;
	T3CONbits.ON=0;     // 8253 timer 0 is stopped first.
	ANSELAbits.ANSA1=0; // Degital mode for RA1
	TRISAbits.TRISA1=1; // input from RA1
	T3CKR=0;            // input from RA1
	T3CONbits.TCKPS=0;  // 1:1 prescaler for 2 MHz
	T3CONbits.TCS=1;    // Use T3CK
	T3CONbits.SIDL=1;   // Stop in idle mode

	// OC1 setting for RA0 pin
	OC1CONbits.ON=0;
	RPB7R=0;            // Do not use RB7 for OC1
	RPA0R=5;            // Use RA0 for OC1
	TRISAbits.TRISA0=0; // Output for RA0
	OC1CONbits.SIDL=1;  // Stop in idle mode
	OC1CONbits.OC32=0;  // Use 16-bits timer
	OC1CONbits.OCTSEL=1;// Timer3 is the clock source
	OC1CONbits.OCM=3;   // Toggle mode
	OC1R=1;             // When timer3=1
	OC1CONbits.ON=1;

	// Prepare output for CMT out
	TRISBbits.TRISB7=0; // Output for RB7

	// Use AN5 of PB3 pin for A/D conversion
	// Note that A/D converter is disabled here (ON=0).
	TRISBbits.TRISB3=1;
	ANSELBbits.ANSB3=1;
	// A/D converter (for CMT reading) settings follow
	// User Timer 3 for detection signaling (SSRC=010)
	// ON=0, SIDL=1, FORM=000, SSRC=010
	// CLRASAM=0, ASAM=1, SAMP=0, DONE=0
	AD1CON1=0xa044;
	// VCFG=000, OFFCAL=0,
	// CSCNA=0, BUFS=0, SMPI=0000, NUFM=0,
	// ALTS=0
	AD1CON2=0x0000;
	// TAD=48/16=3 MHz
	// ADRC=0, SAMC=11111, ADCS=000000100
	// Sampling frequency: 94 KHz (enough for 1400 baud CMT)
	AD1CON3=0x1f04;
	// CH0NA=0, CH0SA=0101 (AN5)
	AD1CHS=0x00050000;
}
void _timer3_sound(){
	// Stop A/D converter
	AD1CON1bits.ON=0;
	// Timer3 (of PIC) setting
	TMR3=0;
	T3CONbits.ON=0;     // 8253 timer 0 is stopped.
	// OC1 setting for RA0 pin
	RPB7R=0;            // Do not use RB7 for OC1
	RPA0R=5;            // Use RA0 for OC1
	// Go back to original settings
	RAM[0x0035]=0x26;
	RAM[0x0036]=0x19;
}
void _timer3_cmt(){
	// Timer3 (of PIC) setting. See also timer3_sound().
	TMR3=0;
	PR3=699;            // Timer3 cycle will be 350 usec for reading.
	T3CONbits.ON=1;     // timer3 is always running.
	// Start A/D converter
	AD1CON1bits.ON=1;
	// OC1 setting for RB7 pin. See also timer3_sound().
	// TODO: revise here for CMT out
	RPA0R=0;            // Do not use RA0 for OC1
	RPB7R=5;            // Use RB7 for OC1
	// Set the waiting cyecle # to 1 
	// The values here are used to wait for 350 usec (etc) in monitor,
	// but is not used in KM-Z80 mini.
	// Instead, timer3 will be used to wait for 350 usec.
	RAM[0x0035]=0x01;
	RAM[0x0036]=0x01;
}

unsigned char readCMT(unsigned short regPC){
	// The process will be here when executing "AND 20" in monitor ROM
	// This corresponds to the reading of CMT data from I/O

	// Go to CMT mode
	timer3_cmt();

	// Reset A/D converter DONE bit
	AD1CON1bits.DONE=0;

	if (0x0601<=regPC && regPC<=0x0623) {
		// Read rising edge (otherwise, Read point)
		TMR3=698;
	}

	// Wait until A/D conversion will be done.
	while (!AD1CON1bits.DONE) {}
	
	return (10<ADC1BUF0) ? 0x20:0;
}

void writeCMT(unsigned short regPC){
	// Go to CMT mode
	timer3_cmt();
	if (regPC==0x0768) {
		// Wait until OC1 output will be high. 
		while (!IFS0bits.OC1IF);
		// Short palse
		// High for 240 usec then low for 240 usec 
		PR3=479;
		IFS0bits.OC1IF=0;
	} else {
		// Wait until OC1 output will be high. 
		while (!IFS0bits.OC1IF);
		// Long palse
		// High for 480 usec then low for 480 usec 
		PR3=959;
		IFS0bits.OC1IF=0;
	}
}