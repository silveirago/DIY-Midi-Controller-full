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

}
