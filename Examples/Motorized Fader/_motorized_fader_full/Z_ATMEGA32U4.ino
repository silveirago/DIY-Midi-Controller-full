/////////////////////////////////////////////
// if using with ATmega32U4 (micro, pro micro, leonardo...)
#ifdef ATMEGA32U4

// - - - - - - - - - - - - - - - - - - - - -
// FUNCTIONS TO SEND MIDI OUT OF THE USB

// Arduino (pro)micro midi functions MIDIUSB Library
// Functions to SEND MIDI
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}

void pitchBend(byte channel, int value) {
  byte lowValue = value & 0x7F;
  byte highValue = value >> 7;
  midiEventPacket_t pitchBend = { 0x0E, 0xE0 | channel, lowValue, highValue };
  MidiUSB.sendMIDI(pitchBend);
}

/*
  Program Change message: This message is used to change the instrument or program on a MIDI device.
  The status byte for this message is 0xC0, and the data byte contains the new program number (0-127).
*/
void programChange(byte channel, byte program) {
  midiEventPacket_t message = { 0x0C, 0xC0 | channel, program, 0 };
  MidiUSB.sendMIDI(message);
}
/*
  Channel Pressure message: This message is used to send aftertouch or pressure data from a MIDI device.
  The status byte for this message is 0xD0, and the data byte contains the pressure value (0-127).
*/
void channelPressure(byte channel, byte pressure) {
  midiEventPacket_t message = { 0x0D, 0xD0 | channel, pressure, 0 };
  MidiUSB.sendMIDI(message);
}

/*
  Aftertouch message: This message is used to send pressure data for a single note on a MIDI device.
  The status byte for this message is 0xA0, and the data bytes contain the note number (0-127) and the pressure value (0-127).
*/
void aftertouch(byte channel, byte note, byte pressure) {
  midiEventPacket_t message = { 0x02, 0xA0 | channel, note, pressure };
  MidiUSB.sendMIDI(message);
}

/*
  Polyphonic Aftertouch message: This message is used to send pressure data for multiple notes on a MIDI device.
  The status byte for this message is 0xD0, and the data bytes contain the note number (0-127) and the pressure value (0-127).
*/
void polyAftertouch(byte channel, byte note, byte pressure) {
  midiEventPacket_t message = { 0x02, 0xD0 | channel, note, pressure };
  MidiUSB.sendMIDI(message);
}

/*
  System Common messages: These are messages used for system-wide control of MIDI devices.
  There are several types of System Common messages, including Song Position Pointer (0xF2), Song Select (0xF3),
  Tune Request (0xF6), and System Exclusive (0xF0).
  The data bytes for these messages vary depending on the type of message being sent.
  THESE ONES ARE NOT TESTED YET
*/
void songPositionPointer(byte positionLSB, byte positionMSB) {
  midiEventPacket_t message = { 0x02, 0xF2, positionLSB, positionMSB };
  MidiUSB.sendMIDI(message);
}

void songSelect(byte songNumber) {
  midiEventPacket_t message = { 0x01, 0xF3, songNumber, 0 };
  MidiUSB.sendMIDI(message);
}

void tuneRequest() {
  midiEventPacket_t message = { 0x01, 0xF6, 0, 0 };
  MidiUSB.sendMIDI(message);
}

void systemExclusive(byte* data, byte length) {
  midiEventPacket_t message = { (length > 3 ? 0x04 : length - 1), 0xF0, data[0], data[1] };
  for (int i = 2; i < length; i++) {
    message.byte2 = data[i];
    MidiUSB.sendMIDI(message);
  }
  message.byte2 = 0xF7;
  MidiUSB.sendMIDI(message);
}

