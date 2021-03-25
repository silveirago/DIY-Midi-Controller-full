void loop() {

#ifdef ATMEGA32U4

  // it will read MIDI incoming messages if using ATMEGA32U4
  MIDIread();
#endif

#ifdef USING_BUTTONS
  buttons();
#endif

#ifdef USING_ENCODER
  encoders();
#endif

  cpu.run(); // for threads

}
