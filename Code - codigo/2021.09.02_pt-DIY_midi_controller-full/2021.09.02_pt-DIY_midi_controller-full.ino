/*/*
   Feito por Gustavo Silveira, 2021.

   http://www.musiconerd.com
   http://www.youtube.com/musiconerd
   http://facebook.com/musiconerdmusiconerd
   http://instagram.com/musiconerd/
   http://www.gustavosilveira.net
   gustavosilveira@musiconerd.com

   Se você estiver usando para algo que não seja para uso pessoal, não se esqueça de dar o crédito.

   PS: Basta alterar o valor que tem um comentário como "// *"

*/

///////////////////////////////////////////////////
// Escolhendo sua placa
// Defina a sua placa, escolha:
// "ATMEGA328" se estiver usando ATmega328 - Uno, Mega, Nano ...
// "ATMEGA32U4" se estiver usando com ATmega32U4 - Micro, Pro Micro, Leonardo ...
// "TEENSY" se estiver usando uma placa Teensy
// "DEBUG" se você deseja apenas debugar o código no monitor serial

#define ATMEGA32U4 1//* coloque aqui o uC que você está usando, como nas linhas acima seguido por "1", como "ATMEGA328 1", "DEBUG 1", etc.

/////////////////////////////////////////////
// Você está usando botões?
#define USING_BUTTONS 1 //* comente se não estiver usando botões

/////////////////////////////////////////////
// Você está usando potenciometros?
//#define USING_POTENTIOMETERS 1 //* comente se não estiver usando potenciometros

/////////////////////////////////////////////
// Você esstá usando um multiplexer?
#define USING_MUX 1 //* comente se não estiver usando multiplexers

/////////////////////////////////////////////
// Você está usando encoders?
//#define USING_ENCODER 1 //* comente se não estiver usando encoders

/////////////////////////////////////////////
// // Você está usando um neopixel? (qualquer fita de led endereçável)
//#define USING_NEOPIXEL 1 //* comente se não estiver usando neopixels

/////////////////////////////////////////////
// Você está usando Oled Display I2C?
#define USING_DISPLAY 1 //* comente se não estiver usando um Oled Display I2C

/////////////////////////////////////////////
// Você está usando bancos que podem ser alternados com 2 botões?
#define USING_BANKS_WITH_BUTTONS 1 //* comente se não estiver usando bancos com botões.

#define BANKS_FOR_BUTTONS 1
//#define BANKS_FOR_POTS 1
//#define BANKS_FOR_ENCODERS 1

/////////////////////////////////////////////
// Você está usando um bit shifter 74HC595?
// Abaixo, use "USING_VU" ou use "USING_LED_FEEDBACK"
// Use VU para monitorar os níveis de áudio ou feedback de LED para obter feedback de nota

//#define USING_74HC595 1 //* comente se não estiver usando o 74HC595 Bit Shifter

// Você está usando um VU (VU de led)?
//#define USING_VU 1//* comente se não estiver usando um VU

// Você está usando feedback de LED (notas)?
//#define USING_LED_FEEDBACK 1//* comente se não estiver usando VU

// Você está usando faders de alta resolução?
//#define USING_HIGH_RES_FADERS 1 //* comente se não estiver usando faders de alta resolução (qualquer fader pode ser de alta resolução)

// Você está usando Faders motorizados?
//#define USING_MOTORIZED_FADERS 1 //* comente se não estiver usando um fader motorizado

// Are you using the Mackie Protocol?
//#define USING_MACKIE 1

// Você está usando dois botões para alterar a oitava?
#define USING_OCTAVE 1

///////////////////////////////////////////// /////////////////////////////////////////////

/////////////////////////////////////////////
// BIBLIOTECAS
// -- Definem as bibliotecas serem usadas -- //

//////////////////////////////////////
// If using Fast Led
#ifdef USING_NEOPIXEL

#include "FastLED.h" // by Daniel Garcia - http://fastled.io

FASTLED_USING_NAMESPACE

#define DATA_PIN    9
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    4

CRGB leds[NUM_LEDS];
byte ledIndex[NUM_LEDS] = {0, 1, 2, 3};

#define BRIGHTNESS          96 // 0-255
#define FRAMES_PER_SECOND  120

#endif
//////////////////////////////////////

// if using with ATmega328 - Uno, Mega, Nano...
#ifdef ATMEGA328
#include <MIDI.h> // https://github.com/FortySevenEffects/arduino_midi_library?fbclid=IwAR0zHNemdtNvONtysP5HZY_YrVbTSlir0zDhPQPq8EPfnliFZQZ0jjRC81g
//MIDI_CREATE_DEFAULT_INSTANCE();

// if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
#elif ATMEGA32U4
#include "MIDIUSB.h"

#endif

