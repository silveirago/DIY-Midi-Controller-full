/*
  Made by Gustavo Silveira, 2025.

  http://www.musiconerd.com
  http://www.youtube.com/musiconerd
  http://facebook.com/musiconerdmusiconerd
  http://instagram.com/musiconerd/
  http://www.gustavosilveira.net
  gustavosilveira@musiconerd.com

  If you are using for anything that's not for personal use don't forget to give credit.

  PS: Just change the value that has a comment like " //* "

*/

/////////////////////////////////////////////
// Choosing your board
// Define your board, choose:
// "ATMEGA328" if using ATmega328 - Uno, Mega, Nano...
// "ATMEGA32U4" if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
// "TEENSY" if using a Teensy board
// "BLEMIDI" if using BLE MIDI (ESP32)
// "DEBUG" if you just want to debug the code in the serial monitor

#define ATMEGA32U4 1  //* put here the uC you are using, like in the lines above followed by "1", like "ATMEGA328 1", "DEBUG 1", etc.

/////////////////////////////////////////////
// Are you using buttons?
#define USING_BUTTONS 1  //* comment if not using buttons

/////////////////////////////////////////////
// Are you using potentiometers?
#define USING_POTENTIOMETERS 1  //* comment if not using potentiometers

// Are you using high res faders?
//#define USING_HIGH_RES_FADERS 1 //* comment if not using high res faders (any fader can be high res)

// Are you using Motorized Faders?
//#define USING_MOTORIZED_FADERS 1 //* comment if not using a motorized fader

/////////////////////////////////////////////
// Are you using a multiplexer?
//#define USING_MUX 1  //* comment if not using a multiplexer, uncomment if using it.

/////////////////////////////////////////////
// Are you using encoders?
//#define USING_ENCODER 1  //* comment if not using encoders, uncomment if using it.
//#define USING_ENCODER_MCP23017 1
//#define TRAKTOR 1 // uncomment if using with traktor, comment if not

// Are you using high res ENCODER?
//#define USING_HIGH_RES_ENC 1  //* comment if not using high res faders (any fader can be high res)

/////////////////////////////////////////////
// Are you using neopixels (any addressable strips)?
//#define USING_NEOPIXEL 1 //* comment if not using neopixels, uncomment if using it.

/////////////////////////////////////////////
// Are you using an I2C Oled Display?
//#define USING_DISPLAY 1 //* comment if not using an I2C Oled Display.

/////////////////////////////////////////////
// Are you using banks that can be switched with 2 buttons?
//#define USING_BANKS_WITH_BUTTONS 1 //* comment if not using banks with buttons.

//#define BANKS_FOR_BUTTONS 1
//#define BANKS_FOR_POTS 1
//#define BANKS_FOR_ENCODERS 1

// Are you using LED note feedback?
//#define USING_LED_FEEDBACK 1  //* comment if not using a VU

/////////////////////////////////////////////
// Are you using a 74HC595 Bit Shifter?
// Below, use either "USING_VU" or "USING_LED_FEEDBACK"
// Use VU to monitor audio levels, or LED feedback to get note feedback

//#define USING_74HC595 1 //* comment if not using 74HC595 Bit Shifter

// Are you using a VU (led meter)?
//#define USING_VU 1//* comment if not using a VU

// Are you using the Mackie Protocol?
//#define USING_MACKIE 1

// Are you using two buttons for octave change?
//#define USING_OCTAVE 1

// Are you using Physical MIDI din 5-pin connector?
//#define MIDI_DIN 1


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
// LIBRARIES
// -- Defines the MIDI library -- //

//////////////////////////////////////
// If using Fast Led
#ifdef USING_NEOPIXEL

#include "FastLED.h"  // by Daniel Garcia - http://fastled.io

FASTLED_USING_NAMESPACE

#define DATA_PIN 9
//#define CLK_PIN   4
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 4

CRGB leds[NUM_LEDS];
byte ledIndex[NUM_LEDS] = { 0, 1, 2, 3 };

#define BRIGHTNESS 96  // 0-255
#define FRAMES_PER_SECOND 120

#endif
//////////////////////////////////////

// if using with ATmega328 - Uno, Mega, Nano...
#ifdef ATMEGA328
#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();
#endif

// if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
#ifdef ATMEGA32U4
#include <MIDIUSB.h>

