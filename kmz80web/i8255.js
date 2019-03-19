/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	i8255.read(addr);
	i8255.write(addr,data);
	i8255.keyup(key);
	i8255.keydown(key);
*/
i8255=new Object();
i8255.vsync=0;
i8255.portA=0xff;
i8255.portC=0x0f;
i8255.p555=0; // 555 timer for prompt
i8255.read=function(addr){
	addr&=0x0003;
	switch (addr) {
		case 0: // PORT A
			return this.portA;
		case 1: // PORT B
			return this.portBr();
		case 2: // PORT C
			return this.portCr();
		default:
			return 0xff;
	}
};
i8255.write=function(addr,data){
	addr&=0x0003;
	data&=0xff;
	switch (addr) {
		case 0:
			this.portA=data&0xff;
			// bit 0=3: selection of key matrix
			// bit 7; 0: reset 555 timer for prompt, 1: not reset
			if ((this.portA&0x80)==0) {
				// Reset 555 timer
				this.p555=new Date().getMilliseconds();
			}
			return;
		case 1:
			// read-only
			return;
		case 2:
			this.portCw(data);
			return;
		default:
			// portC-controling code will be here
			if (data&0x80) return; // Ignore configuration
			if (data&0x01) {
				// set bit
				data&=0x0e;
				data>>=1;
				data=1<<data;
				data|=this.portC;
				this.portCw(data);
			} else {
				// clear bit
				data&=0x0e;
				data>>=1;
				data=1<<data;
				data^=0xff;
				data&=this.portC;
				this.portCw(data);
			}
			return;
	}
};
i8255.portCw=function(data){
	// bit 4-7: read-only
	data&=0x0f;
	this.portC=data;
	// bit 0: /Vgate
	if (data&0x01) {
		// Hide blank canvas.
		dom.showDisplay();
	} else {
		// Show blank canvas after 30 msec.
		setTimeout(function(){
			if (!(i8255.portC&0x01)) dom.blankDisplay();
		},30);
	}
	// bit 1: WDATA
	// bit 2: LED red/green
	display.led(data&0x04);
	// bit 3: motor on
}
i8255.portCr=function(){
	var ret;
	// No wait mode for Vsync
	if (!this.vsync) this.vsync=1;
	else this.vsync=0;
	// 555 timer for prompt
	var prompt=new Date().getMilliseconds();
	prompt-=this.p555;
	if (prompt<0) prompt+=1000;
	// Prepare result
	ret =(this.vsync)      ? 0x80:0;
	ret|=(499<prompt)      ? 0x40:0;
	ret|=this.portC;
	return ret;
}
i8255.keymatrix=[0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff];
i8255.portBr=function(){
	var a=this.portA&0x0f;
	if (9<a) return 0xff;
	return this.keymatrix[a];
}
i8255.keymap=new Array();
i8255.keymap["A".charCodeAt(0)]=[0x40]; 
i8255.keymap["B".charCodeAt(0)]=[0x62]; 
i8255.keymap["C".charCodeAt(0)]=[0x61]; 
i8255.keymap["D".charCodeAt(0)]=[0x41]; 
i8255.keymap["E".charCodeAt(0)]=[0x21]; 
i8255.keymap["F".charCodeAt(0)]=[0x51]; 
i8255.keymap["G".charCodeAt(0)]=[0x42]; 
i8255.keymap["H".charCodeAt(0)]=[0x52]; 
i8255.keymap["I".charCodeAt(0)]=[0x33]; 
i8255.keymap["J".charCodeAt(0)]=[0x43]; 
i8255.keymap["K".charCodeAt(0)]=[0x53]; 
i8255.keymap["L".charCodeAt(0)]=[0x44]; 
i8255.keymap["M".charCodeAt(0)]=[0x63]; 
i8255.keymap["N".charCodeAt(0)]=[0x72]; 
i8255.keymap["O".charCodeAt(0)]=[0x24]; 
i8255.keymap["P".charCodeAt(0)]=[0x34]; 
i8255.keymap["Q".charCodeAt(0)]=[0x20]; 
i8255.keymap["R".charCodeAt(0)]=[0x31]; 
i8255.keymap["S".charCodeAt(0)]=[0x50]; 
i8255.keymap["T".charCodeAt(0)]=[0x22]; 
i8255.keymap["U".charCodeAt(0)]=[0x23]; 
i8255.keymap["V".charCodeAt(0)]=[0x71]; 
i8255.keymap["W".charCodeAt(0)]=[0x30]; 
i8255.keymap["X".charCodeAt(0)]=[0x70]; 
i8255.keymap["Y".charCodeAt(0)]=[0x32]; 
i8255.keymap["Z".charCodeAt(0)]=[0x60]; 
i8255.keymap["1".charCodeAt(0)]=[0x00]; 
i8255.keymap["2".charCodeAt(0)]=[0x10]; 
i8255.keymap["3".charCodeAt(0)]=[0x01]; 
i8255.keymap["4".charCodeAt(0)]=[0x11]; 
i8255.keymap["5".charCodeAt(0)]=[0x02]; 
i8255.keymap["6".charCodeAt(0)]=[0x12]; 
i8255.keymap["7".charCodeAt(0)]=[0x03]; 
i8255.keymap["8".charCodeAt(0)]=[0x13]; 
i8255.keymap["9".charCodeAt(0)]=[0x04]; 
i8255.keymap["0".charCodeAt(0)]=[0x14]; 
i8255.keymap[" ".charCodeAt(0)]=[0x91]; 
i8255.keymap["=".charCodeAt(0)]=[0x25]; 
i8255.keymap[";".charCodeAt(0)]=[0x54]; 
i8255.keymap[0x10]=[0x80]; //shift
i8255.keymap[0x24]=[0x90]; //home 
i8255.keymap[0x2e]=[0x81]; //del 
i8255.keymap[0x08]=[0x81]; //bs -> del 
i8255.keymap[0x28]=[0x92]; //down 
i8255.keymap[0x27]=[0x83]; //right 
i8255.keymap[0x1b]=[0x93]; //esc -> break 
i8255.keymap[0x0d]=[0x84]; //cr 
i8255.keymap[0xbc]=[0x73]; //, 
i8255.keymap[0xbe]=[0x64]; //. 
i8255.keymap[0xbf]=[0x74]; //"/" 
i8255.keymap[0xad]=[0x05]; //- 
i8255.keymap[0x09]=[0x65]; //tab -> kana
i8255.keymap[0x2d]=[0x81,0x80]; //ins -> shift + del
i8255.keymap[0x26]=[0x92,0x80]; //up -> shift + down
i8255.keymap[0x25]=[0x83,0x80]; //left -> shift + right
i8255.keydown=function(key){
	var maskdata=this.keymap[key];
	if (maskdata==null) return;
	var i,mask,pos;
	for (i=0;i<maskdata.length;i++) {
		mask=maskdata[i];
		pos=mask>>4;
		mask=(1<<(mask&0x0f))^0xff;
		this.keymatrix[pos]&=mask;
	}
}
i8255.keyup=function(key){
	var maskdata=this.keymap[key];
	if (maskdata==null) return;
	var i,mask,pos;
	for (i=0;i<maskdata.length;i++) {
		mask=maskdata[i];
		pos=mask>>4;
		mask=1<<(mask&0x0f);
		this.keymatrix[pos]|=mask;
	}
}

