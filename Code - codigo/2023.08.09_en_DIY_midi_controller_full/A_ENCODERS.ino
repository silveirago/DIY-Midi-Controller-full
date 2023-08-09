/////////////////////////////////////////////
// ENCODERS

#ifdef USING_ENCODER  // only happens if encoders are defined in the setup


/////////////////////////////////////////////
// variables you don't need to change

int lastEncoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };
int lastEncoderMidiValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };
int encoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };
int encoderMidiValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };
int encoderMackieValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };

// for the encoder Channels - Used for different banks

byte lastEncoderChannel = 0;

unsigned long encPTime[N_ENCODERS] = { 0 };
unsigned long encTimer[N_ENCODERS] = { 0 };
boolean encMoving[N_ENCODERS] = { false };
boolean encMoved[N_ENCODERS] = { false };
byte encTIMEOUT = 50;
byte encoder_n;
float encTempVal = 0;

#ifdef USING_HIGH_RES_ENC
// Use this to send CC using 2 bytes (14 bit res)
unsigned int highResenc[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };  // stores the high res val
byte encMSB[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };              // Most Significant Byte
byte encLSB[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };              // Least Significant Byte
byte pencLSB[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };             // Previous Most Significant Byte
#endif

/////////////////////////////////////////////
// Function
void encoders() {


  for (int i = 0; i < N_ENCODERS; i++) {

    int encoderValueTemp = encoder[i].read();  // reads each encoder and stores the value
    // Serial.print("Raw Encoder Value: ");
    // Serial.println(encoderValueTemp);
    encoderValue[ENCODER_MIDI_CH][i] = encoderValueTemp;

#if !defined(TRAKTOR) && !defined(USING_MACKIE)  // if not defined traktor or Mackie
    //#ifndef TRAKTOR // if not def Traktor
    //#ifndef USING_MACKIE
    clipEncoderValue(i, encoderMinVal, encoderMaxVal * encSensitivity);  // checks if it's greater than the max value or less than the min value
                                                                         //#endif

#endif

    // USING HIGH RES ENCODERS
#ifdef USING_HIGH_RES_ENC  // if def

    highResenc[ENCODER_MIDI_CH][i] = map(encoderValue[ENCODER_MIDI_CH][i], 0, encoderMaxVal * encSensitivity, 0, encoderMaxVal);  // changes the enconder sensitivity
    encMSB[ENCODER_MIDI_CH][i] = highResenc[ENCODER_MIDI_CH][i] / 128;                                                            // Most Sigficant Byte
    encLSB[ENCODER_MIDI_CH][i] = highResenc[ENCODER_MIDI_CH][i] % 128;                                                            // Least Significant Byte

#endif  // USING_HIGH_RES_ENC

    encoderMidiValue[ENCODER_MIDI_CH][i] = map(encoderValue[ENCODER_MIDI_CH][i], encoderMinVal, encoderMaxVal * encSensitivity, 0, encoderMaxVal);  // changes the enconder sensitivity
  }

  for (int i = 0; i < N_ENCODERS; i++) {

    if (!encMoving[i]) {


      // #ifdef USING_HIGH_RES_ENC
      //     if (encLSB[ENCODER_MIDI_CH][i] != pencLSB[ENCODER_MIDI_CH][i]) {
      //       Serial.println(highResenc[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]);
      // #else
      if (encoderMidiValue[ENCODER_MIDI_CH][i] != lastEncoderMidiValue[ENCODER_MIDI_CH][i]) {
        //#endif

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef TRAKTOR  // to use with Traktor
        if (encoderValue[ENCODER_MIDI_CH][i] > lastEncoderValue[ENCODER_MIDI_CH][i]) {
          encoderMidiValue[ENCODER_MIDI_CH][i] = 127;
          //encoder[i].write(127);
        } else {
          encoderMidiValue[ENCODER_MIDI_CH][i] = 0;
          //encoder[i].write(0);
        }
#endif  // TRAKTOR

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MACKIE  // to use with Mackie
        if (encoderValue[ENCODER_MIDI_CH][i] > lastEncoderValue[ENCODER_MIDI_CH][i]) {
          encoderMackieValue[ENCODER_MIDI_CH][i] = 0 + encoderSens;
        } else {
          encoderMackieValue[ENCODER_MIDI_CH][i] = 64 + encoderSens;
        }
#endif


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //clipEncoderValue(i, encoderMinVal, encoderMaxVal); // checks if it's greater than the max value or less than the min value

        // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328

#ifdef USING_HIGH_RES_ENC                                                                           // using
        MIDI.sendControlChange(ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);       // MSB
        MIDI.sendControlChange(ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);  // LSB

#else  // normal \
// if using with ATmega328 (uno, mega, nano...)
        MIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
#endif
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif ATMEGA32U4
        // if using with ATmega32U4 (micro, pro micro, leonardo...)

#ifdef MIDI_DIN
        // if using MIDI din cable

#ifdef USING_MACKIE  // to use with Mackie

        // usb
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        // din
        midi2.sendControlChange(ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);

#elif USING_HIGH_RES_ENC  // if def

        // usb
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        // din
        midi2.sendControlChange(ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);       // MSB
        midi2.sendControlChange(ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);  // LSB

#else
        // usb
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        // din
        midi2.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);

#endif  // no MACKIE

#else  // if not using MIDI_DIN

#ifdef USING_MACKIE  // to use with Mackie
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();

#elif USING_HIGH_RES_ENC  // if def

        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();

#else
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
        MidiUSB.flush();
#endif

#endif  // MIDI_DIN

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif TEENSY
        // if using with Teensy

#ifdef USING_MACKIE  // to use with Mackie
        usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

#elif USING_HIGH_RES_ENC  // if def
        usbMIDI.sendControlChange(ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
        usbMIDI.sendControlChange(ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

#else
        usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
#endif

        //  usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif BLEMIDI

#ifdef USING_MACKIE  // if USING Remote Script protocol

        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i]);  // channel, cc number, cc value

#elif USING_HIGH_RES_ENC  // if def

        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i]);       // MSB
        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i]);  // LSB

