void setup() {

  // Baud Rate
  // use if using with ATmega328 (uno, mega, nano...)
  // 31250 for MIDI class compliant | 115200 for Hairless MIDI

  int baudRate = 115200;

#ifdef MIDI_DIN
  baudRate = 31250;
#endif


#ifdef ATMEGA32U4

#ifdef DEBUG
  while (!Serial) {
    Serial.println("waiting...");
  }
  Serial.println();
#endif

#ifdef MIDI_DIN
  Serial1.begin(31250);
  midi2.begin();

  /////////////////////////////////////////////
  // Midi in
  midi2.begin();
  midi2.turnThruOff();
  midi2.setHandleControlChange(myHandleControlChange);
  midi2.setHandleNoteOn(myHandleNoteOn);
  midi2.setHandleNoteOff(myHandleNoteOff);
  midi2.setHandlePitchBend(myHandlePitchBend);
#endif
#endif //ATMEGA32U4


  delay(100);

#ifdef ATMEGA328

  POT_MIDI_CH++;
  BUTTON_MIDI_CH++;
  ENCODER_MIDI_CH++;

  Serial.begin(baudRate);  //*

  /////////////////////////////////////////////
  // Midi in
  MIDI.begin();
  MIDI.turnThruOff();
  MIDI.setHandleControlChange(myHandleControlChange);
  MIDI.setHandleNoteOn(myHandleNoteOn);
  MIDI.setHandleNoteOff(myHandleNoteOff);
  MIDI.setHandlePitchBend(myHandlePitchBend);
#endif


  /////////////////////////////////////////////
  // BLE MIDI
#ifdef BLEMIDI
  Serial.begin(baudRate);
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin(bleMIDIDeviceName);  // the name of the device
  Serial.println("Waiting for connections...");
  //BLEMidiServer.enableDebugging();  // Uncomment if you want to see some debugging output from the library (not much for the server class...)
#endif

  /////////////////////////////////////////////
  // TEENSY
#ifdef TEENSY

#ifdef DEBUG
  while (!Serial) {
    Serial.println("waiting...");
  }
  Serial.println();
#endif // DEBUG


#endif

#ifdef DEBUG
  Serial.begin(baudRate);
  Serial.println("Debug mode");
  Serial.println();
#endif


#ifdef USING_NEOPIXEL

  ////////////////////////////////////
  //FAST LED
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  //FastLED.setBrightness(BRIGHTNESS);

  setAllLeds(noteOffHue, 30);  // set all leds at once with a hue (hue, randomness)

  FastLED.show();

  //////////////////////////////////////

#endif

  //////////////////////////////////////
  // Buttons
#ifdef USING_BUTTONS

  // Initialize buttons with pull up resistors
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef USING_BANKS_WITH_BUTTONS

  //////////////////////////////////////
  // Bank Button
  // Initialize buttons with pull up resistors
  for (int i = 0; i < 2; i++) {
    pinMode(BANK_BUTTON_PIN[i], INPUT_PULLUP);
  }

#endif

#ifdef USING_OCTAVE

  //////////////////////////////////////
  // Ocatve Button
  // Initialize buttons with pull up resistors
  for (int i = 0; i < 2; i++) {
    pinMode(OCTAVE_BUTTON_PIN[i], INPUT_PULLUP);
  }

#endif

#ifdef USING_NEOPIXEL

  pinMode(CHANNEL_BUTTON_PIN, INPUT_PULLUP);
#endif

#ifdef pin13  // Initialize pin 13 as an input
  pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif

#endif  //USING_BUTTONS
  ////////////////////////////////////


  //////////////////////////////////////
  // Potentiometer

#ifdef USING_POTENTIOMETERS

  // Initialize pots with pull up resistors
  //  for (int i = 0; i < N_POTS_ARDUINO; i++) {
  //    pinMode(POT_ARDUINO_PIN[i], INPUT_PULLUP);
  //
  //  }

  for (int i = 0; i < N_POTS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);
    responsivePot[i].setAnalogResolution(potMax + 1);  // sets the resolution
  }


#endif
  ////////////////////////////////////


  /////////////////////////////////////////////
  // Multiplexers

#ifdef USING_MUX

  // Initialize the multiplexers
  for (int i = 0; i < N_MUX; i++) {
    mux[i].begin();
  }
  //* Set each X pin as input_pullup (avoid floating behavior)
  pinMode(x1, INPUT_PULLUP);
  //  pinMode(x2, INPUT_PULLUP);
  //  pinMode(x3, INPUT_PULLUP);

#endif

  /////////////////////////////////////////////
  // Threads

#ifdef USING_POTENTIOMETERS

  // Potentiometers
  threadPotentiometers.setInterval(5);         // every how many millisiconds
  threadPotentiometers.onRun(potentiometers);  // the function that will be added to the thread
  cpu.add(&threadPotentiometers);              // add every thread here
#endif

#ifdef USING_BANKS_WITH_BUTTONS

  threadBanksWithButtons.setInterval(30);
  threadBanksWithButtons.onRun(banksWithButtons);
  cpu.add(&threadBanksWithButtons);
#endif

#ifdef USING_OCTAVE

  threadChangeOctave.setInterval(30);
  threadChangeOctave.onRun(changeOctave);
  cpu.add(&threadChangeOctave);
#endif