#ifdef MIDI_DIN
#include <MIDI.h>  // adds the MIDI library to use the hardware serial with a MIDI cable
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi2);
#endif  //MIDI_DIN

#endif  // ATMEGA32U4

#ifdef BLEMIDI
#include <BLEMidi.h>  // https://github.com/max22-/ESP32-BLE-MIDI
// Documentation: https://www.arduino.cc/reference/en/libraries/esp32-ble-midi/
char bleMIDIDeviceName[] = { "BLE Controller" };  // put here the name you want for your device
#endif

//////////////////////////////////////
// if using the 74HC595 bit shifter
#ifdef USING_74HC595

// You can choose the latch pin yourself.
const int ShiftPWM_latchPin = 18;

// ** uncomment this part to NOT use the SPI port and change the pin numbers. This is 2.5x slower **
#define SHIFTPWM_NOSPI
const int ShiftPWM_dataPin = 9;
const int ShiftPWM_clockPin = 19;


// If your LED's turn on if the pin is low, set this to true, otherwise set it to false.
const bool ShiftPWM_invertOutputs = false;

// You can enable the option below to shift the PWM phase of each shift register by 8 compared to the previous.
// This will slightly increase the interrupt load, but will prevent all PWM signals from becoming high at the same time.
// This will be a bit easier on your power supply, because the current peaks are distributed.
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>  // include ShiftPWM.h after setting the pins!
// Download: https://github.com/elcojacobs/ShiftPWM

#endif  // USING_74HC595

//////////////////////
// Add this lib if using a cd4067 multiplexer
#ifdef USING_MUX
#include <Multiplexer4067.h>  // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067
#endif

//////////////////////
// Threads
#include <Thread.h>            // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h>  // Same as above

//////////////////////
// Encoder
#ifdef USING_ENCODER
// In the downloads manager download the Encoder lib from Paul Stoffregen (it comes with the Teensy)
#include <Encoder.h>  // makes all the work for you on reading the encoder
#endif

#ifdef USING_ENCODER_MCP23017
#include <Adafruit_MCP23X17.h>  // Include the Adafruit_MCP23X17 library >> https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
//Adafruit_MCP23X17 mcp; // Create an instance of the Adafruit_MCP23X17 class
#endif

//////////////////////
// Oled Display I2C
#ifdef USING_DISPLAY
//#include <Adafruit_GFX.h>  // Include core graphics library for the display
#include <Adafruit_SSD1306.h>  // Include Adafruit_SSD1306 library to drive the display
//#include <Fonts/FreeMonoBold12pt7b.h>  // Add a custom font
//#include <Fonts/FreeMono9pt7b.h>  // Add a custom font
Adafruit_SSD1306 display(128, 64);  // Create display - size of the display in pixels
#endif

///////////////////////////////////////////
// MULTIPLEXERS
#ifdef USING_MUX

#define N_MUX 1  // number of multiplexers

// Define s0, s1, s2, s3, and x pins
const int mux_s[4] = { 7,6,5,4 };
const int mux_x[N_MUX] = { A2 };  // analog pin of each mux

// Declare an array of pointers (don't change)
Multiplexer4067* mux[N_MUX];

#endif

#ifdef USING_POTENTIOMETERS
// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h>  // https://github.com/dxinteractive/ResponsiveAnalogRead
#endif

#ifdef USING_MOTORIZED_FADERS
#include <CapacitiveSensor.h>  // Ads touch capabilities for the motorized fader
#endif

#ifdef USING_MACKIE
#include "MACKIE.h"
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Types of MIDI messages to send - DO NOT TOUCH
byte NN = 0;  // Note number or MACKIE
byte CC = 1;  // Control change
byte T = 2;   // Toggle
byte PC = 3;  // Program Change
byte PB = 4;  // Pitch Bend

/////////////////////////////////////////////
// BUTTONS
#ifdef USING_BUTTONS

