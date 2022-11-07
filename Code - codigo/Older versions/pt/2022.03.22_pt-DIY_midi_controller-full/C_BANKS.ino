#ifdef USING_BANKS_WITH_BUTTONS

/////////////////////////////////////////////
// BUTTONS
void banksWithButtons() {

  // read pins 
  for (int i = 0; i < 2; i++) {

#ifdef USING_MUX_BANK_BUTTON_PIN // read the mux pins 

    // It will happen if you are using MUX
    // read the pins from every mux
    for (int i = 0; i < 2; i++) {
      buttonBankCState[i] = mux[0].readChannel(BANK_BUTTON_PIN[i]);
      // Scale values to 0-1
      if (buttonBankCState[i] > buttonMuxThreshold) {
        buttonBankCState[i] = HIGH;
      }
      else {
        buttonBankCState[i] = LOW;
      }
    }

#else // USING_MUX_BANK_BUTTON_PIN
// read the Aruino pins
    buttonBankCState[i] = digitalRead(BANK_BUTTON_PIN[i]);

#endif
  }




  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  for (int i = 0; i < 2; i++) { // Read the button state and change the MIDI channel
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonBankPState[i] != buttonBankCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonBankCState[i] == LOW) {

          if (i == 0) { // button left

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            //#ifdef USING_MACKIE
#ifdef USING_MACKIE

#ifdef ATMEGA32U4
            noteOn(0, 46, 127);  // channel, note, velocity
            MidiUSB.flush();
            noteOn(0, 46, 0);  // channel, note, velocity
            MidiUSB.flush();
#endif //ATMEGA32U4

#else // if NOT USING_MACKIE

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef BANKS_FOR_BUTTONS
            BUTTON_MIDI_CH--;
#endif

#ifdef BANKS_FOR_POTS
            POT_MIDI_CH--;
#endif

#ifdef BANKS_FOR_ENCODERS
            ENCODER_MIDI_CH--;
#endif

#endif // USING_MACKIE

          }
          else if (i == 1) { // button right

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifdef USING_MACKIE

#ifdef ATMEGA32U4
            noteOn(0, 47, 127);  // channel, note, velocity
            MidiUSB.flush();
            noteOn(0, 47, 0);  // channel, note, velocity
            MidiUSB.flush();
#endif //ATMEGA32U4

#else // if NOT USING_MACKIE
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef BANKS_FOR_BUTTONS
            BUTTON_MIDI_CH++;
#endif

#ifdef BANKS_FOR_POTS
            POT_MIDI_CH++;
#endif

#ifdef BANKS_FOR_ENCODERS
            ENCODER_MIDI_CH++;
#endif

#endif // USING_MACKIE

          }

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef ATMEGA32U4
          // limits the value to a 16 range
          BUTTON_MIDI_CH %= 16;
          POT_MIDI_CH %= 16;
          ENCODER_MIDI_CH %= 16;
#endif

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef ATMEGA328
          // limits the value to a 16 range
          //int clipValue(int in, int minVal, int maxVal) {
          BUTTON_MIDI_CH = clipValue(BUTTON_MIDI_CH, 1, 16);
          POT_MIDI_CH = clipValue(POT_MIDI_CH, 1, 16);
          ENCODER_MIDI_CH = clipValue(ENCODER_MIDI_CH, 1, 16);
#endif


          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_DISPLAY

#ifdef BANKS_FOR_BUTTONS
          printChannel(BUTTON_MIDI_CH); // prints button midi channel in the oled display
#endif

#ifdef BANKS_FOR_POTS
          printChannel(POT_MIDI_CH); // prints button midi channel in the oled display
#endif

#ifdef BANKS_FOR_ENCODERS
          printChannel(ENCODER_MIDI_CH); // prints button midi channel in the oled display
#endif

#endif // USING_DISPLAY


          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MOTORIZED_FADERS
#ifdef BANKS_FOR_POTS // writes the value pre stored in the encoder

          for (int i = 0; i < N_POTS; i++) {

            faderPos[i] = map(pFaderInVal[POT_MIDI_CH][i], 0, 127, faderMin[i], faderMax[i]);
          }
#endif
#endif

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef DEBUG

          BUTTON_MIDI_CH %= 16;
          POT_MIDI_CH %= 16;
          ENCODER_MIDI_CH %= 16;

#ifdef BANKS_FOR_BUTTONS
          Serial.print("Button MIDI ch: ");
          Serial.println(BUTTON_MIDI_CH);
#endif

#ifdef BANKS_FOR_POTS
          Serial.print("Pot MIDI ch: ");
          Serial.println(POT_MIDI_CH);
#endif

#ifdef BANKS_FOR_ENCODERS
          Serial.print("Encoder MIDI ch: ");
          Serial.println(ENCODER_MIDI_CH);
#endif

#endif //DEBUG

#ifdef USING_74HC595
          ShiftPWM.SetAll(0);
#endif

        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_ENCODER
#ifdef BANKS_FOR_ENCODERS // writes the value pre stored in the encoder 
        for (int j = 0; j < N_ENCODERS; j++) {
          encoder[j].write(encoderValue[ENCODER_MIDI_CH][j]);
        }
#endif
#endif




        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        buttonBankPState[i] = buttonBankCState[i];
      }
    }
  }
}

#endif
