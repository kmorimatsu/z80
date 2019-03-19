/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

importScripts("z80.js","z80functions.js");
importScripts("memory.js","rom.js","io.js");
importScripts("i8253.js","i8255.js");
try { importScripts("mztape.js"); } catch(e) {}
importScripts("get.js","main.js");

addEventListener("message", function(event){
	return gate[event.data[0]](event.data);
}, false);

// Data for worker.
gate=new Object();
gate.start=function(data){
	start();
};
gate.i8255=function(data){
	i8255[data[1]](data[2]);
};
gate.file=function(data){
	file[data[1]](data[2],data[3]);
};

// Data from worker.
alert=function(str){
	postMessage(["alert",str]);
};

display=new Object();
display.write=function(addr,data){
	postMessage(["memory","write",addr,data]);
};
display.led=function(green){
	postMessage(["display","led",green]);
};

dom=new Object();
dom.blankDisplay=function(){
	postMessage(["dom","blankDisplay"]);
};
dom.showDisplay=function(){
	postMessage(["dom","showDisplay"]);
};
dom.showSpeed=function(speed){
	postMessage(["dom","showSpeed",speed]);
};

audio=new Object();
audio.set=function(freq){
	postMessage(["audio","set",freq]);
};
audio.start=function(){
	postMessage(["audio","start"]);
};
audio.stop=function(){
	postMessage(["audio","stop"]);
};

file=new Object();
file.saveHeader=function(){
	postMessage(["file","saveHeader",memory.ram]);
};
file.saveBody=function(){
	postMessage(["file","saveBody",memory.ram]);
};
file.mzt=function(header,body){
	this.headerMZT=header;
	this.bodyMZT=body;
};
file.loadHeader=function(){
	var i;
	for (i=0;i<128;i++) {
		memory.write(0x10f0+i,this.headerMZT[i]);
	}
};
file.loadBody=function(){
	var i,addr,data,from,size;
	from=memory.read(0x1104)|(memory.read(0x1105)<<8);
	size=memory.read(0x1102)|(memory.read(0x1103)<<8);
	for (i=0;i<size;i++) {
		memory.write(from+i,this.bodyMZT[i]);
	}
};
file.verifyBody=function(){
	postMessage(["file","verifyBody"]);
};

