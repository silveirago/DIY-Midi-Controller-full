#ifdef USING_NEOPIXEL

////////////////////////////////////////////
// Channel Menu
void channelMenu() {

  while (digitalRead(CHANNEL_BUTTON_PIN) == LOW) {

    setAllLeds(midiChMenuColor, 0); // turn all lights into the menu lights
    leds[ledIndex[BUTTON_MIDI_CH]].setHue(midiChOnColor); // turn the specific channel light on
    channelMenuOn = true;

    // read pins from arduino
    for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
      buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
    }

#ifdef USING_MUX

    // It will happen if you are using MUX
    int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // offsets the buttonCState at every mux reading

    // read the pins from every mux
    for (int j = 0; j < N_MUX; j++) {
      for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
        buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
        // Scale values to 0-1
        if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
          buttonCState[i + nButtonsPerMuxSum] = HIGH;
        }
        else {
          buttonCState[i + nButtonsPerMuxSum] = LOW;
        }
      }
      nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
    }
#endif

    for (int i = 0; i < N_BUTTONS; i++) { // Read the buttons connected to the Arduino

#ifdef pin13

      // It will happen if you are using pin 13
      if (i == pin13index) {
        buttonCState[i] = !buttonCState[i]; // inverts the pin 13 because it has a pull down resistor instead of a pull up
      }
#endif

      if ((millis() - lastDebounceTime[i]) > debounceDelay) {

        if (buttonPState[i] != buttonCState[i]) {
          lastDebounceTime[i] = millis();

          if (buttonCState[i] == LOW) {
            // DO STUFF
            BUTTON_MIDI_CH = i;
            //Serial.print("Channel ");
            //Serial.println(BUTTON_MIDI_CH);

          }
          buttonPState[i] = buttonCState[i];
        }
      }
    }
    FastLED.show();
    //      // insert a delay to keep the framerate modest
    //      FastLED.delay(1000 / FRAMES_PER_SECOND);
  }

  if (channelMenuOn == true) {
    setAllLeds(noteOffHue, 30);
    //Serial.println("menu lights off");

  }
  channelMenuOn = false;
  //
  FastLED.show();
  //    // insert a delay to keep the framerate modest
  //    FastLED.delay(1000 / FRAMES_PER_SECOND);

} //end

void setAllLeds(byte hue_, byte randomness_) {

  for (int i = 0; i < NUM_LEDS; i++) {
    byte offset = random(0, randomness_);
    leds[i].setHue(hue_  + offset);
  }
}

#endif
