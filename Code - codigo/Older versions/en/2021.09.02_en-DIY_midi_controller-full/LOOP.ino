void loop() {

#ifdef ATMEGA32U4

  // it will read MIDI incoming messages if using ATMEGA32U4
  MIDIread();
#endif

#ifdef ATMEGA328
  MIDI.read();
#endif

#ifdef USING_BUTTONS
  buttons();
#endif

#ifdef USING_ENCODER
  encoders();
  isEncoderMoving();

#endif

  cpu.run(); // for threads

  /////////////////////////////////////////////
  // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS

  for (int i = 0; i < N_M_FADERS; i++) {

    touchLine[i] = capSense[i].capacitiveSensor(capSenseSpeed); // cs_7_8.capacitiveSensor(20); // checks the capacitance in each fader
    checkTouch(i, touchLine[i]); // checks if the fader is beign touched

    updateFader(i, faderPos[i]); // moves the fader if it receives a MIDI in message
    faderPPos[i] = faderPos[i];

  }

#endif

}