#else  // not using Mackie or HighRes \
// if using with BLE MIDI (ESP32)
        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  // channel, cc number, cc value

#endif  // mackie

#endif

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef DEBUG
        Serial.print("Encoder: ");
        Serial.print(i);
        Serial.print(" | ch: ");
        Serial.print(ENCODER_MIDI_CH);
        Serial.print(" | ");
        Serial.print("cc: ");
        Serial.print(ENCODER_CC_N[i]);
#ifdef USING_MACKIE  // to use with Mackie
        Serial.print(" | value: ");
        Serial.println(encoderMackieValue[ENCODER_MIDI_CH][i]);
#elif USING_HIGH_RES_ENC  // if def
        Serial.print("  | |  High Res enc: ");
        Serial.print(highResenc[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]);
        Serial.print(" |  MSB: ");
        Serial.print(encMSB[ENCODER_MIDI_CH][i]);
        Serial.print("  LSB: ");
        Serial.println(encLSB[ENCODER_MIDI_CH][i]);
#else
        Serial.print(" | value: ");
        Serial.println(encoderMidiValue[ENCODER_MIDI_CH][i]);
#endif

#endif  // debug

        lastEncoderMidiValue[ENCODER_MIDI_CH][i] = encoderMidiValue[ENCODER_MIDI_CH][i];
        lastEncoderValue[ENCODER_MIDI_CH][i] = encoderValue[ENCODER_MIDI_CH][i];

#ifdef USING_HIGH_RES_ENC  // if def
        pencLSB[ENCODER_MIDI_CH][i] = encLSB[ENCODER_MIDI_CH][i];
#endif
      }
    }
  }
}


////////////////////////////////////////////
// checks if it's greater than maximum value or less than then the minimum value
void clipEncoderValue(int i, int minVal, int maxVal) {

  if (encoderValue[ENCODER_MIDI_CH][i] > maxVal - 1) {
    encoderValue[ENCODER_MIDI_CH][i] = maxVal;
    //Serial.println(encoderValue[ENCODER_MIDI_CH][i]);
    encoder[i].write(maxVal);
  }
  if (encoderValue[ENCODER_MIDI_CH][i] < minVal + 1) {
    encoderValue[ENCODER_MIDI_CH][i] = minVal;
    //Serial.println(encoderValue[ENCODER_MIDI_CH][i]);
    encoder[i].write(minVal);
  }
}

/* When receiving MIDI from the daw the encoder updates itself and send midi back
  this causes some sort of feedack. This function prevents the encoder to update itself
  unti it stopped receiving midi from the daw.
*/
void isEncoderMoving() {

  for (int i = 0; i < N_ENCODERS; i++) {

    if (encMoved[i] == true) {

      encPTime[i] = millis();  // Stores the previous time
    }

    encTimer[i] = millis() - encPTime[i];  // Resets the encTimer 11000 - 11000 = 0ms

    if (encTimer[i] < encTIMEOUT) {  // If the encTimer is less than the maximum allowed time it means that the encoder is still moving
      encMoving[i] = true;
      encMoved[i] = false;

    } else {

      if (encMoving[i] == true) {

        encMoving[i] = false;
      }
    }
  }
}


#endif