//////////////////////////////////////
// se usando o 74HC595 bit shifter
#ifdef USING_74HC595

// Você mesmo pode escolher o pino latch.
const int ShiftPWM_latchPin = 18;

// ** retire o comentário desta parte para NÃO usar a porta SPI e altere os números dos pinos. Este é 2,5x mais lento **
#define SHIFTPWM_NOSPI
const int ShiftPWM_dataPin = 9;
const int ShiftPWM_clockPin = 19;


// Se o seu LED acender se o pino estiver LOW, defina como verdadeiro, caso contrário, defina como falso.
const bool ShiftPWM_invertOutputs = false;

// Você pode habilitar a opção abaixo para deslocar a fase PWM de cada registro de deslocamento em 8 em comparação com o anterior.
// Isso aumentará um pouco a carga de interrupção, mas impedirá que todos os sinais PWM se tornem altos ao mesmo tempo.
// Isso será um pouco mais fácil em sua fonte de alimentação, porque os picos atuais são distribuídos.
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>   // inclua ShiftPWM.h após definir os pinos!
// Download: https://github.com/elcojacobs/ShiftPWM

#endif // USING_74HC595

//////////////////////
// Adicione esta lib se estiver usando um multiplexador cd4067
#ifdef USING_MUX
#include <Multiplexer4067.h> // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067
#endif

//////////////////////
// Threads
#include <Thread.h> // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h> // Mesmo de cima

//////////////////////
// Encoder
#ifdef USING_ENCODER
// No gerenciador de downloads, baixe o Encoder lib de Paul Stoffregen (vem com o Teensy)
#include <Encoder.h>  // faz todo o trabalho para você na leitura do encoder
#endif

//////////////////////
// Oled Display I2C
#ifdef USING_DISPLAY
//#include <Adafruit_GFX.h>  // Incluir biblioteca gráfica principal para a tela
#include <Adafruit_SSD1306.h>  // Inclui a biblioteca Adafruit_SSD1306 para controlar o display
//#include <Fonts/FreeMonoBold12pt7b.h>  // Adiciona uma fonte customizada
//#include <Fonts/FreeMono9pt7b.h>  // Adiciona uma fonte customizada
Adafruit_SSD1306 display(128, 64);  // Cria o display - tamanho da tela em pixels
#endif


///////////////////////////////////////////
// MULTIPLEXERS
#ifdef USING_MUX

#define N_MUX 1 //* número de multiplexers
//* Defina s0, s1, s2, s3, e x pinos
#define s0 14
#define s1 15
#define s2 18
#define s3 19

#define x1 A3 // pino analógico do primeiro mux (sig)

// adicione mais #define e o número x se precisar

// Inicializa o multiplexer
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1), // O pino SIG onde o multiplexador está conectado
  // ...
};

#endif

#ifdef USING_POTENTIOMETERS
// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h> // https://github.com/dxinteractive/ResponsiveAnalogRead
#endif

#ifdef USING_MOTORIZED_FADERS
#include <CapacitiveSensor.h> // Ads touch capabilities for the motorized fader
#endif

#ifdef USING_MACKIE
#include "MACKIE.h"
#endif


/////////////////////////////////////////////
// BOTÕES
#ifdef USING_BUTTONS

const byte N_BUTTONS = 2; //*  número total de botões. Número de botões no Arduino + número de botões no multiplexer 1 + número de botões no multiplexer 2 ...

const byte N_BUTTONS_ARDUINO = 0; //* número de botões conectados diretamente ao Arduino
const byte BUTTON_ARDUINO_PIN[N_BUTTONS] = {}; //* pinos de cada botão conectados diretamente ao Arduino

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX // Preencha se você estiver usando mux, caso contrário, deixe-o
const byte N_BUTTONS_PER_MUX[N_MUX] = {2}; //* número total de botões em cada mux (em ordem)
const byte BUTTON_MUX_PIN[N_MUX][16] = { //* pino de cada botão de cada mux em ordem
{3, 2}, //* pinos do primeiro mux
// ...
};

int buttonMuxThreshold = 300;

#endif //USING_MUX

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Que tipo de mensagem você deseja enviar?
// Note Number - Control Change - Toggle

byte NN = 0; // Note number
byte CC = 1; // Control change
byte T = 2;  // Toggle

//* Coloque aqui o tipo de mensagem que deseja enviar, na mesma ordem em que declarou os pinos do botão
// "NN" para Note Number | "CC" para Control Change | "T" para Note Number mas em modo toggle
byte MESSAGE_TYPE[N_BUTTONS] = {NN, NN};

//* Coloque aqui o número da mensagem que deseja enviar, na ordem certa, não importa se é um número de nota ou CC.
byte MESSAGE_VAL[N_BUTTONS] = {36, 38};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_BANKS_WITH_BUTTONS

