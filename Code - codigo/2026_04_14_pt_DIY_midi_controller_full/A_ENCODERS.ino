/////////////////////////////////////////////
// ENCODERS

#ifdef USING_ENCODER  // only compiled if encoders are defined in the setup

#ifndef DEBUG
#define DEBUG 0
#endif

// -------------------------------
// Variables you don't need to change
// -------------------------------
int lastEncoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]      = { 0 };
int lastEncoderMidiValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]  = { 0 };
int encoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]          = { 0 };
int encoderMidiValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]      = { 0 };
int encoderMackieValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]    = { 0 };

byte lastEncoderChannel = 0;

unsigned long encPTime[N_ENCODERS] = { 0 };
unsigned long encTimer[N_ENCODERS] = { 0 };
boolean encMoving[N_ENCODERS]      = { false };
boolean encMoved[N_ENCODERS]       = { false };
byte encTIMEOUT = 50;
byte encoder_n;
float encTempVal = 0;

#ifdef USING_HIGH_RES_ENC
unsigned int highResenc[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { 0 };
byte encMSB[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]             = { 0 };
byte encLSB[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]             = { 0 };
byte pencLSB[N_ENCODER_MIDI_CHANNELS][N_ENCODERS]            = { 0 };
#endif

// -------------------------------
// Tunables (feel)
// -------------------------------
// You asked for 4 edges per detent:
const uint8_t ENC_STEPS_PER_DETENT   = 4;    // 1, 2 or 4 depending on your encoder hardware
const uint8_t ENC_IDLE_HYSTERESIS    = 1;    // ignore ±1 twitch when idle in absolute path
const uint8_t ENC_MAX_TICKS_PER_LOOP = 16;   // cap so one encoder can't starve others

// Per-encoder runtime state
long encPrevRaw[N_ENCODERS] = { 0 };         // visual stability in absolute path

// Accumulator logic for relative modes (Traktor/Mackie)
long  encAccum[N_ENCODERS]      = { 0 };     // our own edge accumulator
int8_t encAccumSign[N_ENCODERS]  = { 0 };     // last nonzero sign of encAccum

static inline int8_t sgn(long x) { return (x > 0) - (x < 0); }

// -------------------------------
// Main function
// -------------------------------
void encoders() {

  // 1) Read and pre-process raw values for ABSOLUTE/HIGH-RES paths
  for (int i = 0; i < N_ENCODERS; i++) {
    long raw = encoder[i].read();  // cumulative quadrature count

    // small hysteresis to avoid flicker when resting between edges
    if (abs(raw - encPrevRaw[i]) <= ENC_IDLE_HYSTERESIS) {
      raw = encPrevRaw[i];
    }
    encPrevRaw[i] = raw;

    encoderValue[ENCODER_MIDI_CH][i] = (int)raw;

#if !defined(TRAKTOR) && !defined(USING_MACKIE)
    // Absolute mode: clamp & scale
    clipEncoderValue(i, encoderMinVal, encoderMaxVal * encSensitivity);
#endif

#ifdef USING_HIGH_RES_ENC
    highResenc[ENCODER_MIDI_CH][i] = map(
      encoderValue[ENCODER_MIDI_CH][i],
      0,  encoderMaxVal * encSensitivity,
      0,  encoderMaxVal
    );
    encMSB[ENCODER_MIDI_CH][i] = highResenc[ENCODER_MIDI_CH][i] / 128;
    encLSB[ENCODER_MIDI_CH][i] = highResenc[ENCODER_MIDI_CH][i] % 128;
#endif

    encoderMidiValue[ENCODER_MIDI_CH][i] = map(
      encoderValue[ENCODER_MIDI_CH][i],
      encoderMinVal,               encoderMaxVal * encSensitivity,
      0,                            encoderMaxVal
    );
  }

  // 2) Emit MIDI
  for (int i = 0; i < N_ENCODERS; i++) {

    if (!encMoving[i]) {

#ifdef USING_HIGH_RES_ENC
      bool changedAbs = (encLSB[ENCODER_MIDI_CH][i] != pencLSB[ENCODER_MIDI_CH][i]);
#else
      bool changedAbs = (encoderMidiValue[ENCODER_MIDI_CH][i] != lastEncoderMidiValue[ENCODER_MIDI_CH][i]);
#endif
      bool sentSomething = false;

      // ===============================================================
      // RELATIVE MODES — Traktor and Mackie
      // Edge-accurate draining with reversal-safe accumulator
      // ===============================================================

#if defined(TRAKTOR) || defined(USING_MACKIE)
      {
        // Pull NEW edges since last loop and reset the library's counter.
        long deltaEdges = encoder[i].read();
        if (deltaEdges != 0) {
          encoder[i].write(0);
        }

        // If new edges are opposite to our accumulator direction, reset accumulator
        if (deltaEdges != 0) {
          int8_t dsgn = sgn(deltaEdges);
          if (encAccumSign[i] != 0 && dsgn != encAccumSign[i]) {
            // Start fresh in the new direction so the very first click registers
            encAccum[i]     = deltaEdges;
            encAccumSign[i] = dsgn;
          } else {
            encAccum[i]    += deltaEdges;
            if (encAccum[i] != 0) encAccumSign[i] = sgn(encAccum[i]);
          }
        }

        // Drain whole detents from our accumulator
        int stepsToSend = (int)(abs(encAccum[i]) / (int)ENC_STEPS_PER_DETENT);
        if (stepsToSend > 0) {
          if (stepsToSend > ENC_MAX_TICKS_PER_LOOP) stepsToSend = ENC_MAX_TICKS_PER_LOOP;

          bool cw = (encAccum[i] > 0);

  #ifdef TRAKTOR
          byte v = cw ? 1 : 127;                        // Traktor: 01 (CW) / 7F (CCW)
  #endif
  #ifdef USING_MACKIE
          byte vm = cw ? (0 + encoderSens) : (64 + encoderSens); // Mackie style (you can vary by speed)
  #endif

          for (int k = 0; k < stepsToSend; k++) {
  #ifdef TRAKTOR
    #ifdef ATMEGA328
            MIDI.sendControlChange(ENCODER_CC_N[i], v, ENCODER_MIDI_CH);
    #elif defined(ATMEGA32U4)
      #ifdef MIDI_DIN
            controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], v);  MidiUSB.flush();
            midi2.sendControlChange(ENCODER_CC_N[i], v, ENCODER_MIDI_CH + 1);
      #else
            controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], v);  MidiUSB.flush();
      #endif
    #elif defined(TEENSY)
            usbMIDI.sendControlChange(ENCODER_CC_N[i], v, ENCODER_MIDI_CH);
    #elif defined(BLEMIDI)
            BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], v);
    #endif
  #endif

  #ifdef USING_MACKIE
    #ifdef ATMEGA328
            MIDI.sendControlChange(ENCODER_CC_N[i], vm, ENCODER_MIDI_CH);
    #elif defined(ATMEGA32U4)
      #ifdef MIDI_DIN
            controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], vm);  MidiUSB.flush();
            midi2.sendControlChange(ENCODER_CC_N[i], vm, ENCODER_MIDI_CH + 1);
      #else
            controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], vm);  MidiUSB.flush();
      #endif
    #elif defined(TEENSY)
            usbMIDI.sendControlChange(ENCODER_CC_N[i], vm, ENCODER_MIDI_CH);
    #elif defined(BLEMIDI)
            BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], vm);
    #endif
  #endif

  #if DEBUG
            // ---- DEBUG print for relative tick ----
            Serial.print("Encoder: "); Serial.print(i);
            Serial.print(" | ch: ");    Serial.print(ENCODER_MIDI_CH);
            Serial.print(" | cc: ");    Serial.print(ENCODER_CC_N[i]);
    #ifdef TRAKTOR
            Serial.print(" | rel: ");   Serial.print(cw ? "+1" : "-1");
            Serial.print(" | v: ");     Serial.println(v);
    #elif defined(USING_MACKIE)
            Serial.print(" | rel: ");   Serial.print(cw ? "+det" : "-det");
            Serial.print(" | vm: ");    Serial.println(vm);
    #else
            Serial.println(" | rel");
    #endif
  #endif
          }

          // Remove exactly the detents we sent, keep the remainder for smoothness
          long consumed = (long)stepsToSend * (long)ENC_STEPS_PER_DETENT * (cw ? +1L : -1L);
          encAccum[i] -= consumed;
          // Update sign for next loop
          encAccumSign[i] = (encAccum[i] == 0) ? 0 : sgn(encAccum[i]);

          sentSomething = true;
        }
      }
