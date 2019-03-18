/*
  Made by Gustavo Silveira, 2019.
  - This Sketch reads the Arduino's digital and analog ports and send midi notes and midi control change
  - It can use many multiplexers

  http://www.musiconerd.com
  http://www.youtube.com/musiconerd
  http://facebook.com/musiconerdmusiconerd
  http://instagram.com/musiconerd/
  http://www.gustavosilveira.net
  gustavosilveira@musiconerd.com

  If you are using for anything that's not for personal use don't forget to give credit.

*/

/////////////////////////////////////////////
// choosing your board

// Define your board. Choose:
// "ATMEGA328" if using ATmega328 - Uno, Mega, Nano...
// "ATMEGA32U4" if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
// "TEENSY" if using a Teensy board
// "DEBUG" if you just want to debug the code in the serial monitor

#define DEBUG // put here the uC you are using, like in the lines above

// defines the MIDI library
#ifdef ATMEGA328
#include <MIDI.h> // Use if using with ATmega328 - Uno, Mega, Nano...
MIDI_CREATE_DEFAULT_INSTANCE();

#elif ATMEGA32U4
#include "MIDIUSB.h" // Use if using with ATmega32U4 - Micro, Pro Micro, Leonardo...

#endif

/////////////////////////////////////////////
// Use if using a cd4067 multiplexer
#include <Multiplexer4067.h> // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067

// Threads
#include <Thread.h> // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h> // Same as above

// Encoder
// In the downloads manager download the Encoder lib from Paul Stoffregen (it comes with the Teensy)
#include <Encoder.h>  //makes all the work for you on reading the encoder

///////////////////////////////////////////
// Multiplexer
#define N_MUX 2 // number of multiplexers
// Define s0, s1, s2, s2, and x
#define s0 2
#define s1 3
#define s2 4
#define s3 5
#define x1 A1 // analog pin of the first mux
#define x2 A2 /// analog pin of the x mux...
// add more #define and the x number if you need

// Initializes the multiplexer
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1),
  Multiplexer4067(s0, s1, s2, s3, x2)
};

/////////////////////////////////////////////
// buttons
const int N_BUTTONS = 1 + 2 + 2; //  total numbers of buttons. Number of buttons in the Arduino + number of buttons on multiplexer 1 + number of buttons on multiplexer 2...
const int N_BUTTONS_ARDUINO = 1; // number of buttons connected straight to the Arduino (in order)
const int N_BUTTONS_PER_MUX[N_MUX] = {2, 2}; // number of buttons in each mux (in order)

const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {7}; // pins of each button connected straigh to the Arduino
int BUTTON_MUX_PIN[N_MUX][16] = {//pin of each buttons of each mux in order
  {2, 3},
  {1, 2}
}; 

int buttonCState[N_BUTTONS] = {};         // stores the button current value
int buttonPState[N_BUTTONS] = {};        // stores the button previous value
//byte pin13index = 3; // put the index of the pin 13 in the BUTTON_MUX_PIN[] if you are using it, if not, comment lines 68-70

/////////////////////////////////////////////
// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 5;    //* the debounce time; increase if the output flickers

/////////////////////////////////////////////
// potentiometers
const int N_POTS = 1 + 2 + 1; //* total numbers of pots (slide & rotary). Number of pots in the Arduino + number of pots on multiplexer 1 + number of pots on multiplexer 2...
const int N_POTS_ARDUINO = 1; //* number of pots connected straight to the Arduino (in order)
const int N_POTS_PER_MUX[N_MUX] = {2, 1}; //* number of pots in each multiplexer (in order)

const int POT_ARDUINO_PIN[N_POTS_ARDUINO] = {A0}; //* pins of each pot connected straigh to the Arduino
const int POT_MUX_PIN[N_MUX][16] = { //* pins of each pot of each mux in the order you want them to be
  {0, 1}, 
  {0}
  }; 

int potCState[N_POTS] = {0}; // Current state of the pot
int potPState[N_POTS] = {0}; // Previous state of the pot
int potVar = 0; // Difference between the current and previous state of the pot

int midiCState[N_POTS] = {0}; // Current state of the midi value
int midiPState[N_POTS] = {0}; // Previous state of the midi value

int TIMEOUT = 300; //* Amount of time the potentiometer will be read after it exceeds the varThreshold
int varThreshold = 10; //* Threshold for the potentiometer signal variation
boolean potMoving = true; // If the potentiometer is moving
unsigned long PTime[N_POTS] = {0}; // Previously stored time
unsigned long timer[N_POTS] = {0}; // Stores the time that has elapsed since the timer was reset

