#ifdef USING_BUTTONS

// Define a structure to encapsulate the state of each button
struct Button {
  int currentState;         // Current reading (HIGH or LOW)
  int previousState;        // Previous reading
  unsigned long lastDebounceTime; // Timestamp of last state change for debounce
  byte velocity;            // Velocity value (0 or 127)
  byte toggleVelocity;      // For toggle messages (0 or 127)
};

// Create an array of button states for all buttons
Button buttonsState[N_BUTTONS] = {};

// Other global variables that may be used in button handling
#ifdef USING_BANKS_WITH_BUTTONS
int buttonBankCState[2] = { 0 };
int buttonBankPState[2] = { 0 };
#endif

#ifdef USING_OCTAVE
int buttonOctaveCState[2] = { 0 };
int buttonOctavePState[2] = { 0 };
#endif

int octave = 0; // Global octave offset

// Helper function prototypes
void readArduinoButtons();
#ifdef USING_MUX
void readMuxButtons();
#endif
void processButton(int i);
void sendMidiMessage(int i);

//------------------------------------------------------------------
// Main button processing function.
// Reads the current button states, applies debouncing,
// and sends the corresponding MIDI messages if a state change is detected.
void buttons() {

  // Read button states connected directly to the Arduino
  readArduinoButtons();

  // If using multiplexers, update additional button states
#ifdef USING_MUX
  readMuxButtons();
#endif

  // Process each button
  for (int i = 0; i < N_BUTTONS; i++) {

    // Debounce: process the button only if enough time has passed since the last change
    if ((millis() - buttonsState[i].lastDebounceTime) > debounceDelay) {
      if (buttonsState[i].previousState != buttonsState[i].currentState) {
        // Update the debounce timer
        buttonsState[i].lastDebounceTime = millis();

        // Set velocity based on the button state: pressed (LOW) equals full velocity
        buttonsState[i].velocity = (buttonsState[i].currentState == LOW) ? 127 : 0;
        
        // Process the button event (sends a MIDI message based on its type)
        processButton(i);
        
        // Update the previous state for future debounce comparisons
        buttonsState[i].previousState = buttonsState[i].currentState;
      }
    }
  }
}

//------------------------------------------------------------------
// Reads button states from Arduino digital pins.
void readArduinoButtons() {
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonsState[i].currentState = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }
}

#ifdef USING_MUX
//------------------------------------------------------------------
// Reads button states from multiplexers and updates the buttonsState array.
void readMuxButtons() {
  int offset = N_BUTTONS_ARDUINO; // Start index for mux buttons in the buttonsState array
  for (int m = 0; m < N_MUX; m++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[m]; i++) {
      int state = mux[m]->readChannel(BUTTON_MUX_PIN[m][i]);
      // Normalize the state to HIGH or LOW based on a threshold
      buttonsState[i + offset].currentState = (state > buttonMuxThreshold) ? HIGH : LOW;
    }
    offset += N_BUTTONS_PER_MUX[m];
  }
}
#endif

//------------------------------------------------------------------
// Processes a single button press based on its assigned MIDI message type.
// MESSAGE_TYPE and MESSAGE_VAL are assumed to be global arrays defining the behavior.
void processButton(int i) {
  switch (MESSAGE_TYPE[i]) {
    case 0: // Note Message
#ifdef ATMEGA328
      MIDI.sendNoteOn(MESSAGE_VAL[i] + octave, buttonsState[i].velocity, BUTTON_MIDI_CH);
#elif defined(ATMEGA32U4)
  #ifdef MIDI_DIN
      noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, buttonsState[i].velocity);
      MidiUSB.flush();
      midi2.sendNoteOn(MESSAGE_VAL[i] + octave, buttonsState[i].velocity, BUTTON_MIDI_CH + 1);
  #else
      noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, buttonsState[i].velocity);
      MidiUSB.flush();
  #endif
#elif defined(TEENSY)
      usbMIDI.sendNoteOn(MESSAGE_VAL[i] + octave, buttonsState[i].velocity, BUTTON_MIDI_CH);
#elif defined(BLEMIDI)
      BLEMidiServer.noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, buttonsState[i].velocity);
