void loop() {

#ifdef ATMEGA32U4

  // Ele lerá mensagens de entrada MIDI se estiver usando o ATMGA32U4
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
