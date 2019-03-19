/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	This is experimental.
	It works, but single-thread mode works faster than this.
	To use this, use "kmz80web.html?worker=1" as URL.
*/

if (window.Worker && get.worker){
	dom.useWorker=true;
	dom.showSpeed(speed);
	z80worker=new Worker("worker.js?"+get.query);
	gate=new Object();
	// Data from worker.
	z80worker.addEventListener("message", function(event){
			return gate[event.data[0]](event.data);
		}, true);
	z80worker.addEventListener("error", function(){} , true);
	gate.alert=function(data){
		alert(data[1]);
	};
	gate.dom=function(data){
		dom[data[1]](data[2]);
	};
	gate.display=function(data){
		display[data[1]](data[2],data[3]);
	};
	gate.memory=function(data){
		memory[data[1]](data[2],data[3]);
	};
	gate.audio=function(data){
		audio[data[1]](data[2]);
	};
	gate.file=function(data){
		if (data.length==3) {
			if (data[2].length=memory.ram.length) memory.ram=data[2];
		}
		file[data[1]]();
	};
	// Data for worker
	start=function(){
		z80worker.postMessage(["start"]);
	};
	window.onkeydown=function(e){
	 	z80worker.postMessage(["i8255","keydown",e.keyCode]);
		return false;
	};
	window.onkeyup=function(e){
	 	z80worker.postMessage(["i8255","keyup",e.keyCode]);
		return false;
	};
	file.updateCopy=file.update;
	file.update=function(data){
		this.updateCopy(data);
		z80worker.postMessage(["file","mzt",file.headerMZT,file.bodyMZT]);
	};
}