#endif // relative modes

      // ===============================================================
      // ABSOLUTE / HIGH-RES PATHS (unchanged behavior)
      // ===============================================================
      if (!sentSomething && changedAbs) {

#ifdef ATMEGA328

  #ifdef USING_HIGH_RES_ENC
        MIDI.sendControlChange(ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
        MIDI.sendControlChange(ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
  #else
        MIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
  #endif

#elif defined(ATMEGA32U4)

  #ifdef MIDI_DIN
    #ifdef USING_MACKIE
        // (relative already sent above if USING_MACKIE; this is a fallback absolute)
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
        midi2.sendControlChange(ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);
    #elif defined(USING_HIGH_RES_ENC)
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
        midi2.sendControlChange(ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);
        midi2.sendControlChange(ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);
    #else
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
        midi2.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH + 1);
    #endif
  #else // not using MIDI_DIN
    #ifdef USING_MACKIE
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
    #elif defined(USING_HIGH_RES_ENC)
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
    #else
        controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);  MidiUSB.flush();
    #endif
  #endif

#elif defined(TEENSY)

  #ifdef USING_MACKIE
        usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
  #elif defined(USING_HIGH_RES_ENC)
        usbMIDI.sendControlChange(ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
        usbMIDI.sendControlChange(ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
  #else
        usbMIDI.sendControlChange(ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i], ENCODER_MIDI_CH);
  #endif

#elif defined(BLEMIDI)

  #ifdef USING_MACKIE
        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMackieValue[ENCODER_MIDI_CH][i]);
  #elif defined(USING_HIGH_RES_ENC)
        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encMSB[ENCODER_MIDI_CH][i]);
        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i] + 32, encLSB[ENCODER_MIDI_CH][i]);
  #else
        BLEMidiServer.controlChange(ENCODER_MIDI_CH, ENCODER_CC_N[i], encoderMidiValue[ENCODER_MIDI_CH][i]);
  #endif

