/**********************************
* 8080 emulator written by Katsumi *
*            ver 0.80              *
*      This script is released     *
*       under the LGPL v2.1.       *
***********************************/

cpu=function(){};
cpu.prototype=z80;
i8080=new cpu();
// Some modifications follow.
i8080.setT4_5=  function(){ this.clk+=5; }
i8080.setT4_7=  function(){ this.clk+=7; }
i8080.setT6_5=  function(){ this.clk+=5; }
i8080.setT10_11=function(){ this.clk+=11; }
i8080.setT11_10=function(){ this.clk+=10; }
i8080.setT19_18=function(){ this.clk+=18; }
i8080.codeVector=new Array(256);
for (i=0;i<16;i++) {
	eval("i8080.codeVector["+i+"]=function(){ i8080.code0"+i.toString(16).toUpperCase()+"(); };");
}
for (i=16;i<256;i++) {
	eval("i8080.codeVector["+i+"]=function(){ i8080.code"+i.toString(16).toUpperCase()+"(); };");
}

// Delete Z80-specific commands.
i8080.code08=i8080.codeVOID;
i8080.code10=i8080.codeVOID;
i8080.code18=i8080.codeVOID;
i8080.code20=i8080.codeVOID;
i8080.code28=i8080.codeVOID;
i8080.code30=i8080.codeVOID;
i8080.code38=i8080.codeVOID;
i8080.codeCB=i8080.codeVOID;
i8080.codeD9=i8080.codeVOID;
i8080.codeDD=i8080.codeVOID;
i8080.codeED=i8080.codeVOID;
i8080.codeFD=i8080.codeVOID;
