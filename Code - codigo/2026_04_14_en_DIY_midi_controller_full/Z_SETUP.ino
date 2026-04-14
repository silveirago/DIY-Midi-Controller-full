void setup() {
  //////////////// Baud Rate Setup ////////////////
  // Default baud rate is 115200 for Hairless MIDI.
  // If using MIDI DIN (e.g., for ATmega328), set to 31250.
  int baudRate = 115200;
#ifdef MIDI_DIN
  baudRate = 31250;
#endif

  //////////////// ATMEGA32U4 Specific Setup ////////////////
#ifdef ATMEGA32U4
  // Debug: Wait for Serial connection
#ifdef DEBUG
  Serial.begin(baudRate);
  while (!Serial) {
    Serial.println("waiting...");
    delay(10);
  }
  Serial.println();
#endif

  // MIDI DIN on ATMEGA32U4
#ifdef MIDI_DIN
  Serial1.begin(31250);
  midi2.begin();
  // Configure MIDI input handling
  midi2.turnThruOff();
  midi2.setHandleControlChange(myHandleControlChange);
  midi2.setHandleNoteOn(myHandleNoteOn);
  midi2.setHandleNoteOff(myHandleNoteOff);
  midi2.setHandlePitchBend(myHandlePitchBend);
#endif
#endif  // ATMEGA32U4

  delay(100);  // Short delay after initial setup

  //////////////// ATMEGA328 Specific Setup ////////////////
#ifdef ATMEGA328
  // Increment MIDI channel numbers for pots, buttons, and encoders
  POT_MIDI_CH++;
  BUTTON_MIDI_CH++;
  ENCODER_MIDI_CH++;

  Serial.begin(baudRate);
  // Configure MIDI input handling
  MIDI.begin();
  MIDI.turnThruOff();
  MIDI.setHandleControlChange(myHandleControlChange);
  MIDI.setHandleNoteOn(myHandleNoteOn);
  MIDI.setHandleNoteOff(myHandleNoteOff);
  MIDI.setHandlePitchBend(myHandlePitchBend);
#endif

  //////////////// BLE MIDI Setup ////////////////
#ifdef BLEMIDI
  Serial.begin(baudRate);
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin(bleMIDIDeviceName);  // Set BLE MIDI device name
  Serial.println("Waiting for connections...");
  // Uncomment below to enable BLE MIDI debugging output:
  // BLEMidiServer.enableDebugging();
#endif

  //////////////// TEENSY Specific Setup ////////////////
#ifdef TEENSY
#ifdef DEBUG
  while (!Serial) {
    Serial.println("waiting...");
    delay(10);
  }
  Serial.println();
#endif
#endif

  //////////////// General Debug Serial Setup ////////////////
#ifdef DEBUG
  Serial.begin(baudRate);
  while (!Serial) {
    Serial.println("waiting...");
    delay(10);
  }
  Serial.println();
#endif

  //////////////// Neopixel / FastLED Setup ////////////////
#ifdef USING_NEOPIXEL
  // Initialize FastLED with defined LED type, data pin, and color order
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // Optionally, set master brightness: FastLED.setBrightness(BRIGHTNESS);
  // Set all LEDs to the note-off hue with some randomness
  setAllLeds(noteOffHue, 30);
  FastLED.show();
#endif

  //////////////// Button Setup ////////////////
#ifdef USING_BUTTONS
  // Initialize Arduino buttons with internal pull-up resistors
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef USING_BANKS_WITH_BUTTONS
  // Initialize bank buttons (2 total) with pull-ups
  for (int i = 0; i < 2; i++) {
    pinMode(BANK_BUTTON_PIN[i], INPUT_PULLUP);
  }
#endif

#ifdef USING_OCTAVE
  // Initialize octave buttons (2 total) with pull-ups
  for (int i = 0; i < 2; i++) {
    pinMode(OCTAVE_BUTTON_PIN[i], INPUT_PULLUP);
  }
#endif

#ifdef USING_NEOPIXEL
  // Initialize channel button for Neopixel with pull-up
  pinMode(CHANNEL_BUTTON_PIN, INPUT_PULLUP);
#endif

#ifdef pin13  // If pin13 is defined, set it as an input for additional button functionality
  pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif

#endif  // USING_BUTTONS

  //////////////// Potentiometer Setup ////////////////
#ifdef USING_POTENTIOMETERS
  // Initialize each potentiometer using ResponsiveAnalogRead
  for (int i = 0; i < N_POTS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);
    responsivePot[i].setAnalogResolution(potMax + 1);  // Set resolution
  }
#endif

  //////////////// Multiplexer Setup ////////////////
#ifdef USING_MUX
  // Dynamically allocate Multiplexer4067 objects
  for (int i = 0; i < N_MUX; i++) {
    mux[i] = new Multiplexer4067(mux_s[0], mux_s[1], mux_s[2], mux_s[3], mux_x[i]);
  }
  // Initialize each multiplexer and set its signal pin to INPUT_PULLUP
  for (int i = 0; i < N_MUX; i++) {
    mux[i]->begin();
    pinMode(mux_x[i], INPUT_PULLUP);
  }
#endif  // USING_MUX

  //////////////// Thread Setup ////////////////
#ifdef USING_POTENTIOMETERS
  // Thread for reading potentiometers
  threadPotentiometers.setInterval(5);         // Run every 5 ms
  threadPotentiometers.onRun(potentiometers);
  cpu.add(&threadPotentiometers);
