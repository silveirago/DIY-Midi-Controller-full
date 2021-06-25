#ifdef USING_POTENTIOMETERS

int reading;

/////////////////////////////////////////////
// POTENTIOMETERS
void potentiometers() {

  // reads the pins from arduino
  for (int i = 0; i < N_POTS_ARDUINO; i++) {
    reading = analogRead(POT_ARDUINO_PIN[i]);
    responsivePot[i].update(reading);
    potCState[i] = responsivePot[i].getValue();
    //potCState[i] = reading;

  }


#ifdef USING_MUX
  //It will happen if using a mux
  int nPotsPerMuxSum = N_POTS_ARDUINO; //offsets the buttonCState at every mux reading

  // reads the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_POTS_PER_MUX[j]; i++) {
      //potCState[i + nPotsPerMuxSum] = mux[j].readChannel(POT_MUX_PIN[j][i]);
      reading = mux[j].readChannel(POT_MUX_PIN[j][i]);
      responsivePot[i + nPotsPerMuxSum].update(reading);
      potCState[i + nPotsPerMuxSum] = responsivePot[i + nPotsPerMuxSum].getValue();
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

#ifdef USING_MOTORIZED_FADERS
    potCState[i] = clipValue(potCState[i], faderMin[i], faderMax[i]);
    potMidiCState[i] = map(potCState[i], faderMin[i], faderMax[i], 0, 127); // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = clipValue( potMidiCState[i], 0, 127);
#else
    potCState[i] = clipValue(potCState[i], potMin, potMax);
    potMidiCState[i] = map(potCState[i], potMin, potMax, 0, 127); // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = clipValue(potMidiCState[i], 0, 127);
#endif

    //potMidiCState[i] = potCState[i] >> 3; // Maps the reading of the potCState to a value usable in midi

#ifdef USING_HIGH_RES_FADERS // if def
#ifdef USING_MOTORIZED_FADERS
    highResFader = map(potCState[i], faderMin[i], faderMax[i], 0, 16383); // converts the 10bit range to 14bit (it will skip some values on 14bit)
    faderMSB = highResFader / 128; // Most Sigficant Byte
    faderLSB = highResFader % 128; // Least Sigficant Byte
#else
    highResFader = map(potCState[i], potMin, potMax, 0, 16383); // converts the 10bit range to 14bit (it will skip some values on 14bit)
    faderMSB = highResFader / 128; // Most Sigficant Byte
    faderLSB = highResFader % 128; // Least Sigficant Byte
#endif
#endif // USING_HIGH_RES_FADERS

#ifdef USING_MACKIE // if using Mackie
    PBVal[i] = map(potCState[i], faderMin[i], faderMax[i] , 0, 16383);

    if (PBVal[i] < 0) {
      PBVal[i] = 0;
    }
    if (PBVal[i] > 16383) {
      PBVal[i] = 16383;
    }
#endif // USING_MACKIE


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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


#ifdef  USING_HIGH_RES_FADERS
      if (faderLSB != pFaderLSB) {
#else
      if (potMidiPState[i] != potMidiCState[i]) {
#endif // USING_HIGH_RES_FADERS

#ifdef USING_MOTORIZED_FADERS
        if (isTouched[i] == true) {
#endif


          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

          // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328
          // use if using with ATmega328 (uno, mega, nano...)

#ifdef USING_CUSTOM_CC_N

#ifndef USING_HIGH_RES_FADERS // if NOT def
          MIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel - custom cc
#endif // USING_HIGH_RES_FADERS

#ifdef USING_HIGH_RES_FADERS // if def

          MIDI.sendControlChange(POT_CC_N[i], faderMSB, POT_MIDI_CH); // MSB
          MIDI.sendControlChange(POT_CC_N[i] + 32, faderLSB, POT_MIDI_CH); // LSB

#endif // USING_HIGH_RES_FADERS

#else // USING_CUSTOM_CC_N

#ifndef USING_HIGH_RES_FADERS // if NOT def
          MIDI.sendControlChange(CC + i, potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel
#endif // USING_HIGH_RES_FADERS

#ifdef USING_HIGH_RES_FADERS // if def

          MIDI.sendControlChange(CC + i, faderMSB, POT_MIDI_CH); // MSB
          MIDI.sendControlChange(CC + i + 32, faderLSB, POT_MIDI_CH); // LSB

#endif // USING_HIGH_RES_FADERS

#endif //ATMEGA328


          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif ATMEGA32U4 //use if using with ATmega32U4 (micro, pro micro, leonardo...)

#ifdef USING_MACKIE // if USING Mackie protocol
          pitchBend(POT_MIDI_CH + i, PBVal[i]);
          MidiUSB.flush();

#elif USING_HIGH_RES_FADERS // if def

          controlChange(POT_MIDI_CH, CC + i, faderMSB); //  (channel, CC number,  CC value)
          MidiUSB.flush();
          controlChange(POT_MIDI_CH, CC + i + 32, faderLSB); //  (channel, CC number,  CC value)
          MidiUSB.flush();

#else // not using Mackie or HighRes

#ifdef USING_CUSTOM_CC_N
          controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]); //  (channel, CC number,  CC value)
          MidiUSB.flush();
#else
          controlChange(POT_MIDI_CH, CC + i, potMidiCState[i]); //  (channel, CC number,  CC value)
          MidiUSB.flush();
#endif // USING_CUSTOM_CC_N

#endif // NOT USING_MACKIE


          //#endif //use if using with ATmega32U4 (micro, pro micro, leonardo...)


          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif TEENSY
//do usbMIDI.sendControlChange if using with Teensy

#ifdef USING_CUSTOM_CC_N
usbMIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel
#else
usbMIDI.sendControlChange(CC + i, potMidiCState[i], POT_MIDI_CH); // CC number, CC value, midi channel
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
Serial.print(potMidiCState[i]);
Serial.print("  |  Responsive Value: ");
Serial.print(responsivePot[i].getValue());

#ifdef USING_HIGH_RES_FADERS // if def
Serial.print("  | |  High Res Fader: ");
Serial.print(highResFader);
Serial.print("  MSB: ");
Serial.print(faderMSB);
Serial.print("  LSB: ");
Serial.print(faderLSB);
#endif

#ifdef USING_MACKIE // if def
Serial.print("  |  Pitch Bend Val: ");
Serial.print(PBVal[i]);
#endif
Serial.println();
#endif

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -


          potPState[i] = potCState[i]; // Stores the current reading of the potentiometer to compare with the next
          potMidiPState[i] = potMidiCState[i];

#ifdef USING_HIGH_RES_FADERS // if def
          pFaderLSB = faderLSB;
#endif

#ifdef USING_MOTORIZED_FADERS
        }
#endif

      }
    }
  }
}




#endif // USING_POTENTIOMETERS
