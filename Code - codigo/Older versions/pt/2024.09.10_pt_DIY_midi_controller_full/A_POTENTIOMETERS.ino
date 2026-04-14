#ifdef USING_POTENTIOMETERS

/////////////////////////////////////////////
// variables you don't need to change
int reading = 0;

// Responsive Analog Read
float snapMultiplier = 0.01;                      // (0.0 - 1.0) - Increase for faster, but less smooth reading
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // creates an array for the responsive pots. It gets filled in the Setup.

int potCState[N_POTS] = { 0 };  // Current state of the pot
int potPState[N_POTS] = { 0 };  // Previous state of the pot
int potVar = 0;                 // Difference between the current and previous state of the pot

int potMidiCState[N_POTS] = { 0 };  // Current state of the midi value
int potMidiPState[N_POTS] = { 0 };  // Previous state of the midi value

#ifdef USING_HIGH_RES_FADERS
// Use this to send CC using 2 bytes (14 bit res)
unsigned int highResFader = 0;  // stores the high res val
byte faderMSB = 0;              // Most Significant Byte
byte faderLSB = 0;              // Least Significant Byte
byte pFaderLSB = 0;             // Previous Most Significant Byte
#endif

//#ifdef USING_MACKIE
int PBVal[N_POTS] = { 0 };
//#endif

boolean potMoving = true;             // If the potentiometer is moving
unsigned long PTime[N_POTS] = { 0 };  // Previously stored time
unsigned long timer[N_POTS] = { 0 };  // Stores the time that has elapsed since the timer was reset

// one pole filter
// y[n] = A0 * x[n] + B1 * y[n-1];
// A = 0.15 and B = 0.85
//float filterA = 0.3;
//float filterB = 0.7;