#endif

#ifdef USING_BANKS_WITH_BUTTONS
  // Thread for handling bank button functionality
  threadBanksWithButtons.setInterval(30);
  threadBanksWithButtons.onRun(banksWithButtons);
  cpu.add(&threadBanksWithButtons);
#endif

#ifdef USING_OCTAVE
  // Thread for handling octave change
  threadChangeOctave.setInterval(30);
  threadChangeOctave.onRun(changeOctave);
  cpu.add(&threadChangeOctave);
#endif

#ifdef USING_NEOPIXEL
  // Thread for channel menu handling in Neopixel mode
  threadChannelMenu.setInterval(40);  // Run every 40 ms
  threadChannelMenu.onRun(channelMenu);
  cpu.add(&threadChannelMenu);
#endif

  //////////////// Encoder Setup ////////////////
#ifdef USING_ENCODER
  // Initialize encoder values from presets
  for (int i = 0; i < N_ENCODERS; i++) {
    encoder[i].write(preset[0][i]);
  }
  // Copy preset values for each encoder and each MIDI channel
  for (int z = 0; z < N_ENCODER_MIDI_CHANNELS; z++) {
    for (int i = 0; i < N_ENCODERS; i++) {
      lastEncoderValue[z][i] = preset[z][i];
      encoderValue[z][i] = preset[z][i];
    }
  }
#endif

#ifdef USING_ENCODER_MCP23017
  // Initialize MCP23017 for encoder handling (I2C or SPI)
  if (!mcp.begin_I2C(0x20, &Wire)) {  // Use appropriate I2C instance or SPI (uncomment if needed)
    Serial.println("MCP23017 Error.");
  } else {
    Serial.println("MCP23017 Success.");
  }

  // Configure the interrupt pin for the MCP23017
  pinMode(INT_PIN, INPUT_PULLUP);

  // Set up each encoder pin on the MCP23017 with pull-ups and interrupt on change
  for (int i = 0; i < N_ENC_MCP23017; i++) {
    for (int j = 0; j < 2; j++) {
      mcp.pinMode(encoderPin[i][j], INPUT_PULLUP);
      mcp.setupInterruptPin(encoderPin[i][j], CHANGE);
    }
  }
  // Optional: Mirror INTA/B to use only one wire and set active drive
  mcp.setupInterrupts(true, false, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), readEncoder_MCP23017, CHANGE);
  mcp.clearInterrupts();  // Clear any pending interrupts
#endif  // USING_ENCODER_MCP23017

  //////////////// Display Setup ////////////////
#ifdef USING_DISPLAY
  delay(100);  // Delay to allow display initialization
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with I2C address 0x3C
  display.clearDisplay();       // Clear display buffer
  display.setTextColor(WHITE);  // Set text color
  display.setRotation(0);       // Set orientation (0-3)
  display.setTextWrap(true);    // Enable text wrapping
  display.dim(0);               // Set brightness (0 = max)

#ifdef USING_BANKS_WITH_BUTTONS
  display.setTextSize(display_text_size);
  // Optionally set a custom font:
  // display.setFont(&FreeMonoBold12pt7b);
  // display.setFont(&FreeMono9pt7b);
  // display.setFont(&FreeSansBoldOblique9pt7b);

  byte tempChannel;
#ifdef BANKS_FOR_BUTTONS
  tempChannel = BUTTON_MIDI_CH;
#endif
#ifdef BANKS_FOR_POTS
  tempChannel = POT_MIDI_CH;
#endif
#ifdef BANKS_FOR_ENCODERS
  tempChannel = ENCODER_MIDI_CH;
#endif
  printChannel(tempChannel);  // Display current MIDI channel
#endif  // USING_BANKS_WITH_BUTTONS
#endif  // USING_DISPLAY

  //////////////// LED Feedback Setup (Direct Arduino Pins) ////////////////
#ifdef USING_LED_FEEDBACK
#ifndef USING_74HC595
  // Set LED output pins
  for (int i = 0; i < numOutputs; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
#endif
#endif

  //////////////// 74HC595 (Shift Register) Setup ////////////////
#ifdef USING_74HC595
  // Configure the number of shift registers and initialize them
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetAll(0);
  // Set pin grouping if needed (e.g., for grouped LED arrangements)
  ShiftPWM.SetPinGrouping(1);  // Default grouping is 1
  ShiftPWM.Start(pwmFrequency, maxBrightness);
  delay(200);
  ShiftPWM.SetAll(0);
  // Optionally, set all outputs to maximum brightness:
  // ShiftPWM.SetAll(maxBrightness);
#endif  // USING_74HC595

  //////////////// Motorized Faders Setup ////////////////
#ifdef USING_MOTORIZED_FADERS
  // Initialize motorized faders and calibrate each one
  for (int i = 0; i < N_M_FADERS; i++) {
    pinMode(motorUpPin[i], OUTPUT);
    pinMode(motorDownPin[i], OUTPUT);
    pinMode(faderSpeedPin[i], OUTPUT);
    analogWrite(faderSpeedPin[i], 255);  // Set default speed
    calibrateFader(i);  // Calibrate fader position
  }

#ifdef DEBUG
  Serial.println("Faders Calibrated");
  Serial.println();
#endif
#endif  // USING_MOTORIZED_FADERS
}  // end of setup()