//#define USING_MUX_BANK_BUTTON_PIN 1; // Defina se você está usando os botões de banco nos pinos do Mux. Tem que ser o primeiro mux.
const byte BANK_BUTTON_PIN[2] = {16, 10}; //* o primeiro diminuirá o canal MIDI e o segundo aumentará

#endif //USING_BANKS_WITH_BUTTONS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_OCTAVE

#define USING_MUX_OCTAVE_PIN 1; // Defina se você está usando os botões de oitava no pino Mux. Tem que ser o primeiro mux.
const byte OCTAVE_BUTTON_PIN[2] = {5, 4}; //* o primeiro diminuirá o canal MIDI e o segundo aumentará

#endif // USING_OCTAVE

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//#define pin13 1 // descomente se estiver usando o pino 13 (pino com led) ou comente a linha se não estiver
byte pin13index = 12; //* coloque o index do pino 13 do array buttonPin [] se você estiver usando, se não, comente

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// isso acontecerá se você estiver usando neo pixel
// este botão abrirá o menu para que você possa alterar o canal MIDI
// pressionando um botão. Cada botão será um canal diferente

#ifdef USING_NEOPIXEL
const byte CHANNEL_BUTTON_PIN = 8;
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// debounce
unsigned long debounceDelay = 5;    //* tempo de debounce; aumentar se a saída piscar

#endif // USING_BUTTONS

/////////////////////////////////////////////
// POTENCIOMETROS

#ifdef USING_POTENTIOMETERS

const byte N_POTS = 1; //* número total de pots (slide e rotativo). Número de pots no Arduino + número de pots no multiplexer 1 + número de pots no multiplexer 2 ...

const byte N_POTS_ARDUINO = 1; //* número de pots conectados diretamente ao Arduino
const byte POT_ARDUINO_PIN[N_POTS_ARDUINO] = {A0}; //* pinos de cada potenciômetro conectado diretamente ao Arduino

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX
const byte N_POTS_PER_MUX[N_MUX] = {2}; //* número de pots em cada multiplexador (em ordem)
const byte POT_MUX_PIN[N_MUX][16] = { //* pinos de cada pot de cada mux na ordem que você quer que eles sejam
{0, 1}, //* pinos do primeiro mux
// ...
};
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//#define USING_CUSTOM_CC_N 1 //* comente se não estiver usando NÚMEROS CUSTOM CC, descomente se estiver usando.
#ifdef USING_CUSTOM_CC_N
byte POT_CC_N[N_POTS] = {20, 21}; // Adicione o CC NUMBER de cada pote que você quiser
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const int TIMEOUT = 300; //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const byte varThreshold = 2; //* Threshold for the potentiometer signal variation

// put here the min and max reading in the potCState
// in the potMin put a little bit more and in the potMax put a little bit less
int potMin = 95;
int potMax = 970;

#endif // USING_POTENTIOMETERS

/////////////////////////////////////////////
// ENCODERS
#ifdef USING_ENCODER

//#define TRAKTOR 1 // descomente se estiver usando com traktor, comente se não

const byte N_ENCODERS = 1; //* número de encoders

const byte N_ENCODER_PINS = N_ENCODERS * 2; // número de pinos usados pelos codificadores
const byte N_ENCODER_MIDI_CHANNELS = 16; // número de ENCODER_MIDI_CHs

byte ENCODER_CC_N[N_ENCODERS] = {11}; //* Adicione o CC NUMBER de cada encoder que você deseja

Encoder encoder[N_ENCODERS] = {{2, 3}}; //* os dois pinos de cada encoder - Use pinos com interrupts!

byte encoderMinVal = 0; //* valor mínimo do encoder
byte encoderMaxVal = 127; //* vamor máximo do encoder

