#ifdef USING_POTENTIOMETERS

//==============================================================================
//  Global Variables and Constants (Do not change)
//==============================================================================
int reading = 0;  // Temporary reading holder

// Responsive Analog Read
float snapMultiplier = 0.01;                      // (0.0 - 1.0) Increase for faster but less smooth reading
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // Array for the responsive pots (filled in setup)

// Potentiometer States
int potCState[N_POTS] = { 0 };      // Current state of the pot
int potPState[N_POTS] = { 0 };      // Previous state of the pot
int potVar = 0;                   // Difference between current and previous state

// MIDI Mapped States
int potMidiCState[N_POTS] = { 0 };  // Current MIDI value
int potMidiPState[N_POTS] = { 0 };  // Previous MIDI value

#ifdef USING_HIGH_RES_FADERS
  // High Resolution Faders (14-bit resolution)
  unsigned int highResFader = 0;  // Stores the high-res value
  byte faderMSB = 0;              // Most Significant Byte
  byte faderLSB = 0;              // Least Significant Byte
  byte pFaderLSB = 0;             // Previous Least Significant Byte
#endif

int PBVal[N_POTS] = { 0 };        // Pitch Bend or Mackie values

// Timing and Movement
boolean potMoving = true;             // True if the potentiometer is moving
unsigned long PTime[N_POTS] = { 0 };  // Time stamp when movement was detected
unsigned long timer[N_POTS] = { 0 };  // Elapsed time since last change

//==============================================================================
//  Function Prototypes
//==============================================================================
#ifdef USING_MUX
  void readMuxPotentiometers();
#endif
void processPotentiometer(int i);
void sendMidiForPot(int i);

// Main processing function for all potentiometers
void potentiometers();

//==============================================================================
//  Function Implementations
//==============================================================================

//------------------------------------------------------------------------------
// Reads the analog inputs from Arduino (and from the MUX if defined)
//------------------------------------------------------------------------------
void potentiometers() {
  // Read analog values directly from Arduino pins
  for (int i = 0; i < N_POTS_ARDUINO; i++) {
    reading = analogRead(POT_ARDUINO_PIN[i]);
    responsivePot[i].update(reading);
    potCState[i] = responsivePot[i].getValue();
  }

  #ifdef USING_MUX
    readMuxPotentiometers();
  #endif

  // Process each potentiometer after reading all values
  for (int i = 0; i < N_POTS; i++) {
    processPotentiometer(i);
  }
}

#ifdef USING_MUX
//------------------------------------------------------------------------------
// Reads analog values from multiplexer channels and updates the pot states
//------------------------------------------------------------------------------
void readMuxPotentiometers() {
  int nPotsPerMuxSum = N_POTS_ARDUINO;  // Offset for mux readings
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_POTS_PER_MUX[j]; i++) {
      reading = mux[j]->readChannel(POT_MUX_PIN[j][i]);
      responsivePot[i + nPotsPerMuxSum].update(reading);
      potCState[i + nPotsPerMuxSum] = responsivePot[i + nPotsPerMuxSum].getValue();
    }
    nPotsPerMuxSum += N_POTS_PER_MUX[j];
  }
}
#endif