#endif // board paths

#if DEBUG
        // ---- DEBUG print for absolute/HR send ----
        Serial.print("Encoder: "); Serial.print(i);
        Serial.print(" | ch: ");   Serial.print(ENCODER_MIDI_CH);
        Serial.print(" | cc: ");   Serial.print(ENCODER_CC_N[i]);
  #ifdef USING_HIGH_RES_ENC
        Serial.print(" | HR: ");   Serial.print(highResenc[ENCODER_MIDI_CH][i]);
        Serial.print(" | MSB: ");  Serial.print(encMSB[ENCODER_MIDI_CH][i]);
        Serial.print(" | LSB: ");  Serial.println(encLSB[ENCODER_MIDI_CH][i]);
  #else
        Serial.print(" | value: "); Serial.println(encoderMidiValue[ENCODER_MIDI_CH][i]);
  #endif
#endif

        lastEncoderMidiValue[ENCODER_MIDI_CH][i] = encoderMidiValue[ENCODER_MIDI_CH][i];
        lastEncoderValue[ENCODER_MIDI_CH][i]     = encoderValue[ENCODER_MIDI_CH][i];
#ifdef USING_HIGH_RES_ENC
        pencLSB[ENCODER_MIDI_CH][i]              = encLSB[ENCODER_MIDI_CH][i];
#endif

        sentSomething = true;
      } // absolute/high-res
    }   // if !encMoving
  }     // for i
}

// -------------------------------
// Helpers
// -------------------------------
void clipEncoderValue(int i, int minVal, int maxVal) {
  if (encoderValue[ENCODER_MIDI_CH][i] > maxVal - 1) {
    encoderValue[ENCODER_MIDI_CH][i] = maxVal;
    encoder[i].write(maxVal);
  }
  if (encoderValue[ENCODER_MIDI_CH][i] < minVal + 1) {
    encoderValue[ENCODER_MIDI_CH][i] = minVal;
    encoder[i].write(minVal);
  }
}

/* Prevent feedback while DAW is updating the encoder */
void isEncoderMoving() {
  for (int i = 0; i < N_ENCODERS; i++) {
    if (encMoved[i] == true) {
      encPTime[i] = millis();
    }
    encTimer[i] = millis() - encPTime[i];
    if (encTimer[i] < encTIMEOUT) {
      encMoving[i] = true;
      encMoved[i]  = false;
    } else {
      if (encMoving[i] == true) {
        encMoving[i] = false;
      }
    }
  }
}

#endif  // USING_ENCODER
