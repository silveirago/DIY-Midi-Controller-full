byte LED = 9;

// MyHandleNoteON is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void myHandleNoteOn(byte channel, byte pitch, byte velocity) { 
  digitalWrite(LED,HIGH);  //Turn LED on
}

// MyHandleNoteOFF is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void myHandleNoteOff(byte channel, byte pitch, byte velocity) { 
  digitalWrite(LED,LOW);  //Turn LED off
}

void myHandleControlChange(byte channel, byte ccNum, byte ccVal) { 
  
}
