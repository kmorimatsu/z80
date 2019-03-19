/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	io.read(addrL,addrH);
	io.write(addrL,addrH,data);
*/
io=new Object();
io.read=function(addrL,addrH){
	return 0xff;
};
io.write=function(addrL,addrH,data){
};
