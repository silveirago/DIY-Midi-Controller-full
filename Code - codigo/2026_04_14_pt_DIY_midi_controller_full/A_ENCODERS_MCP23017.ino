#ifdef USING_ENCODER_MCP23017

int lastEncoderValue_MCP23017[N_ENC_CH_MCP23017][N_ENC_MCP23017] = { 127 };
int lastEncoderMidiValue_MCP23017[N_ENC_CH_MCP23017][N_ENC_MCP23017] = { 127 };
int encoderValue_MCP23017[N_ENC_CH_MCP23017][N_ENC_MCP23017] = { 127 };
int encoderMidiValue_MCP23017[N_ENC_CH_MCP23017][N_ENC_MCP23017] = { 127 };
int encoderMackieValue_MCP23017[N_ENC_CH_MCP23017][N_ENC_MCP23017] = { 127 };
int lastencoderMidiValue_MCP23017[N_ENC_CH_MCP23017][N_ENC_MCP23017] = { 127 };

// for the encoder Channels - Used for different banks

//byte lastEncoderChannel = 0;

unsigned long encPTime_MCP23017[N_ENC_MCP23017] = { 0 };
unsigned long encTimer_MCP23017[N_ENC_MCP23017] = { 0 };
boolean encMoving_MCP23017[N_ENC_MCP23017] = { false };
boolean encMoved_MCP23017[N_ENC_MCP23017] = { false };
byte encTIMEOUT_MCP23017 = 50;
byte encoder_n_MCP23017;
float encTempVal_MCP23017 = 0;

byte encIndex;


#ifdef USING_HIGH_RES_ENC
// Use this to send CC using 2 bytes (14 bit res)
unsigned int highResenc_MCP23017[N_ENCODER_MIDI_CHANNELS][N_ENC_MCP23017] = { 0 };  // stores the high res val
byte encMSB_MCP23017[N_ENCODER_MIDI_CHANNELS][N_ENC_MCP23017] = { 0 };              // Most Significant Byte
byte encLSB_MCP23017[N_ENCODER_MIDI_CHANNELS][N_ENC_MCP23017] = { 0 };              // Least Significant Byte
byte pencLSB_MCP23017[N_ENCODER_MIDI_CHANNELS][N_ENC_MCP23017] = { 0 };             // Previous Most Significant Byte
#endif


void readEncoder_MCP23017() {

  byte interruptPin = mcp.getLastInterruptPin();

  updateEncIndex(interruptPin); // finds the encoder index
  
  updateEncoder(encIndex); // reads/updates the encoder

  mcp.clearInterrupts();  // clear th interrupt
}


// - - - - - - - - - - - - - - - - - - - -

void updateEncoder(int encIndex) {


  if (encIndex >= 0 && encIndex < N_ENC_MCP23017) {

    int encoderA = mcp.digitalRead(encoderPin[encIndex][0]);  // Read the state of the A channel of the current encoder
    int encoderB = mcp.digitalRead(encoderPin[encIndex][1]);  // Read the state of the B channel of the current encoder

    //  Serial.print(encoderA);
    //  Serial.print(encoderB);

    if (encoderA != lastEncoderA[encIndex]) {  // Check if the state of the A channel has changed
      if (encoderA == LOW) {                   // If the state of the A channel is LOW
        if (encoderB == LOW) {                 // If the state of the B channel is also LOW

          encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex]--;  // decreaments the value

        } else {

          encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex]++;  // increaments the value
        }
      }
      lastEncoderA[encIndex] = encoderA;  // Update the previous state of the A channel
    }



    // - - - - - - - - - - - - - - - - -
    // Encoder clipping and sensitivity

    // Clip encoder Value
#if !defined(TRAKTOR) && !defined(USING_MACKIE)  // if not defined traktor or Mackie

    clipEncoderValue_MCP23017(encIndex, encoderMinVal, encoderMaxVal * encSensitivity);  // checks if it's greater than the max value or less than the min value

#endif

    // USING HIGH RES ENCODERS
#ifdef USING_HIGH_RES_ENC  // if def

    highResenc_MCP23017[ENCODER_MIDI_CH][encIndex] = map(encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex], 0, encoderMaxVal * encSensitivity, 0, encoderMaxVal);  // changes the enconder sensitivity
    encMSB_MCP23017[ENCODER_MIDI_CH][encIndex] = highResenc_MCP23017[ENCODER_MIDI_CH][encIndex] / 128;                                                                     // Most Sigficant Byte
    encLSB_MCP23017[ENCODER_MIDI_CH][encIndex] = highResenc_MCP23017[ENCODER_MIDI_CH][encIndex] % 128;                                                                     // Least Significant Byte