//------------------------------------------------------------------------------
// Processes a single potentiometer: mapping, clipping, debouncing, and MIDI check
//------------------------------------------------------------------------------
void processPotentiometer(int i) {
  // Map and clip the analog value according to whether we use motorized faders
  #ifdef USING_MOTORIZED_FADERS
    potCState[i] = clipValue(potCState[i], faderMin[i], faderMax[i]);
    potMidiCState[i] = map(potCState[i], faderMin[i], faderMax[i], 0, 127);
    potMidiCState[i] = clipValue(potMidiCState[i], 0, 127);
  #else
    potCState[i] = clipValue(potCState[i], potMin, potMax);
    potMidiCState[i] = map(potCState[i], potMin, potMax, 0, 127);
    potMidiCState[i] = clipValue(potMidiCState[i], 0, 127);
  #endif

  // High resolution fader mapping if enabled
  #ifdef USING_HIGH_RES_FADERS
    #ifdef USING_MOTORIZED_FADERS
      highResFader = map(potCState[i], faderMin[i], faderMax[i], 0, 16383);
    #else
      highResFader = map(potCState[i], potMin, potMax, 0, 16383);
    #endif
    faderMSB = highResFader / 128;
    faderLSB = highResFader % 128;
  #endif

  // Map for pitch bend or Mackie
  PBVal[i] = map(potCState[i], potMin, potMax, 0, 16383);
  #ifdef USING_MACKIE
    #ifdef USING_MOTORIZED_FADERS
      PBVal[i] = map(potCState[i], faderMin[i], faderMax[i], 0, 16383);
    #else
      PBVal[i] = map(potCState[i], potMin, potMax, 0, 16383);
    #endif
    if (PBVal[i] < 0) { PBVal[i] = 0; }
    if (PBVal[i] > 16383) { PBVal[i] = 16383; }
  #endif

  // Calculate the variation between current and previous potentiometer values
  potVar = abs(potCState[i] - potPState[i]);
  if (potVar > varThreshold) {
    PTime[i] = millis();
  }
  timer[i] = millis() - PTime[i];
  potMoving = (timer[i] < TIMEOUT);

  // If the pot is moving and the value has changed (using either MIDI or high-res criteria)
  #ifdef USING_HIGH_RES_FADERS
    if (potMoving && (faderLSB != pFaderLSB)) {
  #else
    if (potMoving && (potMidiPState[i] != potMidiCState[i])) {
  #endif
      #ifdef USING_MOTORIZED_FADERS
        if (isTouched[i] == true) {
      #endif

          // Send the corresponding MIDI message
          sendMidiForPot(i);

      #ifdef USING_MOTORIZED_FADERS
        }
      #endif

      #ifdef DEBUG
        Serial.print("Pot: ");
        Serial.print(i);
        Serial.print("  |  ch: ");
        Serial.print(POT_MIDI_CH);
        switch (MESSAGE_TYPE_POT[i]) {
          case 1:
            Serial.print("  |  cc: ");
            break;
          case 4:
            Serial.print("  |  pb: ");
            break;
        }
        #ifdef USING_CUSTOM_CC_N
          Serial.print(POT_CC_N[i]);
        #else
          Serial.print(CC_NUMBER + i);
        #endif
        switch (MESSAGE_TYPE_POT[i]) {
          case 1:
            Serial.print("  |  value: ");
            Serial.print(potMidiCState[i]);
            Serial.print("  |  Responsive Value: ");
            Serial.print(responsivePot[i].getValue());
            break;
          case 4:
            Serial.print("  |  Pitch Bend Val: ");
            Serial.print(PBVal[i]);
            Serial.print("  |  Responsive Value: ");
            Serial.print(responsivePot[i].getValue());
            break;
        }
        #ifdef USING_HIGH_RES_FADERS
          Serial.print("  | |  High Res Fader: ");
          Serial.print(highResFader);
          Serial.print("  MSB: ");
          Serial.print(faderMSB);
          Serial.print("  LSB: ");
          Serial.print(faderLSB);
        #endif
        #ifdef USING_MACKIE
          Serial.print("  |  Pitch Bend Val: ");
          Serial.print(PBVal[i]);
        #endif
        Serial.println();
      #endif

      // Save current values for the next comparison
      potPState[i] = potCState[i];
      potMidiPState[i] = potMidiCState[i];
      #ifdef USING_HIGH_RES_FADERS
        pFaderLSB = faderLSB;
      #endif
    }
}

  
//------------------------------------------------------------------------------
// Sends the MIDI message for the potentiometer based on board/protocol definitions
//------------------------------------------------------------------------------
void sendMidiForPot(int i) {
  #if defined(ATMEGA328)
    #ifdef USING_MACKIE
      MIDI.sendPitchBend(PBVal[i], POT_CC_N[i]);
    #elif defined(USING_HIGH_RES_FADERS)
      MIDI.sendControlChange(POT_CC_N[i], faderMSB, POT_MIDI_CH);
      MIDI.sendControlChange(POT_CC_N[i] + 32, faderLSB, POT_MIDI_CH);
    #else
      #ifdef USING_CUSTOM_CC_N
        switch (MESSAGE_TYPE_POT[i]) {
          case 1:
            MIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH);
            break;
          case 4:
            MIDI.sendPitchBend(PBVal[i], POT_CC_N[i]);
            break;
        }
      #else
        MIDI.sendControlChange(CC_NUMBER + i, potMidiCState[i], POT_MIDI_CH);
      #endif
    #endif

  #elif defined(ATMEGA32U4)
    #ifdef MIDI_DIN
      #ifdef USING_MACKIE
        pitchBend(POT_CC_N[i], PBVal[i]);
        MidiUSB.flush();
        midi2.sendPitchBend(PBVal[i], POT_CC_N[i] + 1);
      #elif defined(USING_HIGH_RES_FADERS)
        controlChange(POT_MIDI_CH, CC_NUMBER + i, faderMSB);
        MidiUSB.flush();
        controlChange(POT_MIDI_CH, CC_NUMBER + i + 32, faderLSB);
        MidiUSB.flush();
        midi2.sendControlChange(POT_CC_N[i], faderMSB, POT_MIDI_CH + 1);
        midi2.sendControlChange(POT_CC_N[i] + 32, faderLSB, POT_MIDI_CH + 1);
      #else
        #ifdef USING_CUSTOM_CC_N
          controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]);
          MidiUSB.flush();
          midi2.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH + 1);
        #else
          controlChange(POT_MIDI_CH, CC_NUMBER + i, potMidiCState[i]);
          MidiUSB.flush();
          midi2.sendControlChange(CC_NUMBER + i, potMidiCState[i], POT_MIDI_CH + 1);
        #endif
      #endif
    #else
      #ifdef USING_MACKIE
        pitchBend(POT_CC_N[i], PBVal[i]);
        MidiUSB.flush();
      #elif defined(USING_HIGH_RES_FADERS)
        controlChange(POT_MIDI_CH, CC_NUMBER + i, faderMSB);
        MidiUSB.flush();
        controlChange(POT_MIDI_CH, CC_NUMBER + i + 32, faderLSB);
        MidiUSB.flush();
      #else
        #ifdef USING_CUSTOM_CC_N
          switch (MESSAGE_TYPE_POT[i]) {
            case 1:
              controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]);
              MidiUSB.flush();
              break;
            case 4:
              pitchBend(POT_CC_N[i] - 1, PBVal[i]);
              MidiUSB.flush();
              break;
          }
        #else
          controlChange(POT_MIDI_CH, CC_NUMBER + i, potMidiCState[i]);
          MidiUSB.flush();
        #endif
      #endif
    #endif

  #elif defined(TEENSY)
    #ifdef USING_CUSTOM_CC_N
      switch (MESSAGE_TYPE_POT[i]) {
        case 1:
          usbMIDI.sendControlChange(POT_CC_N[i], potMidiCState[i], POT_MIDI_CH);
          break;
        case 4:
          usbMIDI.sendPitchBend(PBVal[i], POT_CC_N[i]);
          break;
      }
    #else
      usbMIDI.sendControlChange(CC_NUMBER + i, potMidiCState[i], POT_MIDI_CH);
    #endif

  #elif defined(BLEMIDI)
    #ifdef USING_MACKIE
      BLEMidiServer.pitchBend(POT_CC_N[i], PBVal[i]);
    #elif defined(USING_HIGH_RES_FADERS)
      BLEMidiServer.controlChange(POT_MIDI_CH, POT_CC_N[i], faderMSB);
      BLEMidiServer.controlChange(POT_MIDI_CH, POT_CC_N[i] + 32, faderLSB);
    #else
      #ifdef USING_CUSTOM_CC_N
        switch (MESSAGE_TYPE_POT[i]) {
          case 1:
            BLEMidiServer.controlChange(POT_MIDI_CH, POT_CC_N[i], potMidiCState[i]);
            break;
          case 4:
            BLEMidiServer.pitchBend(POT_CC_N[i], PBVal[i]);
            break;
        }
      #else
        BLEMidiServer.controlChange(POT_MIDI_CH, CC_NUMBER + i, potMidiCState[i]);
      #endif
    #endif
  #endif
}

#endif  // USING_POTENTIOMETERS
