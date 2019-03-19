/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	file.loaded(obj);
	file.setMZT(obj);
	file.saveHeader();
	file.saveBody();
	file.loadHeader();
	file.loadBody();
	file.verifyBody();
*/
file=new Object();
file.header="";
file.body="";
file.headerMZT=[0x01,0x4e,0x4f,0x20,0x46,0x49,0x4c,0x45,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,
	0x00,0x00,0x00,0x12,0x00,0x00];
file.bodyMZT=new Array();
file.setMZT=function(obj){
	// Show the file upload input.
	obj.style.display='block';
	// Show the dialog to upload local file.
	obj.click();
};
file.loaded=function(obj){
	// This will be called when a file is uploaded.
	// If FileReader API is not supported, following code will fail.
	var fr = new FileReader();
	fr.onload = function () {
		var data=new Uint8Array(fr.result);
		file.update(data);
	};
	fr.readAsArrayBuffer(obj.files[0]);
	obj.style.display='none';
};
file.update=function(data){
	// This will be called when a file is sucessfully loaded by FileReader API.
	var header=new Array();
	var body=new Array();
	var i;
	// First 128 bytes contain header info.
	for (i=0;i<128;i++) {
		header[i]=data[i];
	}
	// Remaining area contains MZ binary code.
	for (i=128;i<data.length;i++) {
		body[i-128]=data[i];
	}
	this.bodyMZT=body;
	this.headerMZT=header;
	// Resolve file name.
	var str="";
	for (i=1;i<18;i++) {
		if (header[i]==0x0d) break;
		str+=String.fromCharCode(header[i]);
	}
	// header[18] and header[19] contain the size in 16 bit value.
	str+=" ("+(header[18]|header[19]<<8)+" bytes)";
	dom.getElement('mztfile').style.display='block';
	dom.getElement('mztfile').innerHTML=str;
}
// Following functions will be called from z80.codeC3(),
// that is modified in main.js.
// Each correspond to MZ monitor APIs for casette tape handling.
file.saveHeader=function(){
	var addr,data;
	this.header="";
	for (addr=0x10f0;addr<0x1170;addr++) {
		this.header+="%";
		data=memory.read(addr);
		if (data<16) this.header+="0";
		this.header+=data.toString(16);
	}
};
file.saveBody=function(){
	var addr,data,from,to;
	from=memory.read(0x1104)|(memory.read(0x1105)<<8);
	to=from+(memory.read(0x1102)|((memory.read(0x1103)<<8)));
	this.body="";
	for (addr=from;addr<to;addr++) {
		this.body+="%";
		data=memory.read(addr);
		if (data<16) this.body+="0";
		this.body+=data.toString(16);
	}
	// Create a link to save file in window.
	var file=dom.getElement('save');
	file.href="data:application/octet-stream,"+this.header+this.body;
	var str=""+file.innerHTML+" (";
	file.innerHTML=str.substr(0,str.indexOf("(")+1)+(to-from)+" bytes)";
	file.style.display='block';
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
	alert("file.verifyBody is not yet implemented.");
};

