# DIY-Midi-Controller-full

   Made by Gustavo Silveira, 2021.
  - This Sketch rmakes the Arduino a full MIDI controller.

  http://go.musiconerd.com
  http://www.youtube.com/musiconerd
  http://facebook.com/musiconerdmusiconerd
  http://instagram.com/musiconerd/
  http://www.gustavosilveira.net
  gustavosilveira@musiconerd.com

  If you are using for anything that's not for personal use don't forget to give credit.
 
    - Update 1 (03/12/19): You can add multiple multiplexers for potentiometers.
    - Update 2 Threads added. it makes your MIDI controller faster.
    - Update 3 Encoder functionality added, using a super handy library.
    - Update 4 (03/18/19): Multiple encoders tested and working.
    - Update 5 (03/19/19): Multiple encoders (with banks) tested and working.
    - Update 6 (06/01/20): Custom note numbers and CC
    - Update 7 (06/02/20): Velocity Variable
    - Update 8 (06/02/20): Buttons using CC
    - Update 9 (06/08/20): Toggle mode added
    - Update 10 (11/17/20): Addressable led Midi feedback with FastLed lib (ATmega32U4 only) 
    - Update 11 (03/14/21): Tabs; Oled displays, 75HC595 Bit shifter for LED feedback and VU; and many fixes.
    - Update 12 (06/25/21): Better pot reading; High Res Faders; Motorized Faders.
    - Update 13 (01/09/21): Custom Note Numbers, CC and Toggle: Now you can use them at the same time with different buttons.  
    - Update 14 (01/09/21): How to change octaves and show it on the display.
    - Update 15 (01/04/22): Motorized Faders
    - Update 16 (01/08/22): MIDI Din
    - Update 17 (01/11/22): BLE MIDI
    - Update 18 (04/14/23): More MIDI Messages
    - Update 19 (04/14/23): LED MIDI In in the Arduino (w/o bit shifter)
    - Update 20 (04/14/23): Encoder Sensitivity
    - Update 21 (04/14/23): Encoder High Resolution
    - Update 22 (04/14/23): MCP23017 Encoder "multiplexing"

    ## Introduction

This guide provides detailed instructions on how to build and use a DIY MIDI controller using the provided Arduino sketches.

## Hardware Requirements

- Arduino Board (ATmega328, ATmega32U4, Teensy)
- Potentiometers
- Encoders
- Buttons
- MCP23017 (for additional I/O)
- Neopixel LEDs
- OLED Display
- Motorized Faders (optional)
- Breadboard and jumper wires

## Software Requirements

- Arduino IDE
- Required Libraries:
  - `Adafruit_NeoPixel`
  - `Wire`
  - `MIDI`

## Installation Guide

1. **Download the Code**: Clone or download the repository to your local machine.

   ```sh
   git clone https://github.com/yourusername/DIY_MIDI_Controller.git
   ```

2. **Install Arduino IDE**: If not already installed, download and install the Arduino IDE from the [official website](https://www.arduino.cc/en/software).

3. **Install Required Libraries**: Open the Arduino IDE and go to `Sketch` -> `Include Library` -> `Manage Libraries...`. Search and install the required libraries mentioned above.

4. **Upload Code to Arduino**: Open the `.ino` files in the Arduino IDE and upload them to your Arduino board.

## Code Explanation

- **2023.08.09_en_DIY_midi_controller_full.ino**: The main sketch that initializes and manages the MIDI controller.
- **A__EXTRA.ino**: Additional functions and features.
- **A_ENCODERS.ino**: Code handling the encoder inputs.
- **A_ENCODERS_MCP23017.ino**: Handling encoders with the MCP23017 expander.
- **A_POTENTIOMETERS.ino**: Code for potentiometer inputs.
- **B_BUTTONS.ino**: Handling button inputs.
- **BLEMIDI_input.ino**: Code for Bluetooth MIDI input.
- **C_BANKS.ino**: Code for bank switching.
- **C_OCTAVE.ino**: Code for octave switching.
- **LOOP.ino**: Main loop handling MIDI messages.
- **MACKIE.h**: Header file for Mackie control functions.
- **MOTORIZED_FADERS.ino**: Code for motorized faders (optional).
- **NEOPIXEL.ino**: Code for controlling Neopixel LEDs.
- **OLED_DISPLAY.ino**: Code for OLED display.
- **VU.ino**: Code for VU meter.
- **Z_ATMEGA328.ino**, **Z_ATMEGA32U4.ino**, **Z_SETUP.ino**, **Z_TEENSY.ino**: Specific setup codes for different microcontrollers.

## Usage

1. **Connect the Hardware**: Connect all the required components (potentiometers, encoders, buttons, etc.) to your Arduino board as per the provided pin configuration.

2. **Power the Arduino**: Connect the Arduino to your computer or a power source.

3. **Run the Code**: Open the Arduino IDE, select your board and port, and upload the main sketch.

4. **MIDI Configuration**: Configure your DAW (Digital Audio Workstation) to recognize the MIDI signals from the Arduino.

## Troubleshooting

- **Arduino Not Recognized**: Ensure the correct board and port are selected in the Arduino IDE.
- **Libraries Not Found**: Verify that all required libraries are installed correctly.
- **No MIDI Signal**: Check connections and ensure the MIDI settings in your DAW are configured properly.

## Contributing

If you would like to contribute to this project, please fork the repository and submit a pull request. Contributions can include code improvements, bug fixes, documentation, and new features.

---

This is a basic structure for the GitHub Wiki. You can expand each section with more detailed information, diagrams, and code snippets as needed.
