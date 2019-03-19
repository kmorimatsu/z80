/*******************************
* ZK-80 web written by Katsumi *
*   This script is released    *
*     under the LGPL v2.1.     *
*******************************/

// Choose CPU
if (get.cpu=="i8080" && typeof i8080 !="undefined") {
	cpu=i8080;
} else {
	cpu=z80;
}
// Initialize CPU
var speed=2048000; // 2.048 Mhz
dom.showSpeed(speed);
var maxspeed=speed;
cpu.setSpeed(speed);
cpu.reset();
// Initialize Memory
memory.program=[// Tiny test program
0x01,0x00,0x49,0x21,0xf8,0x83,0xb5,0x6f,0x70,0x06,0x09,0xcd,0xef,0x02,0x05,0xc2,
0x0b,0x80,0x3c,0x71,0xc3,0x00,0x80,0x00,0xaf,0xee,0x01,0xf6,0x0e,0xd3,0xfb,0x06,
0x09,0xcd,0xef,0x02,0x05,0xc2,0x21,0x80,0xc3,0x19,0x80
/*
(8000)
loop:
LD    BC,4900
LD    HL,83F8
OR    L
LD    L,A
LD    (HL),B
LD    B,09
wait:
CALL  02EF
DEC   B
JP    NZ,wait:
INC   A
LD    (HL),C
JP    loop:
(8018)
XOR   A
loop2:
XOR   01
OR    0E
OUT   FB,A
LD    B,09
wait2:
CALL  02EF
DEC   B
JP    NZ,wait2:
JP    loop2:
*/
];
memory.init();
// The other initializations
if (get.debug  && typeof debugDisplay !="undefined") {
	// Show the debug buttons
	dom.showDebug();
}
if (get.clock) {
	// Over or under clock setting.
	switch (get.clock.substr(-1)) {
		case "k": case "K":
			maxspeed=speed=parseFloat(get.clock.substr(0,get.clock.length-1))*1000;
			break;
		case "m": case "M":
			maxspeed=speed=parseFloat(get.clock.substr(0,get.clock.length-1))*1000000;
			break;
		case "g": case "G":
			maxspeed=speed=parseFloat(get.clock.substr(0,get.clock.length-1))*1000000000;
			break;
		default:
			maxspeed=speed=parseInt(get.clock,10);
			break;
	}
	cpu.setSpeed(speed);
	dom.showSpeed(speed);
}
if (get.start) {
	// Immediate start user program from specified address.
	cpu.loadPC(parseInt(get.start,16));
	memory.write(0x83c7,0x00);
	memory.write(0x83c8,0x00);
	cpu.loadSP(0x83c7);
}
if (get.void=="rst7") {
	// Use RST 38H for void code instead of NOP
	cpu.codeVOID=cpu.codeFF;
}
if (get.daa=="i8080" || get.daa=="tk80e") {
	// Downgrade DAA to i8080 one.
	cpu.code27copy=cpu.code27;
	cpu.code27=function(){
		this.clearNflag(); // Force addition.
		this.code27copy();
	};
}
if (file.storage.getItem("step")) {
	dom.clickToggle();
}
if (file.storage.getItem("protect")) {
	dom.clickToggle2();
}
// Following function will be called after initializing display.
start=function(){
	var time;
	time=new Date().getTime();
	setTimeout(function(){
			var from=time;
			var to=time=new Date().getTime();
			var msec=to-from;
			if (msec<100 && speed<maxspeed) {
				speed<<=1;
				if (125<speed && speed<250) speed=125;
				if (maxspeed<speed) speed=maxspeed;
				cpu.setSpeed(speed);
				dom.showSpeed(speed);
			} else if (250<msec) {
				speed>>=1;
				if (speed<1) speed=1;
				cpu.setSpeed(speed);
				dom.showSpeed(speed);
			}
			cpu.exec(msec);
			if (cpu.step) {
				showRegisters();
			} else {
				setTimeout(arguments.callee,10);
			}
		},10);
};
