/*
/////////////////////////////////////////
Get the Note Number

This function takes in a scaleName string, and an noteIndex int as input. 
It then uses an if-else statement to check which scale the user has inputted and assigns 
the corresponding scale to the selectedScale array. If the scale is not one of 
the 8 mentioned in the prompt, the function returns -1.

The function returns the noteIndex value of the selected scale array, which is the number corresponding to the user input.
*/

int getNoteNumber(String scaleName, int noteIndex) {
  int[] majorScale = { 0, 2, 4, 5, 7, 9, 11 };
  int[] minorScale = { 0, 2, 3, 5, 7, 8, 10 };
  int[] harmonicMinorScale = { 0, 2, 3, 5, 7, 8, 11 };
  int[] melodicMinorScale = { 0, 2, 3, 5, 7, 9, 11 };
  int[] majorPentatonicScale = { 0, 2, 4, 7, 9 };
  int[] minorPentatonicScale = { 0, 3, 5, 7, 10 };
  int[] octatonicScale = { 0, 2, 3, 5, 6, 8, 9, 11 };
  int[] diminishedScale = { 0, 2, 3, 5, 6, 8, 9, 11 };
  int[] selectedScale;

  if (scaleName == "major") {
    selectedScale = majorScale;
  } else if (scaleName == "minor") {
    selectedScale = minorScale;
  } else if (scaleName == "harmonic minor") {
    selectedScale = harmonicMinorScale;
  } else if (scaleName == "melodic minor") {
    selectedScale = melodicMinorScale;
  } else if (scaleName == "major pentatonic") {
    selectedScale = majorPentatonicScale;
  } else if (scaleName == "minor pentatonic") {
    selectedScale = minorPentatonicScale;
  } else if (scaleName == "octatonic") {
    selectedScale = octatonicScale;
  } else if (scaleName == "diminished") {
    selectedScale = diminishedScale;
  } else {
    return -1;  // Invalid scale name
  }
  return selectedScale[noteIndex];
}

/*
/////////////////////////////////////////
MIDI to Frequency

This function takes in a midiNote int as input and converts it to a frequency using the formula:
pow(2, (midiNote - 69) / 12.0) * 440
where pow is the C++ function for exponentiation, and 440 is the reference frequency for A4 (midi note 69). The formula is based on the relationship between MIDI notes and frequencies in which each note is a multiple of the twelfth root of 2 from the reference note.

The function returns the calculated frequency as a float.
*/

float midiNoteToFrequency(int midiNote) {
  return pow(2, (midiNote - 69) / 12.0) * 440;
}