const byte N_BUTTONS = 2 ;                         //*  total numbers of buttons. Number of buttons in the Arduino + number of buttons on multiplexer 1 + number of buttons on multiplexer 2... (DON'T put Octave and MIDI channel (bank) buttons here)
const byte N_BUTTONS_ARDUINO = 2;                     //* number of buttons connected straight to the Arduino
const byte BUTTON_ARDUINO_PIN[N_BUTTONS] = { 2, 3 };  //* pins of each button connected straight to the Arduino

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX                              // Fill if you are using mux, otherwise just leave it
const byte N_BUTTONS_PER_MUX[N_MUX] = { 3 };  //* number of buttons in each mux (in order)
const byte BUTTON_MUX_PIN[N_MUX][16] = {
  //* pin of each button of each mux in order

  { 2, 3, 4 },  // 1
  // { 2, 1, 0, 5, 4, 3, 7, 6, 9, 8, 13, 12, 15 },              // 2
  // { 6, 5, 4, 3, 8 },
  // ...
};

int buttonMuxThreshold = 850;

#endif  //USING_MUX

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// What type of message do you want to send?
// Note Number - Control Change - Toggle - Program Change

// NN: Note number or MACKIE
// CC: Control change
// T: Toggle
// PC: Program Change

//* Put here the type of message you want to send, in the same order you declared the button pins
// "NN" for Note Number | "CC" for Control Change | "T" for Note Number but in toggle mode | "PC" for Program Change
byte MESSAGE_TYPE[N_BUTTONS] = { NN, NN };

//* Put here the number of the message you want to send, in the right order, no matter if it's a note number, CC (or MACKIE), Program Change
byte MESSAGE_VAL[N_BUTTONS] = { 36, 37 };


// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_BANKS_WITH_BUTTONS

//#define USING_MUX_BANK_BUTTON_PIN 1; // Define if you are using the bank buttons on the Mux pin. It has to be the first mux.
const byte BANK_BUTTON_PIN[2] = { 16, 10 };  //* first will decrease MIDI chennel and second will increase

#endif  //USING_BANKS_WITH_BUTTONS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_OCTAVE

//#define USING_MUX_OCTAVE_PIN 1;              // Define if you are using the octave buttons on the Mux pin. It has to be the first mux.
const byte OCTAVE_BUTTON_PIN[2] = { 5, 4 };  //* first will decrease MIDI channel and second will increase

#endif  // USING_OCTAVE

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// it will happen if you are using neo pixel
// this button will open the menu so you can change the MIDI channel
// pressing one button. Each button will be one differente channel

#ifdef USING_NEOPIXEL
const byte CHANNEL_BUTTON_PIN = 8;
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// debounce
unsigned long debounceDelay = 50;  //* the debounce time; increase if the output flickers

#endif  // USING_BUTTONS


/////////////////////////////////////////////
// POTENTIOMETERS

#ifdef USING_POTENTIOMETERS

const byte N_POTS = 2;  //* total numbers of pots (slide & rotary). Number of pots in the Arduino + number of pots on multiplexer 1 + number of pots on multiplexer 2...

const byte N_POTS_ARDUINO = 2;  //* number of pots connected straight to the Arduino
// If using the Arduino declare as "A1, A2"
// If using ESP32 only use the GPIO number as "11, 10"
const byte POT_ARDUINO_PIN[N_POTS_ARDUINO] = { A0, A1 };  //* pins of each pot connected straight to the Arduino (don't use "A" if you are using ESP32, only the number)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX
const byte N_POTS_PER_MUX[N_MUX] = { 2 };  //* number of pots in each multiplexer (in order)
const byte POT_MUX_PIN[N_MUX][16] = {
  //* pins of each pot of each mux in the order you want them to be
  { 0, 1 },  //* pins of the first mux
  //{ 10, 11, 14 },
  //{ 7 }
  // ...
};
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define USING_CUSTOM_CC_N 1  //* comment if not using CUSTOM CC NUMBERS, uncomment if using it.
#ifdef USING_CUSTOM_CC_N

// What type of message do you want to send?
// Control Change - Pitch Bend

// CC: Control change
// PB: Pitch Bend

//* Put here the type of message you want to send, in the same order you declared the button pins
// "CC" for Control Change | "PB" for Pitch Bend
byte MESSAGE_TYPE_POT[N_POTS] = { CC, CC };

byte POT_CC_N[N_POTS] = { 1, 2 };  // Add the CC NUMBER or MACKIE of each pot you want

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const int TIMEOUT = 300;      //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const byte varThreshold = 10;  //* Threshold for the potentiometer signal variation - Increase if using ESP32
// For varThreshold I usually use "8" for normal Arduino and "30" for ESP32

