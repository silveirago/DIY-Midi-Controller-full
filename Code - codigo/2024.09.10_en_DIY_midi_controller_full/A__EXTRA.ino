float customLerp(float x1, float x2, float ratio) {
  return x1 + (x2 - x1) * ratio;
}

/////////////////////////////////////////////
// THREADS
// This libs create a "fake" thread. This means you can make something happen every x milisseconds
// We can use that to read something in an interval, instead of reading it every single loop
// In this case we'll read the potentiometers in a thread, making the reading of the buttons faster
ThreadController cpu;         //thread master, where the other threads will be added
Thread threadPotentiometers;  // thread to control the pots
Thread threadChannelMenu;     // thread to control the pots

#ifdef USING_BANKS_WITH_BUTTONS
Thread threadBanksWithButtons;
#endif

#ifdef USING_OCTAVE
Thread threadChangeOctave;
#endif

////////////////////////////////////////////
// checks if it's greater than maximum value or less than then the minimum value
int clipValue(int in, int minVal, int maxVal) {

  int out;

  minVal++;

  if (in > maxVal) {
    out = maxVal;
  }
  else if (in < minVal) {
    out = minVal -1;
  }
  else {
    out = in;
  }

  return out;
}

