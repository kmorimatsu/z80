/*******************************
* ZK-80 web written by Katsumi *
*   This script is released    *
*     under the LGPL v2.1.     *
*******************************/

/*
	Public methods:
	io.read(addrL,addrH);
	io.write(addrL,addrH,data);
*/
io=new Object();
io.keymatrix=[0xff,0xff,0xff];
io.keydata=0xff;
io.loaded=0;
io.portc=0xff;
io.read=function(addrL,addrH){
	var ret;
	switch(addrL&0xff){
		case 0xf8: // Read from key matrix (TK-80)
		case 0x9c: // Read from key matrix (ND80ZIII)
			return this.keydata;
		case 0x9e: // Value of E register to wait for 4.5 msec
			clocks=speed*0.0045;
			clocks-=68;
			ret=parseInt(clocks/3598)+1;
			return ret; // =0x03 when 2.048 MHz.
		case 0x9f: // Value of A register to wait for 4.5 msec
			clocks=speed*0.0045;
			clocks-=68;
			clocks-=3598*parseInt(clocks/3598);
			ret=parseInt(clocks/14);
			if (ret==0) ret=1;
			if (255<ret) ret=0;
			return ret; // =0x8b when 2.048 MHz
		default:
			return 0xff;
	}
};
io.write=function(addrL,addrH,data){
	data&=0xff;
	switch(addrL&0xff){
		case 0x98: // LED on/off (ND80ZIII)
			display.control(data&0x10);
			return;
		case 0x9c: // Set row in key matrix (ND80ZIII)
			this.keydata=0xff;
			if (!(data&0x01)) this.keydata&=io.keymatrix[0];
			if (!(data&0x02)) this.keydata&=io.keymatrix[1];
			if (!(data&0x04)) this.keydata&=io.keymatrix[2];
			return;
		case 0x9e: // Event handler 
			switch (data) {
				case 0x00: // STORE DATA
					file.store();
					return;
				case 0x01: // LOAD DATA
					file.load();
					return;
				default:
					return;
			}
		case 0xfa: // Set row in key matrix (TK-80)
			this.portc=data;
			this.keydata=0xff;
			if (!(data&0x10)) this.keydata&=io.keymatrix[0];
			if (!(data&0x20)) this.keydata&=io.keymatrix[1];
			if (!(data&0x40)) this.keydata&=io.keymatrix[2];
			display.control(data&0x80);
			return;
		case 0xfb: // 8255 control (TK-80)
			if (data&0x80) return; // Ignore mode change.
			data&=0x0f; // Bit change in Port C follows.
			this.portc&=(1<<(data>>1))^0xff;
			this.portc|=(data&0x01)<<(data>>1);
			data=this.portc;
			this.keydata=0xff;
			if (!(data&0x10)) this.keydata&=io.keymatrix[0];
			if (!(data&0x20)) this.keydata&=io.keymatrix[1];
			if (!(data&0x40)) this.keydata&=io.keymatrix[2];
			display.control(data&0x80);
			return;
		default:
			return;
	}
};
io.mousedown=function(x,y){
	display.mousedown(x,y);
	switch(y){
		case 0:
			switch(x){
				case 0:  // RET
					this.keymatrix[2]=0xfd; return;
				case 1:  // RUN
					this.keymatrix[2]=0xfe; return;
				case 2:  // STORE DATA
					this.keymatrix[2]=0xbf; return;
				case 3:  // LOAD DATA
					this.keymatrix[2]=0x7f; return;
				default: // RESET
					return;
			}
		case 1:
			switch(x){
				case 0:  // C
					this.keymatrix[1]=0xef; return;
				case 1:  // D
					this.keymatrix[1]=0xdf; return;
				case 2:  // E
					this.keymatrix[1]=0xbf; return;
				case 3:  // F
					this.keymatrix[1]=0x7f; return;
				default: // ADRS SET
					this.keymatrix[2]=0xfb; return;
			}
		case 2:
			switch(x){
				case 0:  // 8
					this.keymatrix[1]=0xfe; return;
				case 1:  // 9
					this.keymatrix[1]=0xfd; return;
				case 2:  // A
					this.keymatrix[1]=0xfb; return;
				case 3:  // B
					this.keymatrix[1]=0xf7; return;
				default: // READ INCR
				this.keymatrix[2]=0xef; return;
			}
		case 3:
			switch(x){
				case 0:  // 4
					this.keymatrix[0]=0xef; return;
				case 1:  // 5
					this.keymatrix[0]=0xdf; return;
				case 2:  // 6
					this.keymatrix[0]=0xbf; return;
				case 3:  // 7
					this.keymatrix[0]=0x7f; return;
				default: // READ DECR
					this.keymatrix[2]=0xf7; return;
			}
		default:
			switch(x){
				case 0:  // 0
					this.keymatrix[0]=0xfe; return;
				case 1:  // 1
					this.keymatrix[0]=0xfd; return;
				case 2:  // 2
					this.keymatrix[0]=0xfb; return;
				case 3:  // 3
					this.keymatrix[0]=0xf7; return;
				default: // WRITE INCR
					this.keymatrix[2]=0xdf; return;
			}
	}
};
io.mouseup=function(x,y){
	display.mouseup();
	this.keymatrix=[0xff,0xff,0xff];
	if (y==0 && x==4) cpu.reset();
};
io.keydownnow=false;
io.keyup=function(code){
	this.keydownnow=false;
	code=this.keytable(code);
	if (code) this.mouseup(code[0],code[1]);
};
io.keydown=function(code){
	if (this.keydownnow) return;// Avoid key repeat.
	this.keydownnow=true;
	switch(code){
		case 0x54: //T
		case 0x09: //tab
			dom.getElement("tgswitch").click(); // Toggle switch (STEP/AUTO)
			return;
		case 0x50: //P
			dom.getElement("tgswitch2").click(); // Toggle switch (PROTECT/ENABLE)
			return;
		default:
			code=this.keytable(code);
			if (code) this.mousedown(code[0],code[1]);
			return;
	}
};
io.keytable=function(code){
	switch(code){
		case 0x58: //X
			return [4,1];// ADRS SET
		case 0x20: //space
		case 0x28: //down
		case 0x27: //right
		case 0x6b: //+(numeric keypad)
			return [4,2];// READ INCR
		case 0x4d: //M
		case 0x26: //up
		case 0x25: //left
		case 0x6d: //-(numeric keypad)
			return [4,3];// READ DECR
		case 0x1b: //esc
			return [4,0];// RESET
		case 0x0d: //cr
			return [4,4];// WRITE INCR
		case 0x47: //G
			return [1,0];// RUN
		case 0x52: //R
			return [0,0];// RET
		case 0x53: //S
			return [2,0];// STORE DATA
		case 0x4C: //L
			return [3,0];// LOAD DATA
		case 0x30: case 0x31: case 0x32: case 0x33:
		case 0x60: case 0x61: case 0x62: case 0x63:
			// 0-3
			return [code&0x03,4];// 0-3
		case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x64: case 0x65: case 0x66: case 0x67:
			return [code&0x03,3];// 4-7
		case 0x38: case 0x39:
		case 0x68: case 0x69:
			return [code&0x03,2];// 8-9
		case 0x41: case 0x42:
			return [code-0x41+2,2];// A-B
		case 0x43: case 0x44: case 0x45: case 0x46:
			return [code-0x43,1];// C-F
		default:
			return false;
	}
};
