#ifdef USING_OCTAVE

/////////////////////////////////////////////
// BUTTONS
void changeOctave() {

  // read pins from arduino
  for (int i = 0; i < 2; i++) {

#ifdef USING_MUX_OCTAVE_PIN // read the mux pins 

    // It will happen if you are using MUX

    buttonOctaveCState[i] = mux[0].readChannel(OCTAVE_BUTTON_PIN[i]);

    // Scale values to 0-1
    if (buttonOctaveCState[i] > buttonMuxThreshold) {
      buttonOctaveCState[i] = HIGH;
    }
    else {
      buttonOctaveCState[i] = LOW;
    }

#else // USING_MUX_OCTAVE_PIN
    // read the Aruino pins
    buttonOctaveCState[i] = digitalRead(OCTAVE_BUTTON_PIN[i]);

#endif

  }




  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  for (int i = 0; i < 2; i++) { // Read the button state and change the MIDI channel

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonOctavePState[i] != buttonOctaveCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonOctaveCState[i] == LOW) {

          if (i == 0) { // button left

            octave -= 12; // octave = octave + 12

          }
          else if (i == 1) { // button right

            octave += 12; // octave = octave - 12

          }

#ifdef USING_DISPLAY
          printOctave(octave);
#endif

#ifdef DEBUG
          Serial.print("Octave = ");
          Serial.println(octave);
#endif

        }
        buttonOctavePState[i] = buttonOctaveCState[i];
      }
    }
  }
}

#endif