// - - - - - - - - - - - - - - - - - - - - -
// Functions to RECEIVE MIDI (MIDIUSB lib)
void MIDIread() {

  midiEventPacket_t rx = MidiUSB.read();

  switch (rx.header) {
    case 0:
      break;  //No pending events

    case 0x9:
      handleNoteOn(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0x8:
      handleNoteOff(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0xB:
      handleControlChange(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //cc
        rx.byte3         //value
      );
      break;

    case 0xE:
      //      handlePitchBend(
      //        rx.byte1 & 0xF,  //channel
      //        rx.byte2,        //LSB
      //        rx.byte3         //MSB
      //      );
      break;

    case 0xC:
      handleProgramChange(
        rx.byte1 & 0xF,  //channel
        rx.byte2         //program number
      );
      break;

    case 0xD:
      handleAftertouch(
        rx.byte1 & 0xF,  //channel
        rx.byte2         //pressure
      );
      break;

      // case 0xF:
      //   {
      //     // The first byte of a system exclusive message is always 0xF0
      //     if (rx.byte1 == 0xF0) {
      //       // We have a valid system exclusive message
      //       byte* data = rx.data;
      //       unsigned int length = rx.byte2;
      //       handleSysEx(data, length);
      //     }
      //   }
      //   break;
  }

  //  if (rx.header != 0) {
  //    Serial.print("Unhandled MIDI message: ");
  //    Serial.print(rx.byte1 & 0xF, DEC);
  //    Serial.print("-");
  //    Serial.print(rx.byte1, DEC);
  //    Serial.print("-");
  //    Serial.print(rx.byte2, DEC);
  //    Serial.print("-");
  //    Serial.println(rx.byte3, DEC);
  //  }
}

#endif

// - - - - - - - - - - - - - - - - - - - - -
// FUNCTIONS TO DO SOMETHING WITH THE MIDI INCOMING MESSAGES

void handleNoteOn(byte channel, byte number, byte value) {

  // If yusing neopixel
#ifdef USING_NEOPIXEL

  if (channel == BUTTON_MIDI_CH) {

    byte led_n = number - NOTE;

    byte tempHue = map(value, 0, 127, noteOffHue, noteOnHue);

    leds[ledIndex[led_n]].setHue(tempHue);

    FastLED.show();
    //      // insert a delay to keep the framerate modest
    //      FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
#endif

#ifdef USING_LED_FEEDBACK

  if (channel == BUTTON_MIDI_CH) {

    byte led_n = number - NOTE;

#ifdef USING_74HC595
    ShiftPWM.SetOne(ledPin[led_n], value);
#else
    // IF not using nit shifter, straight to the Arduino pins
    byte tempBrightness = map(value, 0, 127, 0, 255);
    digitalWrite(ledPin[led_n], tempBrightness);

    Serial.print("MIDI in LED: ");
    Serial.print(ledPin[led_n]);
    Serial.print("  | Value: ");
    Serial.println(tempBrightness);

#endif
  }

#endif
}

// - - - - - - - - - - - - - - - - - - - - -

void handleNoteOff(byte channel, byte number, byte value) {

  // If yusing neopixel
#ifdef USING_NEOPIXEL

  if (channel == BUTTON_MIDI_CH) {

    byte led_n = number - NOTE;

    byte offset = random(0, 20);
    leds[ledIndex[led_n]].setHue(noteOffHue + offset);

    FastLED.show();
    //      // insert a delay to keep the framerate modest
    //      FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
#endif

#ifdef USING_LED_FEEDBACK

  if (channel == BUTTON_MIDI_CH) {

    byte led_n = number - NOTE;

#ifdef USING_74HC595
    ShiftPWM.SetOne(ledPin[led_n], LOW);
#endif

#ifndef USING_74HC595
    digitalWrite(ledPin[led_n], LOW);
#endif
  }

#endif
}

// - - - - - - - - - - - - - - - - - - - - -

void handleControlChange(byte channel, byte number, byte value) {

#ifdef USING_VU

  VU(channel, number, value);  //runs the VU
#endif

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_ENCODER  // only happens if encoders are defined in the setup

  encTempVal = value;

  if (encSensitivity >= 1) {
    encTempVal = map(encTempVal, 0, 127, 0, 127 * encSensitivity);  // changes the encoder sensitivity
  } else {
    encTempVal = lerp(0, 127 * encSensitivity, encTempVal / 127.0);  // changes the encoder sensitivity
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_HIGH_RES_ENC

  // finds which encoder it is judging by its CC
  // MSB
  for (int i = 0; i < N_ENCODERS; i++) {
    if (number == ENCODER_CC_N[i]) {
      encoder_n = i;

      if (channel == ENCODER_MIDI_CH) {
        encMoved[i] = true;
        encMSB[channel][encoder_n] = value;
      }
    }
  }

  // LSB
  for (int i = 0; i < N_ENCODERS; i++) {
    if (number == ENCODER_CC_N[i] + 32) {
      encoder_n = i;

      if (channel == ENCODER_MIDI_CH) {
        encMoved[i] = true;
        encLSB[channel][encoder_n] = value;
      }
    }
  }

  // Combine MSB and LSB values to obtain the high-resolution value
  for (int i = 0; i < N_ENCODERS; i++) {
    highResenc[channel][i] = (encMSB[channel][i] << 7) | encLSB[channel][i];

    if (encSensitivity >= 1) {
      encTempVal = map(highResenc[channel][i], 0, 127, 0, 127 * encSensitivity);  // changes the encoder sensitivity
    } else {
      encTempVal = lerp(0, 127 * encSensitivity, highResenc[channel][i] / 127.0);  // changes the encoder sensitivity
    }

    encoderValue[channel][i] = encTempVal;
    encoder[i].write(encTempVal);
  }

#else

  // NOT USING HIGH RES
  // finds which encoder it is judging by its CC
  for (int i = 0; i < N_ENCODERS; i++) {
    if (number == ENCODER_CC_N[i]) {
      encoder_n = i;

      if (channel == ENCODER_MIDI_CH) {
        encMoved[i] = true;
        encoder[encoder_n].write(encTempVal);           // Updates the encoder val
        encoderValue[channel][encoder_n] = encTempVal;  // stores the incoming CC in the encoder's value

        // Serial.print("encTempVal :");
        // Serial.println(encTempVal);
      }
    }
  }

  //encoderValue[channel][encoder_n] = value;  // stores the incoming CC in the encoder's value
#endif  // USING_HIGH_RES_ENC

#endif  //USING_ENCODER

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_ENCODER_MCP23017  // only happens if encoders are defined in the setup

  encTempVal_MCP23017 = value;

  if (encSensitivity >= 1) {
    encTempVal_MCP23017 = map(encTempVal_MCP23017, 0, 127, 0, 127 * encSensitivity);  // changes the encoder sensitivity
  } else {
    encTempVal_MCP23017 = lerp(0, 127 * encSensitivity, encTempVal_MCP23017 / 127.0);  // changes the encoder sensitivity
  }

#ifdef USING_HIGH_RES_ENC

  // finds which encoder it is judging by its CC
  // MSB
  for (int i = 0; i < N_ENC_MCP23017; i++) {
    if (number == ENCODER_CC_N[i]) {
      encoder_n_MCP23017 = i;

      if (channel == ENCODER_MIDI_CH) {
        encMoved_MCP23017[i] = true;
        encMSB_MCP23017[channel][encoder_n_MCP23017] = value;

        // Print MSB value to serial monitor
#ifdef DEBUG
        Serial.print("Encoder ");
        Serial.print(encoder_n_MCP23017);
        Serial.print(" MSB value: ");
        Serial.println(value);
#endif
      }
    }
  }

  // LSB
  for (int i = 0; i < N_ENC_MCP23017; i++) {
    if (number == ENCODER_CC_N[i] + 32) {
      encoder_n_MCP23017 = i;

      if (channel == ENCODER_MIDI_CH) {
        encMoved_MCP23017[i] = true;
        encLSB_MCP23017[channel][encoder_n_MCP23017] = value;

#ifdef DEBUG
        // Print LSB value to serial monitor
        Serial.print("Encoder ");
        Serial.print(encoder_n_MCP23017);
        Serial.print(" LSB value: ");
        Serial.println(value);
#endif
      }
    }
  }

  // Combine MSB and LSB values to obtain the high-resolution value
  for (int i = 0; i < N_ENC_MCP23017; i++) {
    highResenc_MCP23017[channel][i] = (encMSB_MCP23017[channel][i] << 7) | encLSB_MCP23017[channel][i];

    if (encSensitivity >= 1) {
      encTempVal_MCP23017 = map(highResenc_MCP23017[channel][i], 0, 127, 0, 127 * encSensitivity);  // changes the encoder sensitivity
    } else {
      encTempVal_MCP23017 = lerp(0, 127 * encSensitivity, highResenc_MCP23017[channel][i] / 127.0);  // changes the encoder sensitivity
    }


    encoderValue_MCP23017[channel][i] = encTempVal_MCP23017;
    // // encoder[i].write(encTempVal);

#ifdef DEBUG
    // Print high resolution value to serial monitor
    Serial.print("Encoder ");
    Serial.print(i);
    Serial.print(" High-res value: ");
    Serial.println(highResenc_MCP23017[channel][i]);
#endif
  }

#else

  // NOT USING HIGH RES
  // finds which encoder it is judging by its CC
  for (int i = 0; i < N_ENC_MCP23017; i++) {
    if (number == ENCODER_CC_N[i]) {
      encoder_n_MCP23017 = i;

      if (channel == ENCODER_MIDI_CH) {
        encMoved_MCP23017[i] = true;

        encoderValue_MCP23017[channel][encoder_n_MCP23017] = encTempVal_MCP23017;  // stores the incoming CC in the encoder's value
        //encoderValue_MCP23017[channel][encoder_n_MCP23017] = map(encTempVal_MCP23017, encoderMinVal, encoderMaxVal * encSensitivity, 0, encoderMaxVal);
        //encoderMidiValue_MCP23017[channel][encoder_n_MCP23017] = map(encTempVal_MCP23017, encoderMinVal, encoderMaxVal, 0, encoderMaxVal * encSensitivity);

#ifdef DEBUG
        Serial.print("MCP23017 MIDI in value: ");
        Serial.print(value);
        //Serial.print(encoderValue_MCP23017[channel][encoder_n_MCP23017]);
        Serial.print("  Channel: ");
        Serial.println(channel);
#endif
      }
    }
  }

  //encoderValue_MCP23017[channel][encoder_n] = value;  // stores the incoming CC in the encoder's value
#endif  // USING_HIGH_RES_ENC

#endif  //_MCP23017

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /////////////////////////////////////////////
  // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS


  byte midiValue = value;
  midiValue = clipValue(midiValue, 1, 126);

#ifdef USING_MACKIE
  if (channel == 0) {  // update fader position
#else                  // not using Mackie
  if (channel == POT_MIDI_CH) {  // update fader position
#endif                 //USING_MACKIE

    int faderVar = midiValue - pFaderPBInVal[channel];  // Calculates the absolute value between the difference between the current and previous cc value
    faderVar = abs(faderVar);

    faderSpeed[channel] = map(faderVar, 0, 127, faderSpeedMin, faderSpeedMax);
    faderPos[channel] = map(midiValue, 0, 127, faderMin[channel], faderMax[channel]);
  }

  pFaderPBInVal[channel] = midiValue;

#endif
}

void handleProgramChange(byte channel, byte program) {
  // Do something with the program change message
}

void handleAftertouch(byte channel, byte pressure) {
  // Do something with the aftertouch message
}

void handleSysEx(byte* data, unsigned int length) {
  // Do something with the system exclusive message
}

////////////////////////////////////////////
