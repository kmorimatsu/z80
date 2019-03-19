/********************************
* KM-Z80 web written by Katsumi *
*    This script is released    *
*      under the LGPL v2.1.     *
********************************/

/*
	Public methods:
	audio.set(freq);
	audio.start();
	audio.stop();
*/
audio=new Object();
audio.audioObj=new Audio();
audio.volume=0.05;
audio.freq=440;
audio.rate=48000;
audio.xor=audio.rate/audio.freq/2;
audio.set=function(freq){
	this.freq=freq;
	this.xor=this.rate/freq/2;
};
audio.start=function(){};
audio.stop=function(){};
if ((typeof AudioContext)!="undefined"){
	// Begin Web Audio API for Chrome/FireFox
	audio.on=0;
	audio.pos=0;
	audio.context=new AudioContext();
	audio.rate=audio.context.sampleRate;
	audio.node=audio.context.createScriptProcessor(512, 1, 1);
	audio.node.onaudioprocess=function(e){ audio.process(e); };
	// audio.process will be called periodically.
	audio.process=function(e){
		var i;
		var data=e.outputBuffer.getChannelData(0);
		// this.xor shows the point where signal must change
		for(i=0;i<data.length;i++){
			if (this.xor*2<(++this.pos)) audio.pos=0;
			if (this.on) {
				if (this.pos<this.xor) data[i]=0-this.volume;
				else data[i]=this.volume;
			} else {
				data[i]=0;
			}
		}
	};
	audio.node.connect(audio.context.destination);
	audio.start=function(){
		this.on=1;
		// audio.node.connect starts the sound.
		this.node.connect(this.context.destination);
	}
	audio.stop=function(){
		this.on=0;
		this.pos=0;
		// audio.node.connect stopss the sound.
		setTimeout(function(){ audio.node.disconnect(); },20);
	}
	// End Web Audio API for Chrome/FireFox
} else if ((typeof webkitAudioContext)!="undefined"){
	// Begin Chrome and Safari specific settings
	audio.on=0;
	audio.pos=0;
	audio.context=new webkitAudioContext();
	audio.rate=audio.context.sampleRate;
	audio.node=audio.context.createJavaScriptNode(512, 1, 1);
	audio.node.onaudioprocess=function(e){ audio.process(e); };
	// audio.process will be called periodically.
	audio.process=function(e){
		var i;
		var data=e.outputBuffer.getChannelData(0);
		// this.xor shows the point where signal must change
		for(i=0;i<data.length;i++){
			if (this.xor*2<(++this.pos)) audio.pos=0;
			if (this.on) {
				if (this.pos<this.xor) data[i]=0-this.volume;
				else data[i]=this.volume;
			} else {
				data[i]=0;
			}
		}
	};
	audio.node.connect(audio.context.destination);
	audio.start=function(){
		this.on=1;
		// audio.node.connect starts the sound.
		this.node.connect(this.context.destination);
	}
	audio.stop=function(){
		this.on=0;
		this.pos=0;
		// audio.node.connect stopss the sound.
		setTimeout(function(){ audio.node.disconnect(); },20);
	}
	// End Chrome and Safari specific settings
} else if (audio.audioObj.mozSetup) {
	// Begin FireFox specific settins for Audio Data API (disabled after FireFox 38)
	audio.on=0;
	audio.pos=0;
	audio.timer=0;
	audio.audioObj.mozSetup(1,audio.rate);
	// Addition of audio profile with indicated length of time
	audio.add=function(msec){
		var i;
		var size=32*msec;
		// Following line is good for some specific environment (usually not needed)
		this.audioObj.mozCurrentSampleOffset();
		if (size<=0) return;
		var data=new Float32Array(size);
		// this.xor shows the point where signal must change
		for (i=0;i<size;i++) {
			if ((this.xor*2)<(++this.pos)) this.pos=0;
			if (this.pos<this.xor) data[i]=0-this.volume;
			else data[i]=this.volume;
		}
		this.audioObj.mozWriteAudio(data);
	};
	// audio.loop will be called every 20 msec
	audio.loop=function(){
		if (!this.on) return;
		var now=new Date().getTime();
		this.add(now-this.timer);
		this.timer=now;
		setTimeout(function(){ audio.loop(); },20);
	};
	audio.start=function(){
		if (this.on) return;
		this.on=1;
		this.pos=0;
		this.timer=(new Date().getTime())-20;
		this.loop();
	};
	audio.stop=function(){
		this.on=0;
	};
	// End FireFox specific settings
} else {
	// Audio not available
}
