#ifdef USING_VU

////////////////////////////////////////////
// led feedback
void VU(byte channel, byte number, byte value) {

  int value_ = value;

  if (channel == VU_MIDI_CH) { // if we are listening to the right MIDI channel

    // Left VU
    if (number == vuLcc) {

      switch (value_) {
        case 0:
          for (int i = 0; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          break;
        case 1:
          for (int i = 1; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          ShiftPWM.SetOne(VuL[0], ledColor1);
          break;
        case 2:
          for (int i = 2; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          for (int i = 0; i < 2; i++) {
            ShiftPWM.SetOne(VuL[i], ledColor1);
          }
          break;
        case 3:
          for (int i = 3; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          for (int i = 0; i < 3; i++) {
            ShiftPWM.SetOne(VuL[i], ledColor1);
          }
          break;
        case 4:
          for (int i = 4; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          for (int i = 0; i < 4; i++) {
            ShiftPWM.SetOne(VuL[i], ledColor1);
          }
          break;
        case 5:
          for (int i = 5; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          for (int i = 0; i < 5; i++) {
            ShiftPWM.SetOne(VuL[i], ledColor1);
          }
          break;
        case 6:
          for (int i = 6; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          for (int i = 0; i < 5; i++) {
            ShiftPWM.SetOne(VuL[i], ledColor1);
          }
          ShiftPWM.SetOne(VuL[5], ledColor2);
          break;
        case 7:
          for (int i = 6; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuL[i], LOW);
          }
          for (int i = 0; i < 5; i++) {
            ShiftPWM.SetOne(VuL[i], ledColor1);
          }
          ShiftPWM.SetOne(VuL[5], ledColor2);
          ShiftPWM.SetOne(VuL[6], ledColor3);
          break;
      }
    }

    // Right VU
    if (number == vuRcc) {

      switch (value_) {
        case 0:
          for (int i = 0; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          break;
        case 1:
          for (int i = 1; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          ShiftPWM.SetOne(VuR[0], ledColor1);
          break;
        case 2:
          for (int i = 2; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          for (int i = 0; i < 2; i++) {
            ShiftPWM.SetOne(VuR[i], ledColor1);
          }
          break;
        case 3:
          for (int i = 3; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          for (int i = 0; i < 3; i++) {
            ShiftPWM.SetOne(VuR[i], ledColor1);
          }
          break;
        case 4:
          for (int i = 4; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          for (int i = 0; i < 4; i++) {
            ShiftPWM.SetOne(VuR[i], ledColor1);
          }
          break;
        case 5:
          for (int i = 5; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          for (int i = 0; i < 5; i++) {
            ShiftPWM.SetOne(VuR[i], ledColor1);
          }
          break;
        case 6:
          for (int i = 6; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          for (int i = 0; i < 5; i++) {
            ShiftPWM.SetOne(VuR[i], ledColor1);
          }
          ShiftPWM.SetOne(VuR[5], ledColor2);
          break;
        case 7:
          for (int i = 6; i < N_LED_PER_VU; i++) {
            ShiftPWM.SetOne(VuR[i], LOW);
          }
          for (int i = 0; i < 5; i++) {
            ShiftPWM.SetOne(VuR[i], ledColor1);
          }
          ShiftPWM.SetOne(VuR[5], ledColor2);
          ShiftPWM.SetOne(VuR[6], ledColor3);
          break;
      }
    }
  }

}

#endif
