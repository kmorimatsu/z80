/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	display.init();
	display.write(addr,data);
	display(green);
*/

display=new Object();
display.fonts=new Image();
display.font=Array(256);
display.ledImage=Array(2);
display.init=function(){
	// Set the contexts.
	this.context=dom.getContext("display");
	this.ledContext=dom.getContext("led");
	// Load the font data. See also fonts.onload event.
	this.fonts.src="./fonts.png?"+ new Date().getTime();
};
display.fonts.onload=function(){
	display.onload();
	keyboard.init();
}
// display.onload will be called after sucessfull loading of font PNG image.
display.onload=function(){
	// Show the PNG image
	this.context.drawImage(display.fonts,0,0);
	// Construction of images for font
	for (h8=0;h8<16;h8++) {
		for (l8=0;l8<16;l8++) {
			this.font[h8*16+l8]=this.context.getImageData(l8*8,h8*8,8,8);
		}
	}
	// Construction of images for LED
	this.ledImage[1]=display.context.getImageData(128,0,16,16);
	this.ledImage[0]=display.context.getImageData(128,16,16,16);
	// Clear display
	display.all();
	// Construct blank canvas
	dom.getContext("blank").putImageData(this.context.getImageData(0,0,320,200),0,0);
	// Show LED (green)
	this.led(1);
	// All done let's start Z80
	start();
};
display.cache=Array(4096);
display.readPos=0;
display.writePos=0;
display.all=function(){
	var data,posy,posx;
	this.readPos=this.writePos;
	for (posy=0;posy<25;posy++) {
		for (posx=0;posx<40;posx++) {
			data=memory.read(0xD000+posy*40+posx);
			this.context.putImageData(this.font[data],posx<<3,posy<<3);
		}
	}
};
display.refresh=function(){
	var data,posy,posx;
	var num=this.readPos-this.writePos;
	while (num<0) num+=4096;
	if (1000<num) {
		display.all();
		return;
	}
	while (this.readPos!=this.writePos) {
		this.readPos=(this.readPos+1)&0xfff;
		data=this.cache[this.readPos];
		posx=data[0] & 0x3ff;
		data=data[1] & 0xff;
		posy=0;
		if (1000<=posx) return;
		while(40<=posx) {
			posy++;
			posx-=40;
		}
		this.context.putImageData(this.font[data],posx<<3,posy<<3);
	}
};
display.write=function(addr,data){
	if (this.readPos==this.writePos) {
		setTimeout(function(){ display.refresh();},1);
	}
	var pos=(this.writePos+1)&0xfff;
	this.cache[pos]=[addr,data];
	this.writePos=pos;
};
display.led=function(green){
	this.ledContext.putImageData(this.ledImage[green?1:0],0,0);
};
