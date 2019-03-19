/*******************************
* ZK-80 web written by Katsumi *
*   This script is released    *
*     under the LGPL v2.1.     *
*******************************/

file=new Object();
file.data=[0x00,0x80];
file.browse=function(obj){
	// This will be called when a file is uploaded.
	try {
		// If FileReader API is not supported, following code will fail.
		var fr = new FileReader();
		fr.onload = function () {
			var data=new Uint8Array(fr.result);
			file.update(data);
		};
		fr.readAsArrayBuffer(obj.files[0]);
	} catch (e) {
		alert(e);
	}
};
file.update=function(data){
	this.data=data;
};
file.load=function(){
	// This will be called when a file is sucessfully loaded by FileReader API.
	var addr,endaddr,i;
	var data=this.data;
	// First 2 bytes contain starting and ending addresses.
	addr=data[0]|(data[1]<<8);
	memory.write(0x83ee,data[0]);
	memory.write(0x83ef,data[1]);
	// Remaining area contains binary code.
	for (i=2;i<data.length;i++) {
		memory.write(addr++,data[i]);
	}
	addr--;
	memory.write(0x83ec,addr&0xff);
	memory.write(0x83ed,addr>>8);
};
file.store=function(){
	var addr,endaddr,data,result,addfunc;
	addr=memory.read(0x83ee)|(memory.read(0x83ef)<<8);
	endaddr=memory.read(0x83ec)|(memory.read(0x83ed)<<8);
	if (endaddr<addr) return;
	addfunc=function(data){
		if (data<16) return "%0"+data.toString(16);
		else return "%"+data.toString(16);
	};
	result=addfunc(addr&0xff);
	result+=addfunc(addr>>8);
	while(addr<=endaddr){
		result+=addfunc(memory.read(addr++));
	}
	// Create a link to save file in window.
	var filelink=dom.getElement('save');
	filelink.href="data:application/octet-stream,"+result;
	filelink.click();
};
// Web strage setting
if (window.localStorage) {
	file.storage=window.localStorage;
} else {
	file.storage=new Object();
	file.storage.getItem=function(key){
		var c=";"+document.cookie+";";
		var re=c.match(new RegExp(";[\\s]*"+key+"=([^;]?);"));
		if (re) return unescape(re[1]);
		else return "";
	};
	file.storage.setItem=function(key,value){
		document.cookie=key+"="+escape(value);
	};
}
