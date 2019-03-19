/*******************************
* ZK-80 web written by Katsumi *
*   This script is released    *
*     under the LGPL v2.1.     *
*******************************/

/*
	Public methods:
	memory.read(addr);
	memory.write(addr,data);
	memory.init();
	memory.autoRun();
	memory.stepRun();
	
	This object requires memory.rom array containing 1024 bytes monitor program.
	
	This object also privides memory-mapped I/O functions.
*/
memory=new Object();
memory.ram=Array(0x8000);
memory.init=function(){
	var i;
	for (i=0;i<this.ram.length;i++) {
		this.ram[i]=0;
	}
	if (this.program) {
		for (i=0;i<this.program.length;i++) {
			this.ram[i]=this.program[i];
		}
	}
	if (file.storage.getItem("protect") && file.storage.getItem("memory")) {
		var ram=""+file.storage.getItem("memory");
		for (var i=0;i<ram.length;i+=2) {
			this.ram[i>>1]=parseInt(ram.substr(i,2),16);
		}
	}
};
memory.interrupt=false;
memory.autoRun=function(){
	this.interrupt=false;
};
memory.stepRun=function(){
	this.interrupt=true;
};
memory.enable=true;
memory.ce=function(on){
	if (on) this.enable=true;
	else this.enable=false;
}
memory.read=function(addr){
	if (!this.enable) return 0xff;
	addr&=0xffff;
	if (addr<this.rom.length) {
		// ROM
		return this.rom[addr];
	} else if (addr<0x8000) {
		return 0xff;
	} else {
		if (this.interrupt && cpu.m1) cpu.interrupt(0xff);
		return this.ram[addr&0x7fff];
	}
};
memory.write=function(addr,data){
	if (!this.enable) return;
	addr&=0xffff;
	data&=0xff;
	if (addr<0x8000) { // ROM
		return;
	} else if (addr<0x83F8) { // RAM
		this.ram[addr&0x7fff]=data;
	} else if (addr<0x8400) { // RAM + display
		this.ram[addr&0x7fff]=data;
		display.write(addr,data);
	} else { // RAM
		this.ram[addr&0x7fff]=data;
	}
};
