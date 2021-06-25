#include <MIDI.h>

// Simple tutorial on how to receive and send MIDI messages.
// Here, when receiving any message on channel 4, the Arduino 
// will blink a led and play back a note for 1 second.

#define LED 13   		    // LED pin on Arduino Uno

void setup()
{
    pinMode(LED, OUTPUT);
    MIDI.begin(4);          // Launch MIDI and listen to channel 4
}

void loop()
{
    if (MIDI.read())                // If we have received a message
    {
        digitalWrite(LED,HIGH);
        MIDI.sendNoteOn(42,127,1);  // Send a Note (pitch 42, velo 127 on channel 1)
        delay(1000);		        // Wait for a second
        MIDI.sendNoteOff(42,0,1);   // Stop the note
        digitalWrite(LED,LOW);
    }
}
