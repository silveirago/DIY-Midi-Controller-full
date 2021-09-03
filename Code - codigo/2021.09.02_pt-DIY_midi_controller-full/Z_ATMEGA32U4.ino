/////////////////////////////////////////////
// if using with ATmega32U4 (micro, pro micro, leonardo...)
#ifdef ATMEGA32U4

// Arduino (pro)micro midi functions MIDIUSB Library
// Functions to SEND MIDI
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void pitchBend(byte channel, int value) {
  byte lowValue = value & 0x7F;
  byte highValue = value >> 7;
  midiEventPacket_t pitchBend = {0x0E, 0xE0 | channel, lowValue, highValue};
  MidiUSB.sendMIDI(pitchBend);
}

// Functions to RECEIVE MIDI (MIDIUSB lib)
void MIDIread() {

  midiEventPacket_t rx = MidiUSB.read();

  switch (rx.header) {
    case 0:
      break; //No pending events

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
      handlePitchBend(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //LSB
        rx.byte3         //MSB
      );
      break;
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

    ShiftPWM.SetOne(ledPin[led_n], value);

  }

#endif

}

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

    ShiftPWM.SetOne(ledPin[led_n], LOW);

  }

#endif


}

void handleControlChange(byte channel, byte number, byte value)
{

#ifdef USING_VU

  VU(channel, number, value); //runs the VU
#endif

#ifdef USING_ENCODER // only happens if encoders are defined in the setup

  encTempVal = value;

  // finds which encoder it is judging by its CC
  for (int i = 0; i < N_ENCODERS; i++) {
    if (number == ENCODER_CC_N[i]) {
      encoder_n = i;

      if (channel == ENCODER_MIDI_CH  ) {
        encMoved[i] = true;

      }
    }
  }
  encoderValue[channel][encoder_n] = value; // stores the incoming CC in the encoder's value

#endif

  /////////////////////////////////////////////
  // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS


  if (number == CC) { // to avoid listening to the + 32 CC of the higyh res fader

#ifndef DEBUG
    number--;
#endif

    byte midiValue = value;
    midiValue = clipValue(midiValue, 1, 126);


    if (channel == POT_MIDI_CH) {

      int faderVar = midiValue - pFaderInVal[channel][number]; // Calculates the absolute value between the difference between the current and previous cc value
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
#endif // USING_MOTORIZED_FADERS

}

void handlePitchBend(byte channel, byte LSB, byte MSB)
{


  Serial.print("Pitch bend | ");
  Serial.print("ch:  ");
  Serial.print(channel);
  Serial.print("  LSB:  ");
  Serial.print(LSB);
  Serial.print("  MSB:  ");
  Serial.print(MSB);
  Serial.print("  PB Val:  ");
  Serial.print((MSB << 7) + LSB);

  Serial.println();

  /////////////////////////////////////////////
  // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS


  byte midiValue = MSB;
  midiValue = clipValue(midiValue, 1, 126);

#ifdef USING_MACKIE
  if (channel == 0) { // update fader position
#else // not using Mackie
  if (channel == POT_MIDI_CH) { // update fader position
#endif //USING_MACKIE

    int faderVar = midiValue - pFaderPBInVal[channel]; // Calculates the absolute value between the difference between the current and previous cc value
    faderVar = abs(faderVar);

    faderSpeed[channel] = map(faderVar, 0, 127, faderSpeedMin, faderSpeedMax);
    faderPos[channel] = map(midiValue, 0, 127, faderMin[channel], faderMax[channel]);

  }

  pFaderPBInVal[channel] = midiValue;

#endif


}

////////////////////////////////////////////