/////////////////////////////////////////////
// Encoders
// You can add as many encoders you want separated in many banks you want
const int N_ENCODERS = 8; //* number of encoders
const int N_ENCODER_PINS = N_ENCODERS * 2; //number of pins used by the encoders
const int N_ENCODER_BANKS = 2; //* number of banks
//this array stores the two pins of every encoder -  Use pins with Interrupts
Encoder encoder[N_ENCODERS] = {{2, 3}, {4, 5}, {6, 7}, {18, 9}, {10, 19}, {12, 20}, {14, 15}, {16, 17}};
int lastEncoderValue[N_ENCODER_BANKS][N_ENCODERS] = {127};
int encoderValue[N_ENCODER_BANKS][N_ENCODERS] = {127};

int preset[N_ENCODER_BANKS][N_ENCODERS] = { //stores presets
  {255, 0, 0, 0, 255, 0, 0, 0},
  {255, 0, 0, 0, 0, 0, 0, 255}
};

// for the encoder banks
int Bank = 0;
int lastBank = 0;
/////////////////////////////////////////////

byte midiCh = 1; //* MIDI channel to be used
byte note = 24; //* Lowest note to be used
byte cc = 11; //* Lowest MIDI CC to be used

/////////////////////////////////////////////
// Threads
// This libs create a "fake" thread. This means you can make something happen every x milisseconds
// We can use that to read something in an interval, instead of reading it every single loop
// In this case we'll read the potentiometers in a thread, making the reading of the buttons faster
ThreadController cpu; //thread master, where the other threads will be added
Thread threadPotentiometers; // thread to control the pots

void setup() {

  // Baud Rate
  // use if using with ATmega328 (uno, mega, nano...)
  // 31250 for MIDI class compliant | 115200 for Hairless MIDI
  Serial.begin(115200);

#ifdef DEBUG
  Serial.println("Debug mode");
#endif

  // Buttons
  // Initialize buttons with pull up resistors
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }
  //pinMode(BUTTON_MUX_PIN[3], INPUT); //pin 13

  /////////////////////////////////////////////
  // Multiplexers
  // Initialize the multiplexers
  for (int i = 0; i < N_MUX; i++) {
    mux[i].begin();
  }
  // Set each X pin as input_pullup (avoid floating behavior)
  pinMode(x1, INPUT_PULLUP);
  pinMode(x2, INPUT_PULLUP);

  /////////////////////////////////////////////
  // Threads
  threadPotentiometers.setInterval(10); // every how many millisiconds
  threadPotentiometers.onRun(potentiometers); // the function that will be added to the thread
  cpu.add(&threadPotentiometers); // add every thread here

  /////////////////////////////////////////////
  // Encoders
  for (int i = 0; i < N_ENCODERS; i++) { // if you want to start with a certain value use presets
    encoder[i].write(preset[0][i]);
  }

  for (int z = 0; z < N_ENCODER_BANKS; z++) {
    for (int i = 0; i < N_ENCODERS; i++) {
      lastEncoderValue[z][i] = preset[z][i];
      encoderValue[z][i] = preset[z][i];
    }
  }
  /////////////////////////////////////////////

}

void loop() {

  //cpu.run();
  buttons();
  // encoders();
  potentiometers();

}

