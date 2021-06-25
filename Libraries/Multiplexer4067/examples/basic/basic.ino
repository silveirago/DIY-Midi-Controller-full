#include "Multiplexer4067.h"


Multiplexer4067 mplex = Multiplexer4067(3,4,5,6,A0);


void setup(){
  mplex.begin();
  mplex.readChannel(0);//it read the channel 0
  //default it read 5 times but can be changed:
  //mplex.readChannel(0,10);
}

void loop(){
}