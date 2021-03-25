#ifdef USING_BUTTONS

/////////////////////////////////////////////
// BUTTONS
void buttons() {

  // read pins from arduino
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }

#ifdef USING_MUX

  // It will happen if you are using MUX
  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // offsets the buttonCState at every mux reading

  // read the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
      // Scale values to 0-1
      if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
        buttonCState[i + nButtonsPerMuxSum] = HIGH;
      }
      else {
        buttonCState[i + nButtonsPerMuxSum] = LOW;
      }
    }
    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
  }
#endif

  for (int i = 0; i < N_BUTTONS; i++) { // Read the buttons connected to the Arduino

#ifdef pin13

    // It will happen if you are using pin 13
    if (i == pin13index) {
      buttonCState[i] = !buttonCState[i]; // inverts the pin 13 because it has a pull down resistor instead of a pull up
    }
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {

#ifndef USING_TOGGLE // if NOT using button toggle mode

          velocity[i] = 127; // if button is pressed velocity is 127
#else

          velocity[i] = !velocity[i] * 127; // changes the velocity between 0 and 127 each time one press a button
#endif

        }
        else {

#ifndef USING_TOGGLE // if NOT using button toggle mode

          velocity[i] = 0; // if button is released velocity is 0
#endif
        }


        // Sends the MIDI NOTE ON accordingly to the chosen board

#ifdef USING_TOGGLE

        if (buttonCState[i] == LOW) { // only when button is pressed
#endif

          /////////////////////////////////////////////
#ifdef ATMEGA328

          // it will happen if using with ATmega328 (uno, mega, nano...)

#ifndef USING_BUTTON_CC_N // if NOT using button CC

#ifdef USING_CUSTOM_NN          

          // if using custom NOTE numbers
          MIDI.sendNoteOn(BUTTON_NN[i], velocity[i], BUTTON_MIDI_CH + 1); // note, velocity, channel
#else

          // if not using custom NOTE numbers
          MIDI.sendNoteOn(NOTE + i, velocity[i], BUTTON_MIDI_CH + 1); // note, velocity, channel
#endif

#else // if USING button CC

          if (buttonCState[i] == LOW) { // only sends note on when button is pressed, nothing when released
            MIDI.sendControlChange(BUTTON_CC_N[i], velocity[i], BUTTON_MIDI_CH + 1); // note, velocity, channel
          }
#endif // ATMEGA328

          /////////////////////////////////////////////
#elif ATMEGA32U4

          // it will happen if using with ATmega32U4 (micro, pro micro, leonardo...)

#ifndef USING_BUTTON_CC_N // if NOT using button CC

#ifdef USING_CUSTOM_NN

          // if using custom NOTE numbers

          noteOn(BUTTON_MIDI_CH, BUTTON_NN[i], velocity[i]);  // channel, note, velocity
          MidiUSB.flush();
#else

          // if not using custom NOTE
          noteOn(BUTTON_MIDI_CH, NOTE + i, velocity[i]);  // channel, note, velocity
          MidiUSB.flush();
#endif

#else // if USING button CC

          if (velocity[i] > 0) { // only sends note on when button is pressed, nothing when released
            controlChange(BUTTON_MIDI_CH, BUTTON_CC_N[i], velocity[i]); //  (channel, CC number,  CC value)
            MidiUSB.flush();
          }
#endif


          /////////////////////////////////////////////
#elif TEENSY
          //do usbMIDI.sendNoteOn if using with Teensy

#ifndef USING_BUTTON_CC_N // if NOT using button CC

#ifdef USING_CUSTOM_NN

          // if using custom NOTE numbers
          usbMIDI.sendNoteOn(BUTTON_NN[i], velocity[i], BUTTON_MIDI_CH); // note, velocity, channel
#else

          // if not using custom NOTE
          usbMIDI.sendNoteOn(NOTE + i, velocity[i], BUTTON_MIDI_CH); // note, velocity, channel
#endif

#else // if USING button CC

          if (velocity[i] > 0) { // only sends note on when button is pressed, nothing when released
            usbMIDI.sendControlChange(BUTTON_CC_N[i], velocity[i], BUTTON_MIDI_CH); // CC number, CC value, midi channel
          }
#endif

          /////////////////////////////////////////////
#elif DEBUG

#ifndef USING_BUTTON_CC_N // print if not using button cc number

          Serial.print("Button: ");
          Serial.print(i);
          Serial.print("  | ch: ");
          Serial.print(BUTTON_MIDI_CH);
          Serial.print("  | nn: ");

#ifdef USING_CUSTOM_NN

          Serial.print(BUTTON_NN[i]);
#else

          Serial.print(NOTE + i);
#endif

          Serial.print("  | velocity: ");
          Serial.println(velocity[i]);
#else

          Serial.print("Button: ");
          Serial.print(i);
          Serial.print("  | ch: ");
          Serial.print(BUTTON_MIDI_CH);
          Serial.print("  | cc: ");
          Serial.print(BUTTON_CC_N[i]);
          Serial.print("  | value: ");
          Serial.println(velocity[i]);
#endif

#endif

          /////////////////////////////////////////////

#ifdef USING_TOGGLE

        }
#endif

        buttonPState[i] = buttonCState[i];
      }

    }
  }
}

#endif