// put here the min and max reading in the potCState
// in the potMin put a little bit more and in the potMax put a little bit less
// IMPORTANT:
// Regular Arduinos have 10 bit resolution: 0 - 1023
// ESP32 boards have 12 bit resolution: 0 - 4095
const int potMin = 20;
const unsigned int potMax = 940;

#endif  // USING_POTENTIOMETERS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/////////////////////////////////////////////
// ENCODERS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CALIBRATION
byte encoderMinVal = 0;  //* encoder minimum value

#ifdef USING_HIGH_RES_ENC
const unsigned int encoderMaxVal = 16383;  //* encoder max value HIGH RES 14bit
#else
const byte encoderMaxVal = 127;  //* encoder max value NORMAL MIDI 7bit
#endif

//#endif

// Encoder Sensitivity
// choose between a (float) number greater than 0 (0.2 for example) and 4.
// "1" will give you 4 readings per click, it will increament in 4, like 4, 8, 12...
// "4" will give you 1 reading per click, like 1, 2, 3, 4...
// so "4" will give the most precise reading
const float encSensitivity = 4;  //* change the encoder sensitivity here

const byte N_ENCODER_MIDI_CHANNELS = 16;  // number of ENCODER_MIDI_CHs (DO NOT CHANGE)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PINS

#ifdef USING_ENCODER

const byte N_ENCODERS = 1;                   //* number of encoders
const byte N_ENCODER_PINS = N_ENCODERS * 2;  //number of pins used by the encoders

//Encoder encoder[N_ENCODERS] = { { 10, 16 } };  // the two pins of each encoder -  Use pins with Interrupts!
Encoder encoder[N_ENCODERS] = { { 9, 8 } };  // the two pins of each encoder -  Use pins with Interrupts!

// Choose the CC or MACKIE value
byte ENCODER_CC_N[N_ENCODERS] = { 21 };  // Add the CC NUMBER of each encoder you want


// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

