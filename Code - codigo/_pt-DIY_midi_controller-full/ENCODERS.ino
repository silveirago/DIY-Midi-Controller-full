/////////////////////////////////////////////
// ENCODERS

#ifdef USING_ENCODER // only happens if encoders are defined in the setup

void encoders() {

  for (int i = 0; i < N_ENCODERS; i++) {
    encoderValue[ENCODER_MIDI_CH][i] = encoder[i].read(); // reads each encoder and stores the value
  }

  for (int i = 0; i < N_ENCODERS; i++) {

    if (encoderValue[ENCODER_MIDI_CH][i] != lastEncoderValue[ENCODER_MIDI_CH][i]) {

#ifdef TRAKTOR // to use with Traktor
      if (encoderValue[ENCODER_MIDI_CH][i] > lastEncoderValue[ENCODER_MIDI_CH][i]) {
        encoderValue[ENCODER_MIDI_CH][i] = 127;
      } else {
        encoderValue[ENCODER_MIDI_CH][i] = 0;
      }
#endif

      clipEncoderValue(i, encoderMinVal, encoderMaxVal); // checks if it's greater than the max value or less than the min value

      // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328
      // if using with ATmega328 (uno, mega, nano...)
      MIDI.sendControlChange(ENCODER_CC_N[i], encoderValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

#elif ATMEGA32U4
      // if using with ATmega32U4 (micro, pro micro, leonardo...)
      controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderValue[ENCODER_MIDI_CH][i]); //  (channel, CC number,  CC value)
      MidiUSB.flush();

#elif TEENSY
      // if using with Teensy
      usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);

#elif DEBUG
      Serial.print("Encoder: ");
      Serial.print(i);
      Serial.print(" | ch: ");
      Serial.print(ENCODER_MIDI_CH);
      Serial.print(" | ");
      Serial.print("cc: ");
      Serial.print(ENCODER_CC_N[i]);
      Serial.print(" | value: ");
      Serial.println(encoderValue[ENCODER_MIDI_CH][i]);
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

      encPTime[i] = millis(); // Stores the previous time
    }

    encTimer[i] = millis() - encPTime[i]; // Resets the encTimer 11000 - 11000 = 0ms

    if (encTimer[i] < encTIMEOUT) { // If the encTimer is less than the maximum allowed time it means that the encoder is still moving
      encMoving[i] = true;
      encMoved[i] = false;
    }
    else {
      if (encMoving[i] == true) {
        encoder[encoder_n].write(encTempVal);
        encMoving[i] = false;
      }
    }
  }
}


#endif
