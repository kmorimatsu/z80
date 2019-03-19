/*******************************
* ZK-80 web written by Katsumi *
*   This script is released    *
*     under the LGPL v2.1.     *
*******************************/

display=new Object();
display.led=new Array();
display.status=0;
display.blank=new Image();
display.each=new Image();
display.keys=new Image();
display.init=function(){
	this.led8=dom.getContext("led8");
	this.keypad=dom.getContext("keypad");
	this.blank.src="./blank.gif?"+ new Date().getTime();
	this.each.src="./each.gif?"+ new Date().getTime();
	this.keys.src="./keys.png?"+ new Date().getTime();
};
display.blank.onload=function(){
	if (2<(++display.status)) display.loaded();
};
display.each.onload=function(){
	if (2<(++display.status)) display.loaded();
};
display.keys.onload=function(){
	if (2<(++display.status)) display.loaded();
};
display.loaded=function(){
	var i;
	// Prepare led array
	for (i=0;i<256;i++) {
		this.led8.drawImage(display.blank,252,0);
		if (i&0x80) this.led8.drawImage(display.each,252,0);
		if (i&0x40) this.led8.drawImage(display.each,216,0);
		if (i&0x20) this.led8.drawImage(display.each,180,0);
		if (i&0x10) this.led8.drawImage(display.each,144,0);
		if (i&0x08) this.led8.drawImage(display.each,108,0);
		if (i&0x04) this.led8.drawImage(display.each,72,0);
		if (i&0x02) this.led8.drawImage(display.each,36,0);
		if (i&0x01) this.led8.drawImage(display.each,0,0);
		this.led[i]=this.led8.getImageData(252,0,36,50);
	}
	// Clear led array
	this.led8.clearRect(0,0,308,50);
	for (i=0;i<8;i++) {
		this.write(i,0x00);
	}
	// Prepare key pad
	this.keypad.drawImage(display.keys,0,0);
	var keypad=dom.getElement("keypad");
	keypad.onmousedown=function(e){
		io.mousedown(parseInt(5*dom.x(keypad,e)/keypad.width),parseInt(5*dom.y(keypad,e)/keypad.height));
	};
	keypad.onmouseup=function(e){
		setTimeout(function (){
				io.mouseup(parseInt(5*dom.x(keypad,e)/keypad.width),parseInt(5*dom.y(keypad,e)/keypad.height));
			},50);
	};
	// Start CPU
	start();
};
display.on=true;
display.ledstatus=[0,0,0,0,0,0,0,0];
display.update=function(addr,data){
	this.led8.putImageData(display.led[data],addr*36+(3<addr?18:0),0);
};
display.write=function(addr,data){
	addr&=0x07;
	data&=0xff;
	this.ledstatus[addr]=data;
	if (this.on) {
		this.update(addr,data);
	}
};
display.control=function(on){
	var i;
	if (on && !this.on) {
		this.on=true;
		for (i=0;i<8;i++) {
			this.update(i,this.ledstatus[i]);
		}
	} else if (this.on && !on) {
		for (i=0;i<8;i++) {
			this.update(i,0);
		}
		this.on=false;
	}
}
display.mousedown=function(x,y){
	var width=keypad.width/5;
	var height=keypad.height/5;
	var image=this.keypad.getImageData(x*width,y*height,width,height);
	try {
		var audio=dom.getElement("sounddown");
		audio.play();
		audio.currentTime=0;
	} catch(e) {}
	this.keypad.putImageData(image,x*width+3,y*height+3);
};
display.mouseup=function(){
	try {
		var audio=dom.getElement("soundup");
		audio.play();
		audio.currentTime=0;
	} catch(e) {}
	this.keypad.drawImage(display.keys,0,0);
};