#endif
#ifdef DEBUG
      Serial.print("Button: "); Serial.print(i);
      Serial.print(" | ch: "); Serial.print(BUTTON_MIDI_CH);
      Serial.print(" | note: "); Serial.print(MESSAGE_VAL[i] + octave);
      Serial.print(" | velocity: "); Serial.println(buttonsState[i].velocity);
#endif
      break;

    case 1: // Control Change
#ifdef ATMEGA328
      MIDI.sendControlChange(MESSAGE_VAL[i], buttonsState[i].velocity, BUTTON_MIDI_CH);
#elif defined(ATMEGA32U4)
  #ifdef MIDI_DIN
      controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i], buttonsState[i].velocity);
      MidiUSB.flush();
      midi2.sendControlChange(MESSAGE_VAL[i], buttonsState[i].velocity, BUTTON_MIDI_CH + 1);
  #else
      controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i], buttonsState[i].velocity);
      MidiUSB.flush();
  #endif
#elif defined(TEENSY)
      usbMIDI.sendControlChange(MESSAGE_VAL[i], buttonsState[i].velocity, BUTTON_MIDI_CH);
#elif defined(BLEMIDI)
      BLEMidiServer.controlChange(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, buttonsState[i].velocity);
#endif
#ifdef DEBUG
      Serial.print("Button: "); Serial.print(i);
      Serial.print(" | ch: "); Serial.print(BUTTON_MIDI_CH);
      Serial.print(" | cc: "); Serial.print(MESSAGE_VAL[i]);
      Serial.print(" | value: "); Serial.println(buttonsState[i].velocity);
#endif
      break;

    case 2: // Toggle Message
      // Toggle the velocity between 0 and 127 on each press.
      buttonsState[i].toggleVelocity = (buttonsState[i].toggleVelocity == 0) ? 127 : 0;
#ifdef ATMEGA328
      MIDI.sendNoteOn(MESSAGE_VAL[i] + octave, buttonsState[i].toggleVelocity, BUTTON_MIDI_CH);
#elif defined(ATMEGA32U4)
      noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, buttonsState[i].toggleVelocity);
      MidiUSB.flush();
#elif defined(TEENSY)
      usbMIDI.sendNoteOn(MESSAGE_VAL[i] + octave, buttonsState[i].toggleVelocity, BUTTON_MIDI_CH);
#elif defined(BLEMIDI)
      BLEMidiServer.noteOn(BUTTON_MIDI_CH, MESSAGE_VAL[i] + octave, buttonsState[i].toggleVelocity);
#endif
#ifdef DEBUG
      Serial.print("Button (Toggle): "); Serial.print(i);
      Serial.print(" | ch: "); Serial.print(BUTTON_MIDI_CH);
      Serial.print(" | note: "); Serial.print(MESSAGE_VAL[i] + octave);
      Serial.print(" | toggle velocity: "); Serial.println(buttonsState[i].toggleVelocity);
#endif
      break;

    case 3: // Program Change
#ifdef ATMEGA328
      MIDI.sendProgramChange(MESSAGE_VAL[i], BUTTON_MIDI_CH);
#elif defined(ATMEGA32U4)
  #ifdef MIDI_DIN
      programChange(BUTTON_MIDI_CH, MESSAGE_VAL[i]);
      MidiUSB.flush();
      midi2.sendProgramChange(MESSAGE_VAL[i], BUTTON_MIDI_CH);
  #else
      programChange(BUTTON_MIDI_CH, MESSAGE_VAL[i]);
      MidiUSB.flush();
  #endif
#elif defined(TEENSY)
      usbMIDI.sendProgramChange(MESSAGE_VAL[i], BUTTON_MIDI_CH);
#elif defined(BLEMIDI)
      BLEMidiServer.programChange(BUTTON_MIDI_CH, MESSAGE_VAL[i]);
#endif
#ifdef DEBUG
      Serial.print("Button (Prog): "); Serial.print(i);
      Serial.print(" | ch: "); Serial.print(BUTTON_MIDI_CH);
      Serial.print(" | program: "); Serial.println(MESSAGE_VAL[i]);
#endif
      break;
  }
}

#endif  // USING_BUTTONS
