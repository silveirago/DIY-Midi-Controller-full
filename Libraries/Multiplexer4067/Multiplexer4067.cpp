
#include <Arduino.h>
#include "Multiplexer4067.h"

static const uint8_t _muxChAddress[16][4] = {
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
};
	

Multiplexer4067::Multiplexer4067(const uint8_t S0,const uint8_t S1,const uint8_t S2,const uint8_t S3,const uint8_t SIG){
	_adrsPin[0] = S0;
	_adrsPin[1] = S1;
	_adrsPin[2] = S2;
	_adrsPin[3] = S3;
	_sig = SIG;
}

void Multiplexer4067::begin(uint8_t dir){
	uint8_t i;
	for (i = 0; i < 4; i++){
		pinMode(_adrsPin[i], OUTPUT); 
		digitalWrite(_adrsPin[i],LOW);
	}
	if (dir != 1) { //out
		_dir = 0;
		pinMode(_sig, OUTPUT); 
	} else {
		_dir = 1;
	}
}

int Multiplexer4067::readChannel(uint8_t ch,uint8_t readings){
	if (_dir > 0){
		uint8_t i;
		if (ch > 15) ch = 15;
		if (readings < 1) readings = 1;
		int temp = 0;
		_addressing(ch);
		for (i=0;i<readings;i++){
			temp = temp + analogRead(_sig);
		}
		return temp/readings;
	}
	return 0;
}


void Multiplexer4067::_addressing(uint8_t ch){
	uint8_t i;
	for (i = 0; i < 4; i ++){
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
			digitalWriteFast(_adrsPin[i],_muxChAddress[ch][i]);
		#else
			digitalWrite(_adrsPin[i],_muxChAddress[ch][i]);
		#endif
	}
}