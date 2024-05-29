# DIY-Midi-Controller-full

   Made by Gustavo Silveira, 2024.
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

## Customizing the Code

### Board Selection

Choose your board by defining it. Uncomment and set the appropriate board definition in the code:

```cpp
#define ATMEGA328 1  // Uncomment if using ATmega328 - Uno, Mega, Nano...
//#define ATMEGA32U4 1  // Uncomment if using ATmega32U4 - Micro, Pro Micro, Leonardo...
//#define TEENSY 1  // Uncomment if using a Teensy board
//#define BLEMIDI 1  // Uncomment if using BLE MIDI (ESP32)
//#define DEBUG 1  // Uncomment if you just want to debug the code in the serial monitor
```

### Component Usage

Uncomment or comment the following lines based on the components you are using:

```cpp
#define USING_BUTTONS 1  // Comment if not using buttons
#define USING_POTENTIOMETERS 1  // Comment if not using potentiometers
//#define USING_HIGH_RES_FADERS 1  // Comment if not using high res faders
//#define USING_MOTORIZED_FADERS 1  // Comment if not using motorized faders
//#define USING_MUX 1  // Uncomment if using a multiplexer
#define USING_ENCODER 1  // Comment if not using encoders
//#define USING_ENCODER_MCP23017 1  // Uncomment if using encoders with MCP23017
//#define TRAKTOR 1  // Uncomment if using with Traktor
//#define USING_NEOPIXEL 1  // Uncomment if using neopixels
//#define USING_DISPLAY 1  // Uncomment if using an I2C Oled Display
//#define USING_BANKS_WITH_BUTTONS 1  // Uncomment if using banks with buttons
//#define USING_LED_FEEDBACK 1  // Uncomment if using LED feedback
//#define USING_74HC595 1  // Uncomment if using 74HC595 Bit Shifter
//#define USING_VU 1  // Uncomment if using a VU meter
//#define USING_MACKIE 1  // Uncomment if using the Mackie Protocol
//#define USING_OCTAVE 1  // Uncomment if using octave change buttons
//#define MIDI_DIN 1  // Uncomment if using Physical MIDI din 5-pin connector
```

### Pin Configuration

Configure the pins for your components. Below are some examples:

#### Buttons

```cpp
const byte N_BUTTONS = 3;  // Total number of buttons
const byte BUTTON_ARDUINO_PIN[N_BUTTONS] = { 2, 3, 4 };  // Pins for each button
byte MESSAGE_TYPE[N_BUTTONS] = { NN, NN, NN };  // Message type for each button (NN, CC, T, PC)
byte MESSAGE_VAL[N_BUTTONS] = { 36, 37, 38 };  // Message value for each button
```

#### Potentiometers

```cpp
const byte N_POTS = 2;  // Total number of pots
const byte POT_ARDUINO_PIN[N_POTS] = { A2, A1 };  // Pins for each pot
byte MESSAGE_TYPE_POT[N_POTS] = { CC, PB };  // Message type for each pot (CC, PB)
byte POT_CC_N[N_POTS] = { 1, 2 };  // CC number for each pot
```

#### Encoders

```cpp
const byte N_ENCODERS = 2;  // Total number of encoders
Encoder encoder[N_ENCODERS] = { { 10, 16 }, { 14, 15 } };  // Pins for each encoder
byte ENCODER_CC_N[N_ENCODERS] = { 11, 12 };  // CC number for each encoder
```
## Preset Configuration for Encoders

The `preset` array stores initial values for the encoders across different MIDI channels. This allows you to start your encoders with specific values.

### Code Explanation

```cpp
byte preset[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {
  // stores presets to start your encoders
  //  {64, 64}, // ch 1
  //  {64, 64}, // ch 2
  //  {64, 64}, // ch 3
  //  {64, 64}, // ch 4
  //  {64, 64}, // ch 5
  //  {64, 64}, // ch 6
  //  {64, 64}, // ch 7
  //  {64, 64}, // ch 8
  //  {64, 64}, // ch 9
  //  {64, 64}, // ch 10
  //  {64, 64}, // ch 11
  //  {64, 64}, // ch 12
  //  {64, 64}, // ch 13
  //  {64, 64}, // ch 14
  //  {64, 64}, // ch 15
  //  {64, 64}  // ch 16
};
```

### How to Customize

1. **Define the Presets**: Replace the values within the curly braces with your desired starting values for each encoder. Each inner array corresponds to a MIDI channel, and each value within the inner array corresponds to an encoder.

Example:
```cpp
byte preset[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {
  {0, 127}, // ch 1
  {10, 120}, // ch 2
  {20, 110}, // ch 3
  {30, 100}, // ch 4
  {40, 90}, // ch 5
  {50, 80}, // ch 6
  {60, 70}, // ch 7
  {70, 60}, // ch 8
  {80, 50}, // ch 9
  {90, 40}, // ch 10
  {100, 30}, // ch 11
  {110, 20}, // ch 12
  {120, 10}, // ch 13
  {127, 0}, // ch 14
  {64, 64}, // ch 15
  {64, 64}  // ch 16
};
```

