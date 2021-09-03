#ifdef USING_BUTTONS


/////////////////////////////////////////////
// variables you don't need to change

#ifdef USING_BANKS_WITH_BUTTONS
int buttonBankCState[2] = {0};
int buttonBankPState[2] = {0};
#endif

#ifdef USING_OCTAVE
int buttonOctaveCState[2] = {0};
int buttonOctavePState[2] = {0};
#endif

int buttonCState[N_BUTTONS] = {0};        // stores the button current value
int buttonPState[N_BUTTONS] = {0};        // stores the button previous value
int octave = 0;

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // the last time the output pin was toggled

// velocity
byte velocity[N_BUTTONS] = {127};
byte toggleVelocity[N_BUTTONS] = {127};


/////////////////////////////////////////////
// Function
void buttons() {

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

        if (buttonCState[i] == LOW) { // if button is pressed

          velocity[i] = 127; // if button is pressed velocity is 127

        }
        else {

          velocity[i] = 0; // if button is released velocity is 0
        }

        switch (MESSAGE_TYPE[i]) {

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          // NN - NOTE NUMBER

          case 0:

#ifdef ATMEGA328
            // if using custom NOTE numbers
            MIDI.sendNoteOn(MESSAGE_VAL[i] + octave, velocity[i], BUTTON_MIDI_CH); // note, velocity, channel

#elif ATMEGA32U4
            noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, velocity[i]);  // channel, note, velocity
            MidiUSB.flush();

#elif TEENSY
            // if using custom NOTE numbers
            usbMIDI.sendNoteOn(MESSAGE_VAL[i] + octave, velocity[i], BUTTON_MIDI_CH); // note, velocity, channel

#elif DEBUG
            Serial.print("Button: ");
            Serial.print(i);
            Serial.print("  | ch: ");
            Serial.print(BUTTON_MIDI_CH);
            Serial.print("  | nn: ");
            Serial.print(MESSAGE_VAL[i] + octave);
            Serial.print("  | velocity: ");
            Serial.println(velocity[i]);
#endif

            break;

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          // CC - CONTROL CHANGE

          case 1:

#ifdef ATMEGA328

            if (buttonCState[i] == LOW) { // only sends note on when button is pressed, nothing when released
              MIDI.sendControlChange(MESSAGE_VAL[i], velocity[i], BUTTON_MIDI_CH); // note, velocity, channel
            }

#elif ATMEGA32U4

            if (velocity[i] > 0) { // only sends note on when button is pressed, nothing when released
              controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i], velocity[i]); //  (channel, CC number,  CC value)
              MidiUSB.flush();
            }

#elif TEENSY

            if (velocity[i] > 0) { // only sends note on when button is pressed, nothing when released
              usbMIDI.sendControlChange(MESSAGE_VAL[i], velocity[i], ); // CC number, CC value, midi channel
            }

#elif DEBUG
            Serial.print("Button: ");
            Serial.print(i);
            Serial.print("  | ch: ");
            Serial.print(BUTTON_MIDI_CH);
            Serial.print("  | cc: ");
            Serial.print(MESSAGE_VAL[i]);
            Serial.print("  | value: ");
            Serial.println(velocity[i]);

#endif

            break;

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
          // T - TOGGLE

          case 2:
            if (buttonCState[i] == LOW) {
              toggleVelocity[i] = !toggleVelocity[i] * 127; // changes the velocity between 0 and 127 each time one press a button

#ifdef ATMEGA328
              // if using custom NOTE numbers
              MIDI.sendNoteOn(MESSAGE_VAL[i] + octave, toggleVelocity[i], BUTTON_MIDI_CH); // note, velocity, channel

#elif ATMEGA32U4
              noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, toggleVelocity[i]);  // channel, note, velocity
              MidiUSB.flush();

#elif TEENSY
              // if using custom NOTE numbers
              usbMIDI.sendNoteOn(MESSAGE_VAL[i] + octave, toggleVelocity[i], BUTTON_MIDI_CH); // note, velocity, channel

#elif DEBUG
              Serial.print("Button: ");
              Serial.print(i);
              Serial.print("  | ch: ");
              Serial.print(BUTTON_MIDI_CH);
              Serial.print("  | nn toggle: ");
              Serial.print(MESSAGE_VAL[i] + octave);
              Serial.print("  | velocity: ");
              Serial.println(toggleVelocity[i]);
#endif

            }

            break;
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

#endif
