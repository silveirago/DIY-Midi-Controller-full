#ifdef BLEMIDI

/*
How to send messages

noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
noteOff(uint8_t channel, uint8_t note, uint8_t velocity)
afterTouchPoly(uint8_t channel, uint8_t note, uint8_t pressure)
controlChange(uint8_t channel, uint8_t controller, uint8_t value)
programChange(uint8_t channel, uint8_t program)
afterTouch(uint8_t channel, uint8_t pressure)
pitchBend(uint8_t channel, uint8_t lsb, uint8_t msb) {
pitchBend(uint8_t channel, uint16_t value)
pitchBend(uint8_t channel, float semitones, float range)
*/

void connected();

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Received note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Received note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
    Serial.printf("Received control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

void connected()
{
  Serial.println("Connected");
}

#endif
