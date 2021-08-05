#ifdef USING_REMOTE_SCRIPT

// Extracted from the following sources. Adapted by Gustavo Silveira
//
// https://sites.google.com/view/mackiecontroluniversaldiyguide/home
// CC List https://professionalcomposers.com/midi-cc-list/
// Note List https://computermusicresource.com/midikeys.html

// CC - Value CW = 1 or CCW = 65 | Channel = 1
// NOTES - value/On = 127 or value/Off = 0 | Channel = 1
// PITCH BEND - Value/Max = 8176 / Value/Min -8192  | Channel (fader number)



// Vpot
byte vPotCC[8] = {16, 17, 18, 19, 20, 21, 22, 23}; // CC
byte vPotSwitchNote[8] = {44, 45, 46, 47, 48, 49, 50, 51}; // NOTE

// Cannel switches
byte recRdy [8] = {12, 13, 14, 15, 16, 17, 18, 19}; // NOTE
byte solo[8] = {20, 21, 22, 23, 24, 25, 26, 27}; // NOTE
byte mute[8] = {28, 29, 30, 31, 32, 33, 34, 35}; // NOTE
byte select[8] = {36, 37, 38, 39, 40, 41, 42, 43}; // NOTE

// Faders
byte faderTouch[9] = {116, 117, 118, 119, 120, 121, 122, 123, 124}; // NOTE
byte fader[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // PITCH BEND

// General Controls

#endif