/////////////////////////////////////////////
// Function
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
  int nPotsPerMuxSum = N_POTS_ARDUINO;  //offsets the buttonCState at every mux reading

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

  for (int i = 0; i < N_POTS; i++) {  // Loops through all the potentiometers

#ifdef USING_MOTORIZED_FADERS
    potCState[i] = clipValue(potCState[i], faderMin[i], faderMax[i]);
    potMidiCState[i] = map(potCState[i], faderMin[i], faderMax[i], 0, 127);  // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = clipValue(potMidiCState[i], 0, 127);
#else
    potCState[i] = clipValue(potCState[i], potMin, potMax);
    //potMidiCState[i] = map(potCState[i], potMin, potMax, 0, 127); // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = map(potCState[i], potMin, potMax, 0, 127);  // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = clipValue(potMidiCState[i], 0, 127);
#endif

    //potMidiCState[i] = potCState[i] >> 3; // Maps the reading of the potCState to a value usable in midi

#ifdef USING_HIGH_RES_FADERS  // if def
#ifdef USING_MOTORIZED_FADERS
    highResFader = map(potCState[i], faderMin[i], faderMax[i], 0, 16383);  // converts the 10bit range to 14bit (it will skip some values on 14bit)
    faderMSB = highResFader / 128;                                         // Most Sigficant Byte
    faderLSB = highResFader % 128;                                         // Least Sigficant Byte

#else   // if not USING_MOTORIZED_FADERS

    highResFader = map(potCState[i], potMin, potMax, 0, 16383);  // converts the 10bit range to 14bit (it will skip some values on 14bit)
    faderMSB = highResFader / 128;                               // Most Sigficant Byte
    faderLSB = highResFader % 128;                               // Least Sigficant Byte
#endif  // USING_MOTORIZED_FADERS

#endif  // USING_HIGH_RES_FADERS

    PBVal[i] = map(potCState[i], potMin, potMax, 0, 16383);

#ifdef USING_MACKIE  // if using Remote Script

#ifdef USING_MOTORIZED_FADERS
    PBVal[i] = map(potCState[i], faderMin[i], faderMax[i], 0, 16383);
#else   // if not USING_MOTORIZED_FADERS
    PBVal[i] = map(potCState[i], potMin, potMax, 0, 16383);
#endif  // if not USING_MOTORIZED_FADERS

    if (PBVal[i] < 0) {
      PBVal[i] = 0;
    }
    if (PBVal[i] > 16383) {
      PBVal[i] = 16383;
    }
#endif  // USING_MACKIE


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    potVar = abs(potCState[i] - potPState[i]);  // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) {  // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis();        // Stores the previous time
    }

    timer[i] = millis() - PTime[i];  // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) {  // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    } else {
      potMoving = false;
    }

    if (potMoving == true) {  // If the potentiometer is still moving, send the change control


#ifdef USING_HIGH_RES_FADERS
      if (faderLSB != pFaderLSB) {
#else
      if (potMidiPState[i] != potMidiCState[i]) {
#endif  // USING_HIGH_RES_FADERS

#ifdef USING_MOTORIZED_FADERS
        if (isTouched[i] == true) {
#endif


          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

          // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328
          // use if using with ATmega328 (uno, mega, nano...)

#ifdef USING_MACKIE  // if USING Remote Script protocol
          MIDI.sendPitchBend(PBVal[i], POT_CC_N[i]);

#elif USING_HIGH_RES_FADERS  // if def

          MIDI.sendControlChange(POT_CC_N[i], faderMSB, POT_MIDI_CH);       // MSB
          MIDI.sendControlChange(POT_CC_N[i] + 32, faderLSB, POT_MIDI_CH);  // LSB

#else  // not using Mackie or HighRes

#ifdef USING_CUSTOM_CC_N

          switch (MESSAGE_TYPE_POT[i]) {
            case 1:                                                                // CC
              MIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH);  // CC number, CC value, midi channel - custom cc
              break;
            case 4:  // PB
              MIDI.sendPitchBend(PBVal[i], POT_CC_N[i]);
              break;
          }

#else   // not using custom cc
          MIDI.sendControlChange(CC_NUMBER + i, potMidiCState[i], POT_MIDI_CH);  // CC number, CC value, midi channel
#endif  // USING_CUSTOM_CC_N

#endif  // NOT USING_MACKIE

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif ATMEGA32U4  //use if using with ATmega32U4 (micro, pro micro, leonardo...)

#ifdef MIDI_DIN  // if using MIDI din - uses hardware serial

#ifdef USING_MACKIE  // if USING Remote Script protocol
        // usb
        pitchBend(POT_CC_N[i], PBVal[i]);
        MidiUSB.flush();
        // din
        midi2.sendPitchBend(PBVal[i], POT_CC_N[i] + 1);

#elif USING_HIGH_RES_FADERS  // if def

        // usb
        controlChange(POT_MIDI_CH, CC_NUMBER + i, faderMSB);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        controlChange(POT_MIDI_CH, CC_NUMBER + i + 32, faderLSB);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        // din
        midi2.sendControlChange(POT_CC_N[i], faderMSB, POT_MIDI_CH + 1);       // MSB
        midi2.sendControlChange(POT_CC_N[i] + 32, faderLSB, POT_MIDI_CH + 1);  // LSB

#else  // not using Mackie or HighRes

#ifdef USING_CUSTOM_CC_N
        // usb
        controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        // din
        midi2.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH + 1);  // CC number, CC value, midi channel - custom cc
#else
        // usb
        controlChange(POT_MIDI_CH, CC_NUMBER + i, potMidiCState[i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        // din
        midi2.sendControlChange(CC_NUMBER + i, potMidiCState[i], POT_MIDI_CH + 1);  // CC number, CC value, midi channel
#endif  // USING_CUSTOM_CC_N

#endif  // NOT USING_MACKIE

        // - - - - - - - - - -

        // If using 5-pin MIDI cable at the same time
#else   // ends MIDI DIN

#ifdef USING_MACKIE  // if USING Remote Script protocol
        pitchBend(POT_CC_N[i], PBVal[i]);
        MidiUSB.flush();

#elif USING_HIGH_RES_FADERS  // if def

        controlChange(POT_MIDI_CH, CC_NUMBER + i, faderMSB);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        controlChange(POT_MIDI_CH, CC_NUMBER + i + 32, faderLSB);  //  (channel, CC number,  CC value)
        MidiUSB.flush();

#else  // not using Mackie or HighRes

#ifdef USING_CUSTOM_CC_N

        switch (MESSAGE_TYPE_POT[i]) {
          case 1:                                                       // CC
            controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]);  //  (channel, CC number,  CC value)
            MidiUSB.flush();
            break;
          case 4:  // PB
            pitchBend(POT_CC_N[i] -1, PBVal[i]);
            MidiUSB.flush();
            break;
        }
#else
        controlChange(POT_MIDI_CH, CC_NUMBER + i, potMidiCState[i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
#endif  // USING_CUSTOM_CC_N

#endif  // NOT USING_MACKIE

#endif  // MIDI_DIN


        //#endif //use if using with ATmega32U4 (micro, pro micro, leonardo...)

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif TEENSY
      //do usbMIDI.sendControlChange if using with Teensy

#ifdef USING_CUSTOM_CC_N

      // NOT TESTED
      switch (MESSAGE_TYPE_POT[i]) {
        case 1:                                                                   // CC
          usbMIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH);  // CC number, CC value, midi channel
          break;
        case 4:  // PB
          usbMIDI.sendPitchBend(PBVal[i], POT_CC_N[i]);
          break;
      }

#else
      usbMIDI.sendControlChange(CC_NUMBER + i, potMidiCState[i], POT_MIDI_CH);  // CC number, CC value, midi channel
#endif

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Sends the MIDI CC accordingly to the chosen board
#elif BLEMIDI
      // use if using with BLE MIDI (ESP32)

#ifdef USING_MACKIE  // if USING Remote Script protocol
      BLEMidiServer.pitchBend(POT_CC_N[i], PBVal[i]);  // not tested
                                                       // pitchBend(uint8_t channel, uint16_t value)

#elif USING_HIGH_RES_FADERS  // if def

      BLEMidiServer.controlChange(POT_MIDI_CH, POT_CC_N[i], faderMSB);       // MSB
      BLEMidiServer.controlChange(POT_MIDI_CH, POT_CC_N[i] + 32, faderLSB);  // LSB

#else  // not using Mackie or HighRes

#ifdef USING_CUSTOM_CC_N

      switch (MESSAGE_TYPE_POT[i]) {
        case 1:
          BLEMidiServer.controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]);  // channel, cc number, cc value                                            // CC
          break;
        case 4:                                            // PB
          BLEMidiServer.pitchBend(POT_CC_N[i], PBVal[i]);  // NOT TESTED
          break;
      }

#else  // not using custom cc
      BLEMidiServer.controlChange(POT_MIDI_CH, CC_NUMBER + i, potMidiCState[i]);  // channel, cc number, cc value

#endif  // USING_CUSTOM_CC_N

#endif  // NOT USING_MACKIE

#endif // BOARD

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef DEBUG
      Serial.print("Pot: ");
      Serial.print(i);
      Serial.print("  |  ch: ");
      Serial.print(POT_MIDI_CH);

      switch (MESSAGE_TYPE_POT[i]) {
        case 1:  // CC
          Serial.print("  |  cc: ");
          break;
        case 4:  // PB
          Serial.print("  |  pb: ");
          break;
      }

#ifdef USING_CUSTOM_CC_N
      Serial.print(POT_CC_N[i]);
#else
      Serial.print(CC_NUMBER + i);
#endif

      switch (MESSAGE_TYPE_POT[i]) {
        case 1:  // CC
          Serial.print("  |  value: ");
          Serial.print(potMidiCState[i]);
          Serial.print("  |  Responsive Value: ");
          Serial.print(responsivePot[i].getValue());
          break;
        case 4:  // PB
          Serial.print("  |  Pitch Bend Val: ");
          Serial.print(PBVal[i]);
          Serial.print("  |  Responsive Value: ");
          Serial.print(responsivePot[i].getValue());
          break;
      }



#ifdef USING_HIGH_RES_FADERS  // if def
      Serial.print("  | |  High Res Fader: ");
      Serial.print(highResFader);
      Serial.print("  MSB: ");
      Serial.print(faderMSB);
      Serial.print("  LSB: ");
      Serial.print(faderLSB);
#endif

#ifdef USING_MACKIE  // if def
      Serial.print("  |  Pitch Bend Val: ");
      Serial.print(PBVal[i]);
#endif
      Serial.println();
#endif

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -


          potPState[i] = potCState[i];  // Stores the current reading of the potentiometer to compare with the next
          potMidiPState[i] = potMidiCState[i];

#ifdef USING_HIGH_RES_FADERS  // if def
          pFaderLSB = faderLSB;
#endif

#ifdef USING_MOTORIZED_FADERS
        }
#endif
      }
    }
  }
}




#endif  // USING_POTENTIOMETERS