/////////////////////////////////////////////
// BUTTONS
void buttons() {

  // read pins from arduino
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }

  //reads all the buttons of all the boards and stores in potCState
  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO;

  // read pins from mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
      // Scale values to 0-1
      if (buttonCState[i + nButtonsPerMuxSum] > 500) {
        buttonCState[i + nButtonsPerMuxSum] = HIGH;
      }
      else {
        buttonCState[i + nButtonsPerMuxSum] = LOW;
      }
    }
    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
  }


  for (int i = 0; i < N_BUTTONS; i++) {
    /*
        // Comment this if you are not using pin 13...
        if (i == pin13index) {
          buttonCState[i] = !buttonCState[i]; //inverts pin 13 because it has a pull down resistor instead of a pull up
        }
        // ...until here
    */
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {

          // Sends the MIDI note ON accordingly to the chosen board
#ifdef ATMEGA328
          // use if using with ATmega328 (uno, mega, nano...)
          MIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel

#elif ATMEGA32U4
          // use if using with ATmega32U4 (micro, pro micro, leonardo...)
          noteOn(midiCh, note + i, 127);  // channel, note, velocity
          MidiUSB.flush();

#elif TEENSY
          //do usbMIDI.sendNoteOn if using with Teensy
          usbMIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel

#else DEBUG
          Serial.print(i);
          Serial.println(": button on");
#endif

        }
        else {

          // Sends the MIDI note OFF accordingly to the chosen board
#ifdef ATMEGA328
          // use if using with ATmega328 (uno, mega, nano...)
          MIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel

#elif ATMEGA32U4
          // use if using with ATmega32U4 (micro, pro micro, leonardo...)
          noteOn(midiCh, note + i, 0);  // channel, note, velocity
          MidiUSB.flush();

#elif TEENSY
          //do usbMIDI.sendNoteOn if using with Teensy
          usbMIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel

#else DEBUG
          Serial.print(i);
          Serial.println(": button off");
#endif

        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

/////////////////////////////////////////////
// POTENTIOMETERS
void potentiometers() {

//    // read pins from arduino
//  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
//    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
//  }
//
//  //reads all the buttons of all the boards and stores in potCState
//  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO;
//
//  // read pins from mux
//  for (int j = 0; j < N_MUX; j++) {
//    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
//      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
//      // Scale values to 0-1
//      if (buttonCState[i + nButtonsPerMuxSum] > 500) {
//        buttonCState[i + nButtonsPerMuxSum] = HIGH;
//      }
//      else {
//        buttonCState[i + nButtonsPerMuxSum] = LOW;
//      }
//    }
//    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
//  }

    // read pins from arduino
  for (int i = 0; i < N_POTS_ARDUINO; i++) {
    potCState[i] = analogRead(POT_ARDUINO_PIN[i]);
  }

  //reads all the pots of all the boards and stores in potCState
  int nPotsPerMuxSum = N_POTS_ARDUINO;

  // read pins from mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_POTS_PER_MUX[j]; i++) {
      potCState[i + nPotsPerMuxSum] = mux[j].readChannel(POT_MUX_PIN[j][i]);
    }
    nPotsPerMuxSum += N_POTS_PER_MUX[j];
  }



  //Debug only
  //    for (int i = 0; i < nPots; i++) {
  //      Serial.print(potCState[i]); Serial.print(" ");
  //    }
  //    Serial.println();
  //

  for (int i = 0; i < N_POTS; i++) { // Loops through all the potentiometers

    midiCState[i] = map(potCState[i], 0, 1023, 127, 0); // Maps the reading of the potCState to a value usable in midi

    potVar = abs(potCState[i] - potPState[i]); // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) { // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis(); // Stores the previous time
    }

    timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) { // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) { // If the potentiometer is still moving, send the change control
      if (midiPState[i] != midiCState[i]) {

        // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328
        // use if using with ATmega328 (uno, mega, nano...)
        MIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc value, midi channel

#elif ATMEGA32U4
        //use if using with ATmega32U4 (micro, pro micro, leonardo...)
        controlChange(midiCh, cc + i, midiCState[i]); //  (channel, CC number,  CC value)
        MidiUSB.flush();

#elif TEENSY
        //do usbMIDI.sendControlChange if using with Teensy
        usbMIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc value, midi channel

#else DEBUG
        Serial.print("Pot: ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(midiCState[i]);
        //Serial.print("  ");
#endif

        potPState[i] = potCState[i]; // Stores the current reading of the potentiometer to compare with the next
        midiPState[i] = midiCState[i];
      }
    }
  }
}

//////////////////////////////////////////////
////read encoder values
//void readEncoders() { //read each encoder and stores its value
//
//  for (int i = 0; i < N_ENCODERS; i++) {
//    encoderValue[Bank][i] = encoder[i].read();
//  }
//}
//
//void encoders() { //do whatever needs to be done with the encoder values
//
//  readEncoders();
//
//  for (int i = 0; i < N_ENCODERS; i++) {
//    if (encoderValue[Bank][i] != lastEncoderValue[Bank][i]) {
//
//      clipEncoderValue(i, 0, 127); //check if it's > than 127, or < then 0
//      sendEncoderCC(i);
//      lastEncoderValue[Bank][i] = encoderValue[Bank][i];
//      //printEncoderValue(i);
//    }
//  }
//}
//
//////////////////////////////////////////////
////sends encoder's midi cc
//void sendEncoderCC(byte i)  {
//
//  // use if using with ATmega328 (uno, mega, nano...)
//  //do usbMIDI.sendControlChange if using with Teensy
//  MIDI.sendControlChange(i, encoderValue[Bank][i], Bank);
//
//  //use if using with ATmega32U4 (micro, pro micro, leonardo...)
//  //        controlChange(i, encoderValue[Bank][i], Bank);
//  //        MidiUSB.flush();
//
//}
//
//////////////////////////////////////////////
////check if it's > than x, or < then y
//void clipEncoderValue(int i, int minVal, int maxVal) {
//
//  if (encoderValue[Bank][i] > maxVal - 1) {
//    encoderValue[Bank][i] = maxVal;
//    encoder[i].write(maxVal);
//  }
//  if (encoderValue[Bank][i] < minVal + 1) {
//    encoderValue[Bank][i] = minVal;
//    encoder[i].write(minVal);
//  }
//}
//
//////////////////////////////////////////////
////print encoder values in the serial monitor
//void printEncoderValue(byte i) {
//  Serial.print("Bank: "); Serial.print(Bank); Serial.print("  ");
//  Serial.print("Encoder "); Serial.print(i); Serial.print(": ");
//  Serial.println(encoderValue[Bank][i]);
//}


// use if using with ATmega32U4 (micro, pro micro, leonardo...)
/*
  /////////////////////////////////////////////
  // Arduino (pro)micro midi functions MIDIUSB Library
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
*/