byte preset[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {
  //* stores presets to start your encoders
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MACKIE
byte encoderSens = 1;  // 1-63
#endif

#endif  //USING_ENCODER

/////////////////////////////////////////////
// ENCODER MCP23017


#ifdef USING_ENCODER_MCP23017

const int I2C_ADDRESS = 0x20;  //* MCP23017 I2C address

const byte N_ENC_MCP23017 = 6;      // Number of encoders used
const byte N_ENC_CH_MCP23017 = 16;  // number of ENCODER_MIDI_CHs

int encoderPin[N_ENC_MCP23017][2] = { { 8, 9 }, { 0, 1 }, { 12, 13 }, { 10, 11 }, { 2, 3 }, { 14, 15 } };  // Pin numbers for the A and B channels of each encoder
int INT_PIN = 8;                                                                                           // microcontroller pin attached to INTA/B

int count[N_ENC_MCP23017] = { 0 };      // Current count of each encoder
int lastCount[N_ENC_MCP23017] = { 0 };  // Previous count of each encoder

int encoderA[N_ENC_MCP23017] = { 0 };         // Current state of the A channel of each encoder
int encoderB[N_ENC_MCP23017] = { 0 };         // Current state of the B channel of each encoder
int lastEncoderA[N_ENC_MCP23017] = { HIGH };  // Previous state of the A channel of each encoder
int lastEncoderB[N_ENC_MCP23017] = { HIGH };  // Previous state of the B channel of each encoder

byte ENCODER_CC_N[N_ENC_MCP23017] = { 15, 16, 17, 18, 19, 20 };  // Add the CC NUMBER of each encoder you want

Adafruit_MCP23X17 mcp;  // Create an instance of the Adafruit_MCP23X17 class

#endif  //USING_ENCODER_MCP23017

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -


/////////////////////////////////////////////
// MIDI CHANNEL
byte POT_MIDI_CH = 0;  //* MIDI channel to be used
byte BUTTON_MIDI_CH = 0;
byte ENCODER_MIDI_CH = 1;

byte NOTE = 36;      //* Lowest NOTE to be used - if not using custom NOTE NUMBER
byte CC_NUMBER = 1;  //* Lowest MIDI CC to be used - if not using custom CC NUMBER

/////////////////////////////////////////////
// NEOPIXEL | MIDI CHANNEL MENU
#ifdef USING_NEOPIXEL

boolean channelMenuOn = false;
byte midiChMenuColor = 200;                 //* menu color HUE - 0-255
byte midiChOnColor = midiChMenuColor + 60;  // channel on menu color HUE
byte noteOffHue = 135;                      //* HUE when the notes are not played - 135 (blue)
byte noteOnHue = 240;                       //* HUE of the notes when they are played - 240 (magenta)

#endif  // USING_NEOPIXEL


/////////////////////////////////////////////
// DISPLAY
byte display_pos_x = 27;     // pos x
byte display_pos_y = 7;      // pos y
byte display_text_size = 7;  // text font size

#ifdef USING_LED_FEEDBACK
const byte numOutputs = 3;              // /* put here the number of LEDs you'll use
byte ledPin[numOutputs] = { 5, 6, 9 };  // LEDs pins (arduino or bit shifter)
#endif
/////////////////////////////////////////////
// 75HC595
#ifdef USING_74HC595

unsigned char maxBrightness = 255;  //*
unsigned int numRegisters = 2;      //*

unsigned char pwmFrequency = 75;
//unsigned int numOutputs = numRegisters * 8;
unsigned int numRGBLeds = numRegisters * 8 / 3;
unsigned int fadingMode = 0;  //start with all LED's off.

byte brightness = 0;

byte ledColor1 = 255;  // 0-255
byte ledColor2 = 100;
byte ledColor3 = 180;
//byte ledColor4 = 10;


/////////////////////////////////////////////
// VU
#ifdef USING_VU

const byte N_LED_PER_VU = 7;
byte VuL[N_LED_PER_VU] = { 0, 1, 2, 3, 4, 5, 6 };      // VU left pins
byte VuR[N_LED_PER_VU] = { 7, 8, 9, 10, 11, 12, 13 };  // VU right pins

byte VU_MIDI_CH = 10;  // channel to listen to the VU -1

byte vuLcc = 12;  // left CC
byte vuRcc = 13;  // right CC

#endif  //USING_VU


#endif  // USING_74HC595


/////////////////////////////////////////////
// MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS

//Arduino Pin Assignments
const byte N_M_FADERS = 1;
const byte motorDownPin[N_M_FADERS] = { 4 };   //H-Bridge control to make the motor go down
const byte motorUpPin[N_M_FADERS] = { 5 };     //H-Bridge control to make the motor go up
const byte faderSpeedPin[N_M_FADERS] = { 9 };  // motor speed pin (PWM)

byte faderSpeed[N_M_FADERS] = { 240 };  // 0-255
byte faderSpeedMin = 150;               // 0-255 - 140?
byte faderSpeedMax = 255;               // 0-255 -  250?

const byte motorStopPoint = 18;  // motor will stop X values before it reaches its goal. Increase to avoid jittery (it will be less precise).

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// variables you don't need to change
int faderPos[N_M_FADERS] = { 0 };   // position of the fader
int faderPPos[N_M_FADERS] = { 0 };  // previous position of the fader
int faderMax[N_M_FADERS] = { 0 };   //Value read by fader's maximum position (0-1023)
int faderMin[N_M_FADERS] = { 0 };   //Value read by fader's minimum position (0-1023)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Midi
byte pFaderInVal[16][N_M_FADERS] = { 0 };
byte pFaderPBInVal[N_M_FADERS] = { 0 };  // Pitch bend for Mackie

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Cap Sense
boolean isTouched[N_M_FADERS] = { false };   // Is the fader currently being touched?
boolean pIsTouched[N_M_FADERS] = { false };  // previous Is the fader currently being touched?

long touchLine[N_M_FADERS] = { 0 };

unsigned long capTimeNow[N_M_FADERS] = { 0 };

// Capacitive Sensor - Touch Pin
// 10M resistor between pins 7 & 8, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor capSense[N_M_FADERS] = {
  CapacitiveSensor(7, 8)
};  // capSense = CapacitiveSensor(7, 8)

const int touchSendPin[N_M_FADERS] = { 7 };     // Send pin for Capacitance Sensing Circuit (Digital 7)
const int touchReceivePin[N_M_FADERS] = { 8 };  // Receive pin for Capacitance Sensing Circuit (Digital 8)

byte capSenseSpeed = 15;   // number of samples
int capSensitivity = 150;  // touch threshold

int capTimeout = 500;

#endif  // USING_MOTORIZED_FADERS