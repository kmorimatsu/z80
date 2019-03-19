/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	memory.read(addr);
	memory.write(addr,data);
	memory.init(ramSize);
	
	This object requires memory.rom array containing 1024 bytes monitor program.
	
	This object also privides memory-mapped I/O functions.
*/
memory=new Object();
memory.ram=Array(0xd000); // RAM + VRAM
memory.ramVoid=0xd000;
memory.init=function(ramSize){
	// ramSize must be specified in kb 
	var i;
	ramSize*=0x0400;
	this.ramVoid=0x1000+ramSize;
	for (i=0;i<0xd000;i++) {
		this.ram[i]=0;
	}
};
memory.read=function(addr){
	addr&=0xffff;
	if (addr<0x1000) {
		// ROM
		return this.rom[addr];
	} else if (addr<this.ramVoid) {
		// RAM
		return this.ram[addr-0x1000];
	} else if (addr<0xd000) {
		return 0xff;
	} else if (addr<0xe000) {
		// VRAM
		return this.ram[addr-0x1000];
	} else if (addr<0xf000) {
		// I/O
		switch(addr&0x0c){
			case 0x00: // 8255
				return i8255.read(addr);
			case 0x04: // 8253
				return i8253.read(addr);
			case 0x08: // tempo
				var ret=new Date().getTime();
				ret=(ret&0x10) ? 1:0; // 32 Hz
				return ret;
			default:
				return 0xff;
		}
	} else {
		return 0xff;
	}
};
memory.write=function(addr,data){
	addr&=0xffff;
	data&=0xff;
	if (addr<0x1000) {
		// ROM
	} else if (addr<0xd000) {
		// RAM
		this.ram[addr-0x1000]=data;
	} else if (addr<0xe000) {
		// VRAM
		this.ram[addr-0x1000]=data;
		display.write(addr,data);
	} else if (addr<0xf000) {
		// I/O
		switch(addr&0x0c){
			case 0x00: // 8255
				i8255.write(addr,data);
				return;
			case 0x04: // 8253
				i8253.write(addr,data);
				return;
			case 0x08: // Sound on/off
				if (data&0x01) audio.start();
				else audio.stop();
				return;
			default:
				return;
		}
	} else {
		return 0xff;
	}
};
