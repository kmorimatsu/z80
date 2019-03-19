/*-------------------------------------------------------------------------*/
/* PFF - Low level disk control module for PIC            (C)ChaN, 2010    */
/*-------------------------------------------------------------------------*/

#include "pff.h"
#include "diskio.h"

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#include <xc.h>

#define SELECT()	LATAbits.LATA0 = 0		/* CS = L */
#define	DESELECT()	LATAbits.LATA0 = 1		/* CS = H */
#define	MMC_SEL		!LATAbits.LATA0		/* CS status (true:CS == L) */
#define	FORWARD(d)	(d)		/* Data forwarding function (Console out in this example) */

static
void init_spi (void)		/* Initialize SPI port */
{
// TODO: PIC32MX specific settings.
	// SPI2 module settings follow
	TRISBbits.TRISB5=0; // SDO
	TRISAbits.TRISA0=0; // CS
	//ANSELAbits.ANSA4=0; // SDI
	TRISAbits.TRISA4=1; // SDI
	TRISBbits.TRISB15=0;// SCK
	SDI2R=2; //RPA4:SDI2
	RPB5R=4; //RPB5:SDO2
	// Do not use frame sync
	SPI2CONbits.FRMEN=0;
	SPI2CONbits.FRMSYNC=0;
	SPI2CONbits.FRMPOL=0;
	SPI2CONbits.MSSEN=0;
	SPI2CONbits.FRMSYPW=0;
	SPI2CONbits.FRMCNT=0;
	SPI2CONbits.SPIFE=0;
	// PBCLK is used
	SPI2CONbits.MCLKSEL=0;
	// Disable enhanced buffer enable (???)
	SPI2CONbits.ENHBUF=0;
	// Stop in idle mode
	SPI2CONbits.SIDL=1;
	// Do not disable SDO
	SPI2CONbits.DISSDO=0;
	// 8 bit mode
	SPI2CONbits.MODE32=0;
	SPI2CONbits.MODE16=0;
	// Input data sampled at end of data output time
	SPI2CONbits.SMP=1;
	// Serial output data changes on transition from Idle clock state to active clock state
	SPI2CONbits.CKE=0;
	// Not slave mode
	SPI2CONbits.SSEN=0;
	// Idle state for clock is a high level; active state is a low level.
	SPI2CONbits.CKP=1;
	// Master mode
	SPI2CONbits.MSTEN=1;
	// SDI pin is controlled by the SPI module
	SPI2CONbits.DISSDI=0;
	// Interrupt is generated when the last transfer is shifted out of SPISR and transmit operations are complete
	// Interrupt is generated when the last word in the receive buffer is read (i.e., buffer is empty)
	// But interrupt is not used.
	SPI2CONbits.STXISEL=0;
	SPI2CONbits.SRXISEL=0;
	SPI2CON2=0x0300;
	// SPI clock=REFCLK/(2x(SPI2BRG+1))=0.4 Mhz. (REFCLK=57.3 Mhz; SPI2BRG=71)
	SPI2BRG=71;
	// Enable SPI module
	SPI2CONbits.ON=1;
}

void dly_100us (void)		/* Delay 100 microseconds */
{
// TODO: PIC32MX specific settings.
	UINT n = 57300000 / 90000;
	do {
		asm volatile("nop");
	} while (--n);
}

static
void xmit_spi (BYTE d)		/* Send a byte to the MMC */
{
// TODO: PIC32MX specific settings.
	SPI2BUF = d;
	while (SPI2STATbits.SPIBUSY) ;
	SPI2BUF;
}

static
BYTE rcv_spi (void)			/* Send a 0xFF to the MMC and get the received byte */
{
// TODO: PIC32MX specific settings.
	SPI2BUF = 0xFF;
	while (SPI2STATbits.SPIBUSY) ;
	return (BYTE)SPI2BUF;
}

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_BLOCK			0x08	/* Block addressing */


static
BYTE CardType;


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* 1st byte (Start + Index) */
	DWORD arg		/* Argument (32 bits) */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	DESELECT();
	rcv_spi();
	SELECT();
	rcv_spi();

	/* Send a command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = rcv_spi();
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}




/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;

#if _USE_WRITE
	if (CardType && MMC_SEL) disk_writep(0, 0);	/* Finalize write process if it is in progress */
#endif
	init_spi();		/* Initialize ports to control MMC */
	DESELECT();
	for (n = 10; n; n--) rcv_spi();	/* 80 Dummy clocks with CS=H */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2 */
			for (n = 0; n < 4; n++) ocr[n] = rcv_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {			/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) dly_100us();	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcv_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 (HC or SC) */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 10000; tmr && send_cmd(cmd, 0); tmr--) dly_100us();	/* Wait for leaving idle state */
			if (!tmr || send_cmd(CMD16, 512) != 0)			/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	DESELECT();
	rcv_spi();

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
	DWORD lba,		/* Sector number (LBA) */
	WORD ofs,		/* Byte offset to read from (0..511) */
	WORD cnt		/* Number of bytes to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	BYTE rc;
	WORD bc;


	if (!(CardType & CT_BLOCK)) lba *= 512;		/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, lba) == 0) {		/* READ_SINGLE_BLOCK */

		bc = 40000;
		do {							/* Wait for data packet */
			rc = rcv_spi();
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {				/* A data packet arrived */
			bc = 514 - ofs - cnt;

			/* Skip leading bytes */
			if (ofs) {
				do rcv_spi(); while (--ofs);
			}

			/* Receive a part of the sector */
			if (buff) {	/* Store data to the memory */
				do {
					*buff++ = rcv_spi();
				} while (--cnt);
			} else {	/* Forward data to the outgoing stream (depends on the project) */
				do {
					FORWARD(rcv_spi());
				} while (--cnt);
			}

			/* Skip trailing bytes and CRC */
			do rcv_spi(); while (--bc);

			res = RES_OK;
		}
	}

	DESELECT();
	rcv_spi();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_writep (
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sa			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT res;
	WORD bc;
	static WORD wc;

	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = (WORD)sa;
		while (bc && wc) {		/* Send data bytes to the card */
			xmit_spi(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sa) {	/* Initiate sector write process */
			if (!(CardType & CT_BLOCK)) sa *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sa) == 0) {			/* WRITE_SINGLE_BLOCK */
				xmit_spi(0xFF); xmit_spi(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) xmit_spi(0);	/* Fill left bytes and CRC with zeros */
			if ((rcv_spi() & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; rcv_spi() != 0xFF && bc; bc--) dly_100us();	/* Wait ready */
				if (bc) res = RES_OK;
			}
			DESELECT();
			rcv_spi();
		}
	}

	return res;
}
#endif
