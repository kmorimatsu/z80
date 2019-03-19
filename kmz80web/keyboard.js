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

keyboard=new Object();
keyboard.init=function(){
	var i,j,data,obj;
	
	// Set the contexts.
	this.context=dom.getContext("keyboard");
	
	// Construct charRGBA and bgRGBA for fonts.
	this.context.putImageData(display.font[0x43],1,1);
	data=this.context.getImageData(1,1,1,1).data;
	this.charRGBA=data[0]+data[1]+data[2]+data[3];
	this.context.putImageData(display.font[0x00],1,1);
	data=this.context.getImageData(1,1,1,1).data;
	this.bgRGBA=data[0]+data[1]+data[2]+data[3];
	
	// Create black blank keys
	this.context.fillStyle = 'rgb(0,0,0)';
	for (i=0;i<11;i++) {
		for (j=0;j<4;j++) {
			this.context.fillRect(i*20+1,j*20+1,18,18);
		}
	}
	for (j=0;j<5;j++) {
		this.context.fillRect(15*20+1,j*20+1,18,18);
	}
	// Create blue blank keys
	this.context.fillStyle = 'rgb(0,112,192)';
	for (i=11;i<15;i++) {
		for (j=0;j<5;j++) {
			this.context.fillRect(i*20+1,j*20+1,18,18);
		}
	}
	// Create yellow blank keys
	this.context.fillStyle = 'rgb(255,192,0)';
	this.context.fillRect(10*20+1,3*20+1,18,18);
	for (i=0;i<11;i++) {
		this.context.fillRect(i*20+1,4*20+1,18,18);
	}
	this.context.fillRect(3*20+1,4*20+1,38,18);
	this.context.fillRect(8*20+1,4*20+1,38,18);
	
	// Normal keys
	this.context.fillStyle = 'rgb(255,255,255)';
	for (i=0;i<keyboard.keys.length;i+=3) {
		this.drawChar(keyboard.keys[i],keyboard.keys[i+1],keyboard.keys[i+2]);
	}
	// Kana characters
	this.context.fillStyle = 'rgb(255,64,64)';
	for (i=0;i<keyboard.kanaKeys.length;i+=3) {
		this.drawChar(keyboard.kanaKeys[i],keyboard.kanaKeys[i+1],keyboard.kanaKeys[i+2]);
	}
	// Control keys
	this.context.fillStyle = 'rgb(0,0,0)';
	this.context.font="10px 'Times New Roman'";
	this.context.fillText('Shift',1,93,18);
	this.context.fillText('Home',21,89,18);
	this.context.fillText('Clr',23,99,18);
	this.context.fillText('Inst',43,89,18);
	this.context.fillText('Del',43,99,18);
	this.context.fillText('Space',68,93,36);
	this.drawChar(105,81,0x50); this.drawChar(105,90,0x80);
	this.drawChar(124,81,0x45); this.drawChar(126,90,0x40);
	this.context.fillText('Break',141,93,18);
	this.context.fillText('CR',173,93,36);
	this.context.fillText('Shift',201,93,18);
	this.drawChar(201,62,0x94); this.drawChar(209,62,0x95);
	this.context.fillText(unescape('%u82F1%u6570'),201,78,18); // Eisuu, in Chinise characters
	
	// Handling the events
	obj=dom.getElement("keyboard");
	obj.onmousedown=function(e){
		keyboard.onmousedown(parseInt(16*dom.x(obj,e)/obj.width),parseInt(5*dom.y(obj,e)/obj.height));
	};
	obj.onmouseup=function(e){
		setTimeout(function (){
				keyboard.onmouseup(parseInt(16*dom.x(obj,e)/obj.width),parseInt(5*dom.y(obj,e)/obj.height));
			},50);
	};
};
keyboard.onmousedown=function(x,y){
	var pos,mask;
	if (x==4 && y==4) x=3; // Space
	if (x==9 && y==4) x=8; // CR
	pos=(y<<1)|(x&1);
	mask=1<<(x>>1);
	if ((x==0 || x==10) && y==4) {
		// Shift
		i8255.keymatrix[pos]^=mask;
		if (i8255.keymatrix[pos]&mask) {
			// Shift key is up, so draw yellow lines
			this.context.fillStyle = 'rgb(255,192,0)';
		} else {
			// Shift key is down, so draw blue lines
			this.context.fillStyle = 'rgb(64,64,255)';
			this.context.strokeStyle = 'rgb(64,64,255)';
		}
		this.context.fillRect(x*20+1,4*20+1,18,2);
		this.context.fillRect(x*20+1,4*20+16,18,2);
	} else {
		// Other keys
		mask=0xff^mask;
		i8255.keymatrix[pos]&=mask;
	}
};
keyboard.onmouseup=function(x,y){
	var pos,mask;
	if (x==4 && y==4) x=3; // Space
	if (x==9 && y==4) x=8; // CR
	pos=(y<<1)|(x&1);
	mask=1<<(x>>1);
	if ((x==0 || x==10) && y==4) {
		// Shift
	} else {
		// Other keys
		i8255.keymatrix[pos]|=mask;
	}
};
keyboard.keys=[
	// 1234567890-
	2,11,0x21, 2,2,0x61, 22,11,0x22, 22,2,0x62, 42,11,0x23, 42,2,0x63,
	62,11,0x24, 62,2,0x64, 82,11,0x25, 82,2,0x65, 102,11,0x26, 102,2,0x66,
	122,11,0x27, 122,2,0x67, 142,11,0x28, 142,2,0x68, 162,11,0x29, 162,2,0x69,
	182,11,0x20, 182,2,0x60, 202,11,0x2A, 202,2,0x6A, 
	// QWERTYUIOP=
	2,31,0x11, 2,22,0x51, 22,31,0x17, 22,22,0x57, 42,31,0x05, 42,22,0x45,
	62,31,0x12, 62,22,0x52, 82,31,0x14, 82,22,0x54, 102,31,0x19, 102,22,0x59,
	122,31,0x15, 122,22,0x55, 142,31,0x09, 142,22,0x49, 162,31,0x0F, 162,22,0x4F,
	182,31,0x10, 182,22,0x50, 202,31,0x2B, 202,22,0x6B, 
	// ASDFGHJKL;+
	2,51,0x01, 2,42,0x41, 22,51,0x13, 22,42,0x53, 42,51,0x04, 42,42,0x44,
	62,51,0x06, 62,42,0x46, 82,51,0x07, 82,42,0x47, 102,51,0x08, 102,42,0x48,
	122,51,0x0A, 122,42,0x4A, 142,51,0x0B, 142,42,0x4B, 162,51,0x0C, 162,42,0x4C,
	182,51,0x2C, 182,42,0x6C, 202,51,0x1B, 202,42,0x5B, 
	// ZXCVBNM,./
	2,71,0x1A, 2,62,0x5A, 22,71,0x18, 22,62,0x58, 42,71,0x03, 42,62,0x43,
	62,71,0x16, 62,62,0x56, 82,71,0x02, 82,62,0x42, 102,71,0x0E, 102,62,0x4E,
	122,71,0x0D, 122,62,0x4D, 142,71,0x2F, 142,62,0x6F, 162,71,0x2E, 162,62,0x6E,
	182,71,0x2D, 182,62,0x6D, 
	// Blue keys first column
	222,2,0x5C, 222,11,0x1C, 222,22,0x70, 222,31,0x30, 222,42,0x74,
	222,51,0x34, 222,62,0x78, 222,71,0x38, 222,82,0x7C, 222,91,0x3C,
	// Blue keys secod column
	242,2,0x5D, 242,11,0x1D, 242,22,0x71, 242,31,0x31, 242,42,0x75,
	242,51,0x35, 242,62,0x79, 242,71,0x39, 242,82,0x7D, 242,91,0x3D,
	// Blue keys third column
	262,2,0x5E, 262,11,0x1E, 262,22,0x72, 262,31,0x32, 262,42,0x76,
	262,51,0x36, 262,62,0x7A, 262,71,0x3A, 262,82,0x7E, 262,91,0x3E,
	// Blue keys fourth column
	282,2,0x5F, 282,11,0x1F, 282,22,0x73, 282,31,0x33, 282,42,0x77,
	282,51,0x37, 282,62,0x7B, 282,71,0x3B, 282,82,0x7F, 282,91,0x3F,
	// Blue keys others
	                                       231,71,0xB8, 231,91,0xBC,
	251,11,0x9D, 251,31,0xB1, 251,51,0xB5, 251,71,0xB9, 251,91,0xBD,
	271,11,0x9E, 271,31,0xB2, 271,51,0xB6, 271,71,0xBA, 271,91,0xBE,
	291,11,0x9F, 291,31,0xB3, 291,51,0xB7, 291,71,0xBB, 291,91,0xBF,
	// Weeks etc
	302,2,0xD0, 302,11,0xD1, 302,22,0xD2, 302,31,0xD3, 302,42,0xD4,
	302,51,0xD5, 302,62,0xD6, 302,71,0xD7, 302,82,0xD8, 302,91,0xDC,
	311,11,0xDD, 311,31,0xDE, 311,51,0xD9, 311,71,0xDA, 311,91,0xDB,
];
keyboard.kanaKeys=[
	// nu fu a u e o ya yu yo wa ho ke
	10,11,0xA1, 30,11,0xA2, 50,11,0xA3, 70,11,0xA4, 90,11,0xA5, 
	110,11,0xA6, 130,11,0xA7, 150,11,0xA8, 170,11,0xA9, 190,11,0xA0, 
	210,11,0xAA, 230,11,0x9C, 
	// ta te i su ka n na ni ra se he mu
	10,31,0x91, 30,31,0x97, 50,31,0x85, 70,31,0x92, 90,31,0x94, 
	110,31,0x99, 130,31,0x95, 150,31,0x89, 170,31,0x8F, 190,31,0x90, 
	210,31,0xAB, 230,31,0xB0, 
	// chi to shi ha ki ku ma no ri re ro wo
	10,51,0x81, 30,51,0x93, 50,51,0x84, 70,51,0x86, 90,51,0x87, 
	110,51,0x88, 130,51,0x8A, 150,51,0x8B, 170,51,0x8C, 190,51,0xAC, 
	210,51,0x9B, 230,51,0xB4, 
	// tsu sa so hi ko mi mo ne ru me
	10,71,0x9A, 30,71,0x98, 50,71,0x83, 70,71,0x96, 90,71,0x82, 
	110,71,0x8E, 130,71,0x8D, 150,71,0xAF, 170,71,0xAE, 190,71,0xAD, 
];
keyboard.drawChar=function(x,y,ascii){
	var i,j,data;
	var bgColor=this.context.getImageData(x,y,1,1);
	this.context.fillRect(x,y,1,1);
	var charColor=this.context.getImageData(x,y,1,1);
	this.context.putImageData(display.font[ascii],x,y);
	for (i=0;i<8;i++) {
		for (j=0;j<8;j++) {
			data=this.context.getImageData(x+i,y+j,1,1).data;
			if (data[0]+data[1]+data[2]+data[3]==this.charRGBA) {
				this.context.putImageData(charColor,x+i,y+j);
			} else {
				this.context.putImageData(bgColor,x+i,y+j);
			}
		}
	}
};
