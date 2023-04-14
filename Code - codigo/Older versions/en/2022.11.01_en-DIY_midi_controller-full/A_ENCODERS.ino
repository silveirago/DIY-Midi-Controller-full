/////////////////////////////////////////////
// ENCODERS

#ifdef USING_ENCODER  // only happens if encoders are defined in the setup


/////////////////////////////////////////////
// variables you don't need to change

int lastEncoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 127 };
int encoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 127 };
int encoderMidiValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 127 };
int encoderMackieValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 127 };

// for the encoder Channels - Used for different banks

byte lastEncoderChannel = 0;

unsigned long encPTime[N_ENCODERS] = { 0 };
unsigned long encTimer[N_ENCODERS] = { 0 };
boolean encMoving[N_ENCODERS] = { false };
boolean encMoved[N_ENCODERS] = { false };
byte encTIMEOUT = 50;
byte encoder_n;
byte encTempVal = 0;

boolean isClockwise = true;


/////////////////////////////////////////////
// Function
void encoders() {

  for (int i = 0; i < N_ENCODERS; i++) {
    encoderValue[ENCODER_MIDI_CH][i] = encoder[i].read();  // reads each encoder and stores the value
                                                           //encoderValue[ENCODER_MIDI_CH][i] = map(encoderValue[ENCODER_MIDI_CH][i], 0, 127, 0, 254); // try increasing/decreasing the last value for more/less sensitivity

#if !defined(TRAKTOR) && !defined(USING_MACKIE)  // if not define
    //#ifndef TRAKTOR // if not def Traktor
    //#ifndef USING_MACKIE
    clipEncoderValue(i, encoderMinVal, encoderMaxVal);  // checks if it's greater than the max value or less than the min value
                                                        //#endif
#endif

    encoderMidiValue[ENCODER_MIDI_CH][i] = encoderValue[ENCODER_MIDI_CH][i];
  }

  for (int i = 0; i < N_ENCODERS; i++) {

    if (encoderValue[ENCODER_MIDI_CH][i] != lastEncoderValue[ENCODER_MIDI_CH][i]) {

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef TRAKTOR  // to use with Traktor
      if (beaurValue[ENCODER_MIDI_CH][i] > lastEncoderValue[ENCODER_MIDI_CH][i]) {
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
      // if using with ATmega328 (uno, mega, nano...)
      MIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

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
#else
      //controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  //  (channel, CC number,  CC value)
      noteOn(ENCODER_MIDI_CH, encoderMidiValue[ENCODER_MIDI_CH][i], 127);  // channel, note, velocity
      MidiUSB.flush();
#endif

#endif  // atmega32U4

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif TEENSY
      // if using with Teensy
      usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif BLEMIDI
      // if using with BLE MIDI (ESP32)
      BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  // channel, cc number, cc value

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#elif DEBUG
      Serial.print("Encoder: ");
      Serial.print(i);
      Serial.print(" | ch: ");
      Serial.print(ENCODER_MIDI_CH);
      Serial.print(" | ");
      Serial.print("cc: ");
      Serial.print(ENCODER_CC_N[i]);
      Serial.print(" | value: ");
#ifdef USING_MACKIE  // to use with Mackie
      Serial.println(encoderMackieValue[ENCODER_MIDI_CH][i]);
#else
      Serial.println(encoderMidiValue[ENCODER_MIDI_CH][i]);
#endif
#endif

      lastEncoderValue[ENCODER_MIDI_CH][i] = encoderValue[ENCODER_MIDI_CH][i];
    }
  }
}


////////////////////////////////////////////
// checks if it's greater than maximum value or less than then the minimum value
void clipEncoderValue(int i, int minVal, int maxVal) {

  if (encoderValue[ENCODER_MIDI_CH][i] > maxVal - 1) {
    encoderValue[ENCODER_MIDI_CH][i] = maxVal;
    encoder[i].write(maxVal);
  }
  if (encoderValue[ENCODER_MIDI_CH][i] < minVal + 1) {
    encoderValue[ENCODER_MIDI_CH][i] = minVal;
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
        encoder[encoder_n].write(encTempVal);
        encMoving[i] = false;
      }
    }
  }
}


#endif