## MCP23017 Encoder Configuration

If you are using encoders with the MCP23017 I/O expander, configure the settings as follows:

### Code Explanation

```cpp
#ifdef USING_ENCODER_MCP23017

const int I2C_ADDRESS = 0x20;  // MCP23017 I2C address

const byte N_ENC_MCP23017 = 6;      // Number of encoders used
const byte N_ENC_CH_MCP23017 = 16;  // Number of ENCODER_MIDI_CHs

int encoderPin[N_ENC_MCP23017][2] = {{8, 9}, {0, 1}, {12, 13}, {10, 11}, {2, 3}, {14, 15}};  // Pin numbers for the A and B channels of each encoder
int INT_PIN = 8;  // Microcontroller pin attached to INTA/B

int count[N_ENC_MCP23017] = { 0 };      // Current count of each encoder
int lastCount[N_ENC_MCP23017] = { 0 };  // Previous count of each encoder

int encoderA[N_ENC_MCP23017] = { 0 };         // Current state of the A channel of each encoder
int encoderB[N_ENC_MCP23017] = { 0 };         // Current state of the B channel of each encoder
int lastEncoderA[N_ENC_MCP23017] = { HIGH };  // Previous state of the A channel of each encoder
int lastEncoderB[N_ENC_MCP23017] = { HIGH };  // Previous state of the B channel of each encoder

byte ENCODER_CC_N[N_ENC_MCP23017] = { 15, 16, 17, 18, 19, 20 };  // Add the CC NUMBER of each encoder you want

Adafruit_MCP23X17 mcp;  // Create an instance of the Adafruit_MCP23X17 class

#endif  // USING_ENCODER_MCP23017
```

### How to Customize

1. **Set the I2C Address**: Change the `I2C_ADDRESS` if your MCP23017 uses a different address.

2. **Define Encoder Pins**: Adjust the `encoderPin` array to match the pins you have connected your encoders to. Each inner array should contain the pin numbers for the A and B channels of an encoder.

Example:
```cpp
int encoderPin[N_ENC_MCP23017][2] = {
  {8, 9},
  {0, 1},
  {12, 13},
  {10, 11},
  {2, 3},
  {14, 15}
};
```

3. **Set Interrupt Pin**: Define the `INT_PIN` to the microcontroller pin connected to the MCP23017 interrupt output.

4. **Initialize Encoder States**: Initialize `count`, `lastCount`, `encoderA`, `encoderB`, `lastEncoderA`, and `lastEncoderB` arrays as shown.

5. **Assign CC Numbers**: Assign the desired MIDI CC numbers to the `ENCODER_CC_N` array for each encoder.

Example:
```cpp
byte ENCODER_CC_N[N_ENC_MCP23017] = { 15, 16, 17, 18, 19, 20 };
```

---

This concludes the instructions for configuring encoder presets and setting up MCP23017 encoders.


#### Multiplexers

```cpp
#define N_MUX 1  // Number of multiplexers
#define s0 18
#define s1 19
#define s2 20
#define s3 21
#define x1 A0  // Analog pin of the first mux
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1),
  // Add more if you have additional muxes
};
```

### Usage

1. **Connect the Hardware**: Connect all the required components (potentiometers, encoders, buttons, etc.) to your Arduino board as per the provided pin configuration.

2. **Power the Arduino**: Connect the Arduino to your computer or a power source.

3. **Run the Code**: Open the Arduino IDE, select your board and port, and upload the main sketch.

4. **MIDI Configuration**: Configure your DAW (Digital Audio Workstation) to recognize the MIDI signals from the Arduino.

### Troubleshooting

- **Arduino Not Recognized**: Ensure the correct board and port are selected in the Arduino IDE.
- **Libraries Not Found**: Verify that all required libraries are installed correctly.
- **No MIDI Signal**: Check connections and ensure the MIDI settings in your DAW are configured properly.

## Contributing

If you would like to contribute to this project, please fork the repository and submit a pull request. Contributions can include code improvements, bug fixes, documentation, and new features.

---

This is a basic structure for the GitHub Wiki. You can expand each section with more detailed information, diagrams, and code snippets as needed.


## Troubleshooting

- **Arduino Not Recognized**: Ensure the correct board and port are selected in the Arduino IDE.
- **Libraries Not Found**: Verify that all required libraries are installed correctly.
- **No MIDI Signal**: Check connections and ensure the MIDI settings in your DAW are configured properly.

## Contributing

If you would like to contribute to this project, please fork the repository and submit a pull request. Contributions can include code improvements, bug fixes, documentation, and new features.

---

This is a basic structure for the GitHub Wiki. You can expand each section with more detailed information, diagrams, and code snippets as needed.