#ifdef USING_NEOPIXEL

  // Channel Menu
  threadChannelMenu.setInterval(40);     // every how many millisiconds
  threadChannelMenu.onRun(channelMenu);  // the function that will be added to the thread
  cpu.add(&threadChannelMenu);           // add every thread here
#endif


  /////////////////////////////////////////////
  // Encoders
#ifdef USING_ENCODER

  for (int i = 0; i < N_ENCODERS; i++) {  // if you want to start with a certain value use presets
    encoder[i].write(preset[0][i]);
  }

  for (int z = 0; z < N_ENCODER_MIDI_CHANNELS; z++) {
    for (int i = 0; i < N_ENCODERS; i++) {
      lastEncoderValue[z][i] = preset[z][i];
      encoderValue[z][i] = preset[z][i];
    }
  }

#endif

#ifdef USING_ENCODER_MCP23017

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C(0x20, &Wire1)) { // Wire1 or Wire
    //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("MCP23017 Error.");
    while (1)
      ;
  } else {
    Serial.println("MCP23017 Success.");
  }

  // configure MCU pin that will read INTA/B state
  pinMode(INT_PIN, INPUT_PULLUP);

  for (int i = 0; i < N_ENC_MCP23017; i++) {
    for (int j = 0; j < 2; j++) {
      mcp.pinMode(encoderPin[i][j], INPUT_PULLUP);
      mcp.setupInterruptPin(encoderPin[i][j], CHANGE);
    }
  }

  // OPTIONAL - call this to override defaults
  // mirror INTA/B so only one wire required
  // active drive so INTA/B will not be floating
  // INTA/B will be signaled with a LOW
  mcp.setupInterrupts(true, false, CHANGE);

  attachInterrupt(digitalPinToInterrupt(INT_PIN), readEncoder_MCP23017, CHANGE);
  mcp.clearInterrupts();  // clear

#endif  // USING_ENCODER_MCP23017


  /////////////////////////////////////////////
  // DISPLAY

#ifdef USING_DISPLAY

  delay(100);                                 // This delay is needed to let the display to initialize
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with the I2C address of 0x3C
  //display.begin(SSD1306_SWITCHCAPVCC, 0x7A);  // Initialize display with the I2C address of 0x3C
  display.clearDisplay();       // Clear the buffer
  display.setTextColor(WHITE);  // Set color of the text
  display.setRotation(0);       // Set orientation. Goes from 0, 1, 2 or 3
  display.setTextWrap(true);    // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
  // To override this behavior (so text will run off the right side of the display - useful for
  // scrolling marquee effects), use setTextWrap(false). The normal wrapping behavior is restored
  // with setTextWrap(true).
  display.dim(0);  //Set brightness (0 is maximun and 1 is a little dim)


#ifdef USING_BANKS_WITH_BUTTONS

  display.setTextSize(display_text_size);
  //  display.setFont(&FreeMonoBold12pt7b);  // Set a custom font
  //display.setFont(&FreeMono9pt7b);  // Set a custom font
  //display.setFont(&FreeSansBoldOblique9pt7b);  // Set a custom font

  byte tempChannel;

#ifdef BANKS_FOR_BUTTONS
  tempChannel = BUTTON_MIDI_CH;
#endif

#ifdef BANKS_FOR_POTS
  tempChannel = POT_MIDI_CH;
#endif

#ifdef BANKS_FOR_ENCODERS
  tempChannel = ENCODER_MIDI_CH; e
#endif

  printChannel(tempChannel);  // displays the MIDI channel in the display

#endif  // USING_BANKS_WITH_BUTTONS

#endif  // USING_DISPLAY

  /////////////////////////////////////////////
  // LEDs
#ifdef USING_LED_FEEDBACK  // if using the arduino pins for LEDs
#ifndef USING_74HC595      // if not using bit shifter

  for (int i = 0; i < numOutputs; i++) {
    pinMode(ledPin[i], OUTPUT);
  }

#endif
#endif

  /////////////////////////////////////////////
  // 74HC595
#ifdef USING_74HC595

  // Sets the number of 8-bit registers that are used.
  ShiftPWM.SetAmountOfRegisters(numRegisters);

  ShiftPWM.SetAll(0);

  // SetPinGrouping allows flexibility in LED setup.
  // If your LED's are connected like this: RRRRGGGGBBBBRRRRGGGGBBBB, use SetPinGrouping(4).
  ShiftPWM.SetPinGrouping(1);  //This is the default, but I added here to demonstrate how to use the funtion

  ShiftPWM.Start(pwmFrequency, maxBrightness);
  delay(200);

  ShiftPWM.SetAll(0);

  //ShiftPWM.SetAll(maxBrightness);

#endif  // USING_74HC595

  /////////////////////////////////////////////
  // MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS

  for (int i = 0; i < N_M_FADERS; i++) {
    pinMode(motorUpPin[i], OUTPUT);
    pinMode(motorDownPin[i], OUTPUT);
    pinMode(faderSpeedPin[i], OUTPUT);
    analogWrite(faderSpeedPin[i], 255);

    //capSense[i] = CapacitiveSensor(touchSendPin[i], touchReceivePin[i]);

    calibrateFader(i);
  }

#ifdef DEBUG
  Serial.println("Faders Calibrated");
  Serial.println();
#endif

#endif
}
