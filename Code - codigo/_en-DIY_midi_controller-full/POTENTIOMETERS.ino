#ifdef USING_POTENTIOMETERS

/////////////////////////////////////////////
// POTENTIOMETERS
void potentiometers() {

  // reads the pins from arduino
  for (int i = 0; i < N_POTS_ARDUINO; i++) {
    //potCState[i] = analogRead(POT_ARDUINO_PIN[i]);

    // one pole filter
    // y[n] = A0 * x[n] + B1 * y[n-1];
    // A = 0.15 and B = 0.85
    int reading = analogRead(POT_ARDUINO_PIN[i]);
    float filteredVal = filterA * reading + filterB * potPState[i]; // filtered value
    potCState[i] = filteredVal;
  }

#ifdef USING_MUX
  //It will happen if using a mux
  int nPotsPerMuxSum = N_POTS_ARDUINO; //offsets the buttonCState at every mux reading

  // reads the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_POTS_PER_MUX[j]; i++) {
      potCState[i + nPotsPerMuxSum] = mux[j].readChannel(POT_MUX_PIN[j][i]);
    }
    nPotsPerMuxSum += N_POTS_PER_MUX[j];
  }
#endif

  //Debug only
  //    for (int i = 0; i < nPots; i++) {
  //      Serial.print(potCState[i]); Serial.print(" ");
  //    }
  //    Serial.println();

  for (int i = 0; i < N_POTS; i++) { // Loops through all the potentiometers

    potMidiCState[i] = map(potCState[i], 0, 1023, 0, 127); // Maps the reading of the potCState to a value usable in midi

    potVar = abs(potCState[i] - potPState[i]); // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) { // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis(); // Stores the previous time
    }

    timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) { // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) { // If the potentiometer is still moving, send the change control
      if (potMidiPState[i] != potMidiCState[i]) {


        // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328
        // use if using with ATmega328 (uno, mega, nano...)

#ifdef USING_CUSTOM_CC_N
        MIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel - custom cc
#else
        MIDI.sendControlChange(CC + i, potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel
#endif //ATMEGA328

#elif ATMEGA32U4
        //use if using with ATmega32U4 (micro, pro micro, leonardo...)

#ifdef USING_CUSTOM_CC_N
        controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]); //  (channel, CC number,  CC value)
        MidiUSB.flush();
#else
        controlChange(POT_MIDI_CH, CC + i, potMidiCState[i]); //  (channel, CC number,  CC value)
        MidiUSB.flush();
#endif

#elif TEENSY
        //do usbMIDI.sendControlChange if using with Teensy

#ifdef USING_CUSTOM_CC_N
        usbMIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel
#else
        usbMIDI.sendControlChange(CC + i, potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel
#endif

#elif DEBUG
        Serial.print("Pot: ");
        Serial.print(i);
        Serial.print("  |  ch: ");
        Serial.print(POT_MIDI_CH);
        Serial.print("  |  cc: ");
#ifdef USING_CUSTOM_CC_N
        Serial.print(POT_CC_N[i]);
#else
        Serial.print(CC + i);
#endif
        Serial.print("  |  value: ");
        Serial.println(potMidiCState[i]);
#endif


        potPState[i] = potCState[i]; // Stores the current reading of the potentiometer to compare with the next
        potMidiPState[i] = potMidiCState[i];
      }
    }
  }
}

#endif
