#ifdef USING_BUTTONS


/////////////////////////////////////////////
// variables you don't need to change

#ifdef USING_BANKS_WITH_BUTTONS
int buttonBankCState[2] = { 0 };
int buttonBankPState[2] = { 0 };
#endif

#ifdef USING_OCTAVE
int buttonOctaveCState[2] = { 0 };
int buttonOctavePState[2] = { 0 };
#endif

int buttonCState[N_BUTTONS] = { 0 };  // stores the button current value
int buttonPState[N_BUTTONS] = { 0 };  // stores the button previous value
int octave = 0;

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = { 0 };  // the last time the output pin was toggled

// velocity
byte velocity[N_BUTTONS] = { 127 };
byte toggleVelocity[N_BUTTONS] = { 127 };


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
  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO;  // offsets the buttonCState at every mux reading

  // read the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
      // Scale values to 0-1
      if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
        buttonCState[i + nButtonsPerMuxSum] = HIGH;
      } else {
        buttonCState[i + nButtonsPerMuxSum] = LOW;
      }
    }
    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
  }
#endif

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  for (int i = 0; i < N_BUTTONS; i++) {  // Read the buttons connected to the Arduino

#ifdef pin13

    // It will happen if you are using pin 13
    if (i == pin13index) {
      buttonCState[i] = !buttonCState[i];  // inverts the pin 13 because it has a pull down resistor instead of a pull up
    }
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {  // if button is pressed

          velocity[i] = 127;  // if button is pressed velocity is 127

        } else {

          velocity[i] = 0;  // if button is released velocity is 0
        }

        switch (MESSAGE_TYPE[i]) {

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // NN - NOTE NUMBER

          case 0:

#ifdef ATMEGA328
            // if using custom NOTE numbers
            MIDI.sendNoteOn(MESSAGE_VAL[i] + octave, velocity[i], BUTTON_MIDI_CH);  // note, velocity, channel

#elif ATMEGA32U4

#ifdef MIDI_DIN
            // if using midi din cable

            // usb
            noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, velocity[i]);  // channel, note, velocity
            MidiUSB.flush();
            // din
            midi2.sendNoteOn(MESSAGE_VAL[i] + octave, velocity[i], BUTTON_MIDI_CH + 1);  // note, velocity, channel

#else  // not using MIDI_DIN
            noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, velocity[i]);  // channel, note, velocity
            MidiUSB.flush();

#endif  // MIDI_DIN

#elif TEENSY
            // if using custom NOTE numbers
            usbMIDI.sendNoteOn(MESSAGE_VAL[i] + octave, velocity[i], BUTTON_MIDI_CH);  // note, velocity, channel

#elif BLEMIDI
            BLEMidiServer.noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, velocity[i]);          // channel, note, velocity
#endif

#ifdef DEBUG
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
            MIDI.sendControlChange(MESSAGE_VAL[i], velocity[i], BUTTON_MIDI_CH);  // note, velocity, channel

#elif ATMEGA32U4

#ifdef MIDI_DIN
            // if using MIDI din cable

            // usb
            controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i], velocity[i]);  //  (channel, CC number,  CC value)
            MidiUSB.flush();
            // din
            midi2.sendControlChange(MESSAGE_VAL[i], velocity[i], BUTTON_MIDI_CH + 1);  // note, velocity, channel

#else  // no MIDI din
            controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i], velocity[i]);  //  (channel, CC number,  CC value)
            MidiUSB.flush();

#endif  // MIDI_DIN

#elif TEENSY
            usbMIDI.sendControlChange(MESSAGE_VAL[i], velocity[i], BUTTON_MIDI_CH);    // CC number, CC value, midi channel

#elif BLEMIDI
            BLEMidiServer.controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, velocity[i]);   // channel, cc number, cc value

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
              toggleVelocity[i] = !toggleVelocity[i] * 127;  // changes the velocity between 0 and 127 each time one press a button

#ifdef ATMEGA328
              // if using custom NOTE numbers
              MIDI.sendNoteOn(MESSAGE_VAL[i] + octave, toggleVelocity[i], BUTTON_MIDI_CH);  // note, velocity, channel

#elif ATMEGA32U4
              noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, toggleVelocity[i]);      // channel, note, velocity
              MidiUSB.flush();

#elif TEENSY
              // if using custom NOTE numbers
              usbMIDI.sendNoteOn(MESSAGE_VAL[i] + octave, toggleVelocity[i], BUTTON_MIDI_CH);  // note, velocity, channel

#elif BLEMIDI
              BLEMidiServer.noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, toggleVelocity[i]);  // channel, note, velocity

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

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // PC - PROGRAM CHANGE

          case 3:

#ifdef ATMEGA328
            // if using custom NOTE numbers
            MIDI.sendProgramChange(MESSAGE_VAL[i], BUTTON_MIDI_CH);

#elif ATMEGA32U4

#ifdef MIDI_DIN
            // if using midi din cable

            // usb
            programChange(BUTTON_MIDI_CH, MESSAGE_VAL[i]);
            MidiUSB.flush();
            // din
            midi2.sendProgramChange(MESSAGE_VAL[i], BUTTON_MIDI_CH);

#else  // not using MIDI_DIN
            programChange(BUTTON_MIDI_CH, MESSAGE_VAL[i]);
            MidiUSB.flush();

#endif  // MIDI_DIN

#elif TEENSY
            // if using custom NOTE numbers
            usbMIDI.sendProgramChange(MESSAGE_VAL[i], BUTTON_MIDI_CH);

#elif BLEMIDI

            BLEMidiServer.programChange(BUTTON_MIDI_CH, MESSAGE_VAL[i]);

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
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

#endif
