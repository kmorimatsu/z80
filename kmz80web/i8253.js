/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	i8253.read(addr);
	i8253.write(addr,data);
*/
i8253=new Object();
i8253.msb=0; // boolean
i8253.rwbuff=0;
i8253.timer0=0;
i8253.timer1=0;
i8253.timer2=0;
i8253.timer1time=0;
i8253.timer2time=0;
i8253.events=function(){
	// This events will be called every 1 msec
	this.timer1time-=31.25;
	while (1>this.timer1time && 0<this.timer1) {
		this.timer1time+=this.timer1;
		this.timer2time--;
		while (1>this.timer2time && 0<this.timer2) {
			this.timer2time+=this.timer2;
			z80.interrupt();
		}
	}
}
i8253.read=function(addr){
	var ret;
	addr&=0x0003;
	switch (addr) {
		case 0:
			if (this.msb) {
				this.msb=0;
				ret=this.timer0>>8;
				return ret;
			} else {
				this.msb=1;
				ret=this.timer0&0xff;
				return ret;
			}
		case 1:
			if (this.msb) {
				this.msb=0;
				ret=(this.timer1time&0xff00)>>8;
				return ret;
			} else {
				this.msb=1;
				ret=this.timer1time&0xff;
				return ret;
			}
		case 2:
			if (this.msb) {
				this.msb=0;
				ret=(this.timer2time&0xff00)>>8;
				return ret;
			} else {
				this.msb=1;
				ret=this.timer2time&0xff;
				return ret;
			}
		default:
			return 0xff;
	}
};
i8253.write=function(addr,data){
	addr&=0x0003;
	data&=0xff;
	switch (addr) {
		case 0: // Mode 2 (rate generator); used for sound.
			if (this.msb) {
				this.timer0=data<<8|this.rwbuff;
				this.msb=0;
				// Set frequencey of sound
				audio.set(1000000/(this.timer0+1));
			} else {
				this.rwbuff=data;
				this.msb=1;
			}
			return;
		case 1: // Mode 2 (rate generator); used for clock for timer2
			if (this.msb) {
				this.timer1=this.timer1time=data<<8|this.rwbuff;
				this.msb=0;
			} else {
				this.rwbuff=data;
				this.msb=1;
			}
			return;
		case 2: // Mode 0 (interrupt on terminal count); used for interrupt
			if (this.msb) {
				this.timer2=this.timer2time=data<<8|this.rwbuff;
				this.msb=0;
			} else {
				this.rwbuff=data;
				this.msb=1;
			}
			return;
		default:
			// Ignore configuration
			this.msb=0;
			return;
	}
};