byte preset[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = { //* armazena presets para o seu encoder
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

#endif //USING_ENCODER


/////////////////////////////////////////////
// MIDI CHANNEL
byte POT_MIDI_CH = 0; //* Canal MIDI a ser usado
byte BUTTON_MIDI_CH = 0;
byte ENCODER_MIDI_CH = 0;

byte NOTE = 36; //* NOTA mais baixa a ser usada - se não estiver usando NÚMERO DE NOTA personalizado
byte CC_NUMBER = 1; //* Menor CC MIDI a ser usado - se não estiver usando NÚMERO CC personalizado

/////////////////////////////////////////////
// NEOPIXEL | MIDI CHANNEL MENU
#ifdef USING_NEOPIXEL

boolean channelMenuOn = false;
byte midiChMenuColor = 200; //* menu color HUE - 0-255
byte midiChOnColor = midiChMenuColor + 60; // canal na cor do menu HUE
byte noteOffHue = 135; //* HUE quando as notas não são tocadas - 135 (azul)
byte noteOnHue = 240; //* HUE of the notes when they are played - 240 (magenta)

#endif // USING_NEOPIXEL

/////////////////////////////////////////////
// THREADS
// Este libs cria uma thread "falsa". Isso significa que você pode fazer algo acontecer a cada x milissegundos
// Podemos usar isso para ler algo em um intervalo, em vez de ler a cada loop
// Neste caso vamos ler os potenciômetros em um thread, tornando a leitura dos botões mais rápida

ThreadController cpu; // thread master, onde os outros threads serão adicionados
Thread threadPotentiometers; // thread para controlar os pots
Thread threadChannelMenu; // thread to control o menu

#ifdef USING_BANKS_WITH_BUTTONS
Thread threadBanksWithButtons;
#endif

#ifdef USING_OCTAVE
Thread threadChangeOctave;
#endif


/////////////////////////////////////////////
// DISPLAY
byte display_pos_x = 27; // pos x
byte display_pos_y = 7; // pos y
byte display_text_size = 7; // text font size


/////////////////////////////////////////////
// 75HC595
#ifdef USING_74HC595

unsigned char maxBrightness = 255; //*
unsigned int numRegisters = 2; //*
const byte numOutputs = 14; //*

unsigned char pwmFrequency = 75;
//unsigned int numOutputs = numRegisters * 8;
unsigned int numRGBLeds = numRegisters * 8 / 3;
unsigned int fadingMode = 0; //start with all LED's off.

byte brightness = 0;

byte ledColor1 = 255; // 0-255
byte ledColor2 = 100;
byte ledColor3 = 180;
//byte ledColor4 = 10;


/////////////////////////////////////////////
// VU
#ifdef USING_VU

const byte N_LED_PER_VU = 7;
byte VuL[N_LED_PER_VU] = {0, 1, 2, 3, 4, 5, 6}; // VU esquerda pins
byte VuR[N_LED_PER_VU] = {7, 8, 9, 10, 11, 12, 13}; // VU direita pins

byte VU_MIDI_CH = 10; // channel para ouvir o VU

byte vuLcc = 12; // esquerda CC
byte vuRcc = 13; // direta CC

#endif //USING_VU

#ifdef USING_LED_FEEDBACK

byte ledPin[numOutputs] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

#endif //USING_LED_FEEDBACK
#endif // USING_74HC595


/////////////////////////////////////////////
// MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS

//Arduino Pin Assignments
const byte N_M_FADERS = 1;
const byte motorDownPin[N_M_FADERS] = {4};   //Controle ponte-H para fazer o motor descer
const byte motorUpPin[N_M_FADERS] = {5};   // Controle ponte-H para fazer o motor subir
const byte faderSpeedPin[N_M_FADERS] = {false}; // pino de velocidade do motor (PWM)

byte faderSpeed[N_M_FADERS] = {240}; // 0-255
byte faderSpeedMin = 150; // 0-255 - 140?
byte faderSpeedMax = 255; // 0-255 -  250?

const byte motorStopPoint = 18; // motor will stop X values before it reaches its goal. Increase to avoid jittery (it will be less precise).

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// variables you don't need to change
int faderPos[N_M_FADERS] = {0}; // posição do fader
int faderPPos[N_M_FADERS] = {0}; // posição anterior do fader
int faderMax[N_M_FADERS] = {0}; // Valor lido pela posição máxima do fader (0-1023)
int faderMin[N_M_FADERS] = {0}; // Valor lido pela posição mínima do fader (0-1023)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Midi
byte pFaderInVal[16][N_M_FADERS] = {0};
byte pFaderPBInVal[N_M_FADERS] = {0}; // Pitch bend para Mackie

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Cap Sense
boolean isTouched[N_M_FADERS] = {false}; // O fader está sendo tocado?
boolean pIsTouched[N_M_FADERS] = {false}; // anterior O fader está sendo tocado?

long touchLine[N_M_FADERS] = {0};

unsigned long capTimeNow[N_M_FADERS] = {0};

// Sensor capacitivo - Touch Pin
// resistor de 10M entre os pinos 7 e 8, pino 2 é o pino do sensor, adicione um fio e / ou folha se desejar
CapacitiveSensor capSense[N_M_FADERS] = {
  CapacitiveSensor(7, 8)
}; // capSense = CapacitiveSensor(7, 8)

const int touchSendPin[N_M_FADERS] = {7}; // Enviar pino para circuito de detecção de capacitância (Digital 7)
const int touchReceivePin[N_M_FADERS] = {8}; // Receber pino para circuito de detecção de capacitância (Digital 8)

byte capSenseSpeed = 15; // Número de amostras
int capSensitivity = 150; // toque threshold

int capTimeout = 500;

#endif // USING_MOTORIZED_FADERS