#endif  // USING_HIGH_RES_ENC

    encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex] = map(encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex], encoderMinVal, encoderMaxVal * encSensitivity, 0, encoderMaxVal);  // changes the enconder sensitivity

    // - - - - - - - - - - - - - - - - -

    if (encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex] != lastEncoderValue_MCP23017[ENCODER_MIDI_CH][encIndex]) {  // If the count has changed

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -


      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      //clipEncoderValue(i, encoderMinVal, encoderMaxVal); // checks if it's greater than the max value or less than the min value

      // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328

#ifdef USING_HIGH_RES_ENC                                                                           // using
      MIDI.sendControlChange(ENCODER_CC_N[encIndex], encMSB_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);       // MSB
      MIDI.sendControlChange(ENCODER_CC_N[encIndex] + 32, encLSB_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);  // LSB

#else  // normal \
// if using with ATmega328 (uno, mega, nano...)
      MIDI.sendControlChange(ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);
#endif
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif ATMEGA32U4
      // if using with ATmega32U4 (micro, pro micro, leonardo...)

#ifdef MIDI_DIN
      // if using MIDI din cable

#ifdef USING_MACKIE  // to use with Mackie

      // usb
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encoderMackieValue[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();
      // din
      midi2.sendControlChange(ENCODER_CC_N[encIndex], encoderMackieValue[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH + 1);

#elif USING_HIGH_RES_ENC  // if def

      // usb
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encMSB_MCP23017[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex] + 32, encLSB_MCP23017[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();
      // din
      midi2.sendControlChange(ENCODER_CC_N[encIndex], encMSB_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH + 1);       // MSB
      midi2.sendControlChange(ENCODER_CC_N[encIndex] + 32, encLSB_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH + 1);  // LSB

#else
      // usb
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();
      // din
      midi2.sendControlChange(ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH + 1);

#endif  // no MACKIE

#else  // if not using MIDI_DIN

#ifdef USING_MACKIE  // to use with Mackie
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encoderMackieValue[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();

#elif USING_HIGH_RES_ENC  // if def

      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encMSB_MCP23017[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex] + 32, encLSB_MCP23017[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();

#else
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex]);  //  (channel, CC number,  CC value)
      MidiUSB.flush();
#endif

#endif  // MIDI_DIN

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif TEENSY
      // if using with Teensy

#ifdef USING_MACKIE  // to use with Mackie
      usbMIDI.sendControlChange(ENCODER_CC_N[encIndex], encoderMackieValue[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);

#elif USING_HIGH_RES_ENC  // if def
      usbMIDI.sendControlChange(ENCODER_CC_N[encIndex], encMSB_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);
      usbMIDI.sendControlChange(ENCODER_CC_N[encIndex] + 32, encLSB_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);

#else
      usbMIDI.sendControlChange(ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);
#endif

      //  usbMIDI.sendControlChange(ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex], ENCODER_MIDI_CH);

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif BLEMIDI

#ifdef USING_MACKIE  // if USING Remote Script protocol

      BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encoderMackieValue[ENCODER_MIDI_CH][encIndex]);  // channel, cc number, cc value

#elif USING_HIGH_RES_ENC  // if def

      BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encMSB_MCP23017[ENCODER_MIDI_CH][encIndex]);       // MSB
      BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex] + 32, encLSB_MCP23017[ENCODER_MIDI_CH][encIndex]);  // LSB

#else  // not using Mackie or HighRes \
// if using with BLE MIDI (ESP32)
      BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[encIndex], encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex]);  // channel, cc number, cc value

#endif  // mackie

#endif

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef DEBUG
      Serial.print("Encoder: ");
      Serial.print(encIndex);
      Serial.print(" | ch: ");
      Serial.print(ENCODER_MIDI_CH);
      Serial.print(" | ");
      Serial.print("cc: ");
      Serial.print(ENCODER_CC_N[encIndex]);
#ifdef USING_MACKIE  // to use with Mackie
      Serial.print(" | value: ");
      Serial.println(encoderMackieValue[ENCODER_MIDI_CH][encIndex]);
#elif USING_HIGH_RES_ENC  // if def
      Serial.print("  | |  High Res enc: ");
      Serial.print(highResenc_MCP23017[N_ENCODER_MIDI_CHANNELS][N_ENC_MCP23017]);
      Serial.print(" |  MSB: ");
      Serial.print(encMSB_MCP23017[ENCODER_MIDI_CH][encIndex]);
      Serial.print("  LSB: ");
      Serial.println(encLSB_MCP23017[ENCODER_MIDI_CH][encIndex]);
#else
      Serial.print(" | value: ");
      Serial.println(encoderMidiValue_MCP23017[ENCODER_MIDI_CH][encIndex]);
#endif

#endif  // debug

      lastEncoderValue_MCP23017[ENCODER_MIDI_CH][encIndex] = encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex];  // Update the previous count
    }
  }
}


// - - - - - - - - - - -

void updateEncIndex(int _interruptPin) {

  // Loop through each subarray in encoderPin
  for (int i = 0; i < N_ENC_MCP23017; i++) {
    // Check if _interruptPin matches either value in the current subarray
    if (_interruptPin == encoderPin[i][0] || _interruptPin == encoderPin[i][1]) {
      // If a match is found, set encIndex to the current index i and exit the loop
      encIndex = i;
      break;
    }
  }
}

// - - - - - - - - - - -

////////////////////////////////////////////
// checks if it's greater than maximum value or less than then the minimum value
void clipEncoderValue_MCP23017(int encIndex, int minVal, int maxVal) {

  if (encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex] > maxVal - 1) {
    encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex] = maxVal;
    //Serial.println(encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex]);
    //encoder[encIndex].write(maxVal);
  }
  if (encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex] < minVal + 1) {
    encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex] = minVal;
    //Serial.println(encoderValue_MCP23017[ENCODER_MIDI_CH][encIndex]);
    // encoder[encIndex].write(minVal);
  }
}

#endif  // USING_ENCODER_MCP23017
