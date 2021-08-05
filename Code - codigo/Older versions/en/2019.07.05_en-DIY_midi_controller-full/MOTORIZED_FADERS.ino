#ifdef USING_MOTORIZED_FADERS

/////////////////////////////////////////////
// variables you don't need to change
int faderPos[N_M_FADERS] = {0}; // position of the fader
int faderPPos[N_M_FADERS] = {0}; // previous position of the fader
int faderMax[N_M_FADERS] = {0};   //Value read by fader's maximum position (0-1023)
int faderMin[N_M_FADERS] = {0};   //Value read by fader's minimum position (0-1023)


// Midi
byte pFaderInVal[16][N_M_FADERS] = {0};
byte pFaderPBInVal[N_M_FADERS] = {0}; // Pitch bend for Mackie

// Cap Sense
boolean isTouched[N_M_FADERS] = {false}; // Is the fader currently being touched?
boolean pIsTouched[N_M_FADERS] = {false}; // previous Is the fader currently being touched?

long touchLine[N_M_FADERS] = {0};

unsigned long capTimeNow[N_M_FADERS] = {0};

/////////////////////////////////////////////
// Functions

void calibrateFader(byte index_) {

  int reading;

  //Send fader to the top and read max pos_ition
  digitalWrite(motorUpPin[index_], HIGH);
  delay(250);
  digitalWrite(motorUpPin[index_], LOW);

  reading = analogRead(POT_ARDUINO_PIN[index_]);
  responsivePot[index_].update(reading);
  potCState[index_] = responsivePot[index_].getValue();


  faderMax[index_] = potCState[index_] - 10;

  //Send fader to the bottom and read max pos_ition
  digitalWrite(motorDownPin[index_], HIGH);
  delay(250);
  digitalWrite(motorDownPin[index_], LOW);

  reading = analogRead(POT_ARDUINO_PIN[index_]);
  responsivePot[index_].update(reading);
  potCState[index_] = responsivePot[index_].getValue();

  faderMin[index_] = potCState[index_] + 10;

#ifdef DEBUG
  Serial.print("Fader: ");
  Serial.print(index_);
  Serial.print(" | min: ");
  Serial.print(faderMin[index_]);
  Serial.print(" | max: ");
  Serial.println(faderMax[index_]);
  Serial.println();
#endif
}

//Check to see if the fader is being touched
void checkTouch(byte index_, long in_) {

  boolean isTouchTemp;

  if (in_ > capSensitivity) {
    isTouchTemp = true;  //700 is arbitrary and may need to be changed
  } else {
    isTouchTemp = false;
  }
  //depending on the fader cap used (if any).

  if (isTouchTemp ==  true) {
    capTimeNow[index_] = millis();
  }

  if ( (millis() - capTimeNow[index_]) < capTimeout) {
    isTouched[index_] = true;
  } else {
    isTouched[index_] = false;
  }

  //    if (in_ > capSensitivity) {
  //    isTouched[index_] = true;  //700 is arbitrary and may need to be changed
  //  } else {
  //    isTouched[index_] = false;
  //  }

#ifdef DEBUG
  if ( isTouched[index_] != pIsTouched[index_]) {
    Serial.print("Fader: ");
    Serial.print(index_);
    Serial.print(" | touch: ");
    Serial.println(isTouched[index_]);
    pIsTouched[index_] = isTouched[index_];
  }

#endif
}

// changes the position of the motorized fader
void updateFader(byte index_, int pos_) {

  analogWrite(faderSpeedPin[index_], faderSpeed[index_]); // sets the speed of the motor

  int reading = analogRead(POT_ARDUINO_PIN[index_]);
  responsivePot[index_].update(reading);
  potCState[index_] = responsivePot[index_].getValue();


  if (pos_ < potCState[index_] - motorStopPoint && pos_ > faderMin[index_] && !isTouched[index_]) {
    digitalWrite(motorDownPin[index_], HIGH);
  } else {
    digitalWrite(motorDownPin[index_], LOW);
  }

  if (pos_ > potCState[index_] + motorStopPoint && pos_ < faderMax[index_] && !isTouched[index_]) {
    digitalWrite(motorUpPin[index_], HIGH);
  } else {
    digitalWrite(motorUpPin[index_], LOW);
  }

}


#endif // USING_MOTORIZED_FADERS
