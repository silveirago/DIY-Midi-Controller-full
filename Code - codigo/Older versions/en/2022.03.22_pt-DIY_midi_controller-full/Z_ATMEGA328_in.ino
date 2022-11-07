// handleNoteOn is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void handleNoteOn2(byte channel, byte pitch, byte velocity) { 
  digitalWrite(9,HIGH);  //Turn LED on
}

// handleNoteOff is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void handleNoteOff2(byte channel, byte pitch, byte velocity) { 
  digitalWrite(9,LOW);  //Turn LED off
}

// handleControlChange
void handleControlChange2(byte channel, byte ccNum, byte ccVal) { 
  
}
