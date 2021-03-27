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
  }

  if (rx.header != 0) {
    //Serial.print("Unhandled MIDI message: ");
    //      Serial.print(rx.byte1 & 0xF, DEC);
    //      Serial.print("-");
    //      Serial.print(rx.byte1, DEC);
    //      Serial.print("-");
    //      Serial.print(rx.byte2, DEC);
    //      Serial.print("-");
    //      Serial.println(rx.byte3, DEC);
  }

}

#endif

void handleControlChange(byte channel, byte number, byte value)
{

  //  Serial.print("ATMEGA328: ");
  //  Serial.println(channel);
  //  printChannel(channel);

  byte _channel = channel;
  byte _number = number;
  byte _value = value;

#ifdef USING_VU

  VU(_channel, _number, _value); //runs the VU
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

}

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

////////////////////////////////////////////
