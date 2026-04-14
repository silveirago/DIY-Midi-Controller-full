#ifdef TEENSY

void processMIDI(void) {
  byte type, channel, number, value, cable;

  // fetch the MIDI message, defined by these 5 numbers (except SysEX)
  //
  type = usbMIDI.getType();        // which MIDI message, 128-255
  channel = usbMIDI.getChannel();  // which MIDI channel, 1-16
  number = usbMIDI.getData1();     // first data byte of message, 0-127
  value = usbMIDI.getData2();      // second data byte of message, 0-127
  cable = usbMIDI.getCable();      // which virtual cable with MIDIx8, 0-7

  // uncomment if using multiple virtual cables
  //Serial.print("cable ");
  //Serial.print(cable, DEC);
  //Serial.print(": ");

  // print info about the message
  //
  switch (type) {

    /////////////////////////////////////////////
    case usbMIDI.NoteOff:  // 0x80

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
      Serial.print("Note Off, ch=");
      Serial.print(channel, DEC);
      Serial.print(", note=");
      Serial.print(number, DEC);
      Serial.print(", velocity=");
      Serial.println(value, DEC);
      break;

    /////////////////////////////////////////////

    case usbMIDI.NoteOn:  // 0x90


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
#endif
      }

#endif
      Serial.print("Note On, ch=");
      Serial.print(channel, DEC);
      Serial.print(", note=");
      Serial.print(number, DEC);
      Serial.print(", velocity=");
      Serial.println(value, DEC);
      break;

    case usbMIDI.AfterTouchPoly:  // 0xA0
      Serial.print("AfterTouch Change, ch=");
      Serial.print(channel, DEC);
      Serial.print(", note=");
      Serial.print(number, DEC);
      Serial.print(", velocity=");
      Serial.println(value, DEC);
      break;

    /////////////////////////////////////////////

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    case usbMIDI.ControlChange:  // 0xB0



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

#endif //USING_ENCODER

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


      if (number == CC) {  // to avoid listening to the + 32 CC of the higyh res fader

#ifndef DEBUG
        number--;
#endif

        byte midiValue = value;
        midiValue = clipValue(midiValue, 1, 126);


        if (channel == POT_MIDI_CH) {

          int faderVar = midiValue - pFaderInVal[channel][number];  // Calculates the absolute value between the difference between the current and previous cc value
          faderVar = abs(faderVar);

          faderSpeed[number] = map(faderVar, 0, 127, faderSpeedMin, faderSpeedMax);
          faderPos[number] = map(midiValue, 0, 127, faderMin[number], faderMax[number]);
        }

        pFaderInVal[channel][number] = midiValue;

        //    Serial.print("CC | ");
        //    Serial.print("ch:  ");
        //    Serial.print(channel);
        //    Serial.print("  number:  ");
        //    Serial.print(number);
        //    Serial.print("  value:  ");
        //    Serial.print(value);
        //    Serial.println();
      }
#endif  // USING_MOTORIZED_FADERS

      //      Serial.print("Control Change, ch=");
      //      Serial.print(channel, DEC);
      //      Serial.print(", control=");
      //      Serial.print(number, DEC);
      //      Serial.print(", value=");
      //      Serial.println(value, DEC);
      break;

    /////////////////////////////////////////////

    case usbMIDI.ProgramChange:  // 0xC0
      Serial.print("Program Change, ch=");
      Serial.print(channel, DEC);
      Serial.print(", program=");
      Serial.println(number, DEC);
      break;

    /////////////////////////////////////////////

    case usbMIDI.AfterTouchChannel:  // 0xD0
      Serial.print("After Touch, ch=");
      Serial.print(channel, DEC);
      Serial.print(", pressure=");
      Serial.println(number, DEC);
      break;

    /////////////////////////////////////////////

    case usbMIDI.PitchBend:  // 0xE0


      //#ifdef DEBUG
      //      Serial.print("Pitch bend | ");
      //      Serial.print("ch:  ");
      //      Serial.print(channel);
      //      Serial.print("  LSB:  ");
      //      Serial.print(LSB);
      //      Serial.print("  MSB:  ");
      //      Serial.print(MSB);
      //      Serial.print("  PB Val:  ");
      //      Serial.print((MSB << 7) + LSB);
      //
      //      Serial.println();
      //#endif

      /////////////////////////////////////////////
      // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS


      byte midiValue = MSB;
      midiValue = clipValue(midiValue, 1, 126);

#ifdef USING_MACKIE
      if (channel == 0) {  // update fader position
#else                      // not using Mackie
      if (channel == POT_MIDI_CH) {  // update fader position
#endif                     //USING_MACKIE

        int faderVar = midiValue - pFaderPBInVal[channel];  // Calculates the absolute value between the difference between the current and previous cc value
        faderVar = abs(faderVar);

        faderSpeed[channel] = map(faderVar, 0, 127, faderSpeedMin, faderSpeedMax);
        faderPos[channel] = map(midiValue, 0, 127, faderMin[channel], faderMax[channel]);
      }

      pFaderPBInVal[channel] = midiValue;

#endif

      Serial.print("Pitch Change, ch=");
      Serial.print(channel, DEC);
      Serial.print(", pitch=");
      Serial.println(number + value * 128, DEC);
      break;

    /////////////////////////////////////////////

    case usbMIDI.SystemExclusive:  // 0xF0
      // Messages larger than usbMIDI's internal buffer are truncated.
      // To receive large messages, you *must* use the 3-input function
      // handler.  See InputFunctionsComplete for details.
      Serial.print("SysEx Message: ");
      printBytes(usbMIDI.getSysExArray(), number + value * 256);
      Serial.println();
      break;

    /////////////////////////////////////////////

    case usbMIDI.TimeCodeQuarterFrame:  // 0xF1
      Serial.print("TimeCode, index=");
      Serial.print(number >> 4, DEC);
      Serial.print(", digit=");
      Serial.println(number & 15, DEC);
      break;

    /////////////////////////////////////////////

    case usbMIDI.SongPosition:  // 0xF2
      Serial.print("Song Position, beat=");
      Serial.println(number + value * 128);
      break;

    case usbMIDI.SongSelect:  // 0xF3
      Serial.print("Sond Select, song=");
      Serial.println(number, DEC);
      break;

    case usbMIDI.TuneRequest:  // 0xF6
      Serial.println("Tune Request");
      break;

    case usbMIDI.Clock:  // 0xF8
      Serial.println("Clock");
      break;

    case usbMIDI.Start:  // 0xFA
      Serial.println("Start");
      break;

    case usbMIDI.Continue:  // 0xFB
      Serial.println("Continue");
      break;

    case usbMIDI.Stop:  // 0xFC
      Serial.println("Stop");
      break;

    case usbMIDI.ActiveSensing:  // 0xFE
      Serial.println("Actvice Sensing");
      break;

    case usbMIDI.SystemReset:  // 0xFF
      Serial.println("System Reset");
      break;

    default:
      Serial.println("Opps, an unknown MIDI message type!");
  }
}


void printBytes(const byte *data, unsigned int size) {
  while (size > 0) {
    byte b = *data++;
    if (b < 16) Serial.print('0');
    Serial.print(b, HEX);
    if (size > 1) Serial.print(' ');
    size = size - 1;
  }
}


#endif  // TEENSY
