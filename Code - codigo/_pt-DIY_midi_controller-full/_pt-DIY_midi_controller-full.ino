/*
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

#define DEBUG 1//* coloque aqui o uC que você está usando, como nas linhas acima seguido por "1", como "ATMEGA328 1", "DEBUG 1", etc.

/////////////////////////////////////////////
// Você está usando botões?
#define USING_BUTTONS 1 //* comente se não estiver usando botões

/////////////////////////////////////////////
// Você está usando potenciometros?
#define USING_POTENTIOMETERS 1 //* comente se não estiver usando potenciometros

/////////////////////////////////////////////
// Você esstá usando um multiplexer?
//#define USING_MUX 1 //* comente se não estiver usando multiplexers

/////////////////////////////////////////////
// Você esstá usando encoders?
//#define USING_ENCODER 1 //* comente se não estiver usando encoders

/////////////////////////////////////////////
// // Você esstá usando um neopixel? (qualquer fita de led endereçável)
//#define USING_NEOPIXEL 1 //* comente se não estiver usando neopixels

/////////////////////////////////////////////
// Você esstá usando Oled Display I2C?
//#define USING_DISPLAY 1 //* comente se não estiver usando um Oled Display I2C

/////////////////////////////////////////////
// Você está usando bancos que podem ser alternados com 2 botões?
//#define USING_BANKS_WITH_BUTTONS 1 //* comente se não estiver usando bancos com botões.

//#define BANKS_FOR_BUTTONS 1
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
#include <MIDI.h>
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
#define s0 7
#define s1 6
#define s2 5
#define s3 4

#define x1 A2 // pino analógico do primeiro mux (sig)

// adicione mais #define e o número x se precisar

// Inicializa o multiplexer
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1), // O pino SIG onde o multiplexador está conectado
  // ...
};

#endif



/////////////////////////////////////////////
// BOTÕES
#ifdef USING_BUTTONS

const byte N_BUTTONS = 0 + 4; //*  número total de botões. Número de botões no Arduino + número de botões no multiplexer 1 + número de botões no multiplexer 2 ...

const byte N_BUTTONS_ARDUINO = 0; //* número de botões conectados diretamente ao Arduino
const byte BUTTON_ARDUINO_PIN[N_BUTTONS] = {}; //* pinos de cada botão conectados diretamente ao Arduino

#ifdef USING_MUX // Preencha se você estiver usando mux, caso contrário, deixe-o
const byte N_BUTTONS_PER_MUX[N_MUX] = {4}; //* número total de botões em cada mux (em ordem)
const byte BUTTON_MUX_PIN[N_MUX][16] = { //* pino de cada botão de cada mux em ordem
{2, 3, 4, 5}, //* pinos do primeiro mux
// ...
};
#endif

#ifdef USING_BANKS_WITH_BUTTONS

const byte BANK_BUTTON_PIN[2] = {8, 21}; //* o primeiro diminuirá o canal MIDI e o segundo aumentará
int buttonBankCState[2] = {0};
int buttonBankPState[2] = {0};
#endif

// vai acontecer se você estiver usando neo pixel
// este botão abrirá o menu para que você possa alterar o canal MIDI
// pressionando um botão. Cada botão será um canal diferente
#ifdef USING_NEOPIXEL
const byte CHANNEL_BUTTON_PIN = 8;
#endif

//#define USING_CUSTOM_NN 1 // * comentar se não estiver usando NÚMEROS DE NOTA PERSONALIZADOS (escalas), descomente se estiver usando.
#ifdef USING_CUSTOM_NN
byte BUTTON_NN[N_BUTTONS] = {36, 38};

// * Adicione o NÚMERO DE NOTA de cada botão / interruptor que você deseja
#endif

//#define USING_BUTTON_CC_N 1 //* comente se não estiver usando o BUTTON CC, descomente se estiver usando.
#ifdef USING_BUTTON_CC_N // se estiver usando o botão com CC
byte BUTTON_CC_N[N_BUTTONS] = {10, 11}; //* Adicione o NÚMERO CC de cada botão/switch que desejar
#endif

//#define USING_TOGGLE 1 //* comente se não estiver usando o modo TOGGLE, descomente se estiver usando.
// Com o modo toggle ativado, quando você pressiona o botão uma vez ele envia uma note on, quando você pressiona novamente ele envia uma note off

int buttonMuxThreshold = 300;

int buttonCState[N_BUTTONS] = {0};        // armazena o valor atual do botão
int buttonPState[N_BUTTONS] = {0};        // armazena o valor anterior do botão

//#define pin13 1 // descomente se estiver usando o pino 13 (pino com led) ou comente a linha se não estiver
//byte pin13index = 12; //* coloque o index do pino 13 do array buttonPin[] se você estiver usando, se não, comente

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // a última vez que o pino de saída foi alternado
unsigned long debounceDelay = 5;    //* o tempo de debounce; aumentar se houver "bounce"

// velocity
byte velocity[N_BUTTONS] = {127};

#endif // USING_BUTTONS

/////////////////////////////////////////////
// POTENCIOMETROS

#ifdef USING_POTENTIOMETERS

const byte N_POTS = 2; //* número total de pots (slide e rotativo). Número de pots no Arduino + número de pots no multiplexer 1 + número de pots no multiplexer 2 ...

const byte N_POTS_ARDUINO = 2; //* número de pots conectados diretamente ao Arduino
const byte POT_ARDUINO_PIN[N_POTS_ARDUINO] = {A0, A1}; //* pinos de cada potenciômetro conectado diretamente ao Arduino

//#define USING_CUSTOM_CC_N 1 //* comente se não estiver usando NÚMEROS CUSTOM CC, descomente se estiver usando.
#ifdef USING_CUSTOM_CC_N
byte POT_CC_N[N_POTS] = {20, 21}; // Adicione o CC NUMBER de cada pote que você quiser
#endif

#ifdef USING_MUX
const byte N_POTS_PER_MUX[N_MUX] = {2}; //* número de pots em cada multiplexador (em ordem)
const byte POT_MUX_PIN[N_MUX][16] = { //* pinos de cada pot de cada mux na ordem que você quer que eles sejam
{0, 1}, //* pinos do primeiro mux
// ...
};
#endif

int potCState[N_POTS] = {}; // Estado atual do pot
int potPState[N_POTS] = {}; // Estado previo do pot
int potVar = 0; // Diferença entre o estado atual e anterior do pot

int potMidiCState[N_POTS] = {}; // Estado atual do valor midi
int potMidiPState[N_POTS] = {}; // Estado anterior do valor midi

const int TIMEOUT = 300; //* Quantidade de tempo que o potenciômetro será lido após exceder o varThreshold
const byte varThreshold = 20; //* Limiar para a variação do sinal do potenciômetro
boolean potMoving = true; // Se o potenciômetro está se movendo
unsigned long PTime[N_POTS] = {}; // Tempo previamente armazenado
unsigned long timer[N_POTS] = {}; // Armazena o tempo decorrido desde que o cronômetro foi zerado

// filtro one pole
// y[n] = A0 * x[n] + B1 * y[n-1];
// A = 0.15 and B = 0.85
float filterA = 0.3;
float filterB = 0.7;

#endif // USING_POTENTIOMETERS

/////////////////////////////////////////////
// ENCODERS
#ifdef USING_ENCODER

//#define TRAKTOR 1 // descomente se estiver usando com traktor, comente se não

const byte N_ENCODERS = 2; //* número de encoders
const byte N_ENCODER_PINS = N_ENCODERS * 2; // número de pinos usados pelos codificadores
const byte N_ENCODER_MIDI_CHANNELS = 16; // número de ENCODER_MIDI_CHs

byte ENCODER_CC_N[N_ENCODERS] = {17, 18}; //* Adicione o CC NUMBER de cada encoder que você deseja

Encoder encoder[N_ENCODERS] = {{10, 16}, {14, 15}}; //* os dois pinos de cada encoder - Use pinos com interrupts!

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

byte lastEncoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {127};
byte encoderValue[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {127};

// para os canais dos encoders - Usado para bancos diferentes

byte lastEncoderChannel = 0;

unsigned long encPTime[N_ENCODERS] = {0};
unsigned long encTimer[N_ENCODERS] = {0};
boolean encMoving[N_ENCODERS] = {false};
boolean encMoved[N_ENCODERS] = {false};
byte encTIMEOUT = 50;
byte encoder_n;
byte encTempVal = 0;

#endif
/////////////////////////////////////////////

/////////////////////////////////////////////
// CANAL MIDI
byte POT_MIDI_CH = 0; //* Cana MIDI a ser usado nos pots
byte BUTTON_MIDI_CH = 0; // nos botões
byte ENCODER_MIDI_CH = 0; // nos encoders

byte NOTE = 36; //* NOTA mais grave a ser usada - se não estiver usando CUSTOM NOTE personalizado
byte CC = 1; //* Menor CC MIDI a ser usado - se não estiver usando NÚMERO CC personalizado

/////////////////////////////////////////////
// NEOPIXEL | MIDI CHANNEL MENU
#ifdef USING_NEOPIXEL

boolean channelMenuOn = false;
byte midiChMenuColor = 200; //* menu color HUE - 0-255
byte midiChOnColor = midiChMenuColor + 60; // cor em HUE do menu
byte noteOffHue = 135; //* HUE quando as notas não são tocadas - 135 (azul)
byte noteOnHue = 240; //* HUE das notas quando são tocadas - 240 (magenta)

#endif // USING_NEOPIXEL

/////////////////////////////////////////////////////////////////////
// THREADS
// Essa libs cria uma thread "falsa". Isso significa que você pode fazer algo acontecer a cada x milisseconds
// podemos usar isso para ler algo em um intervalo, em vez de lê-lo a cada loop
// Neste caso, vamos ler os potenciômetros (ou outras coisas) em uma thread, fazendo a leitura dos botões mais rapidamente
ThreadController cpu; //thread master, onde as threads serão adicionadas
Thread threadPotentiometers; // thread to control the pots
Thread threadChannelMenu; // thread para controlar os pots
#ifdef USING_BANKS_WITH_BUTTONS
Thread threadBanksWithButtons;
#endif

/////////////////////////////////////////////
// DISPLAY
#ifdef USING_DISPLAY

byte display_pos_x = 28; //* pos x
byte display_pos_y = 7; //* pos y
byte display_text_size = 7; //* tamanho da fonte
#endif

/////////////////////////////////////////////
// 75HC595
#ifdef USING_74HC595

unsigned char maxBrightness = 255; //*
unsigned int numRegisters = 2; //*
const byte numOutputs = 14; //*

unsigned char pwmFrequency = 75;
//unsigned int numOutputs = numRegisters * 8;
unsigned int numRGBLeds = numRegisters * 8 / 3;
unsigned int fadingMode = 0; // Começa com todos os LEDs off

byte brightness = 0;

byte ledColor1 = 255; // 0-255
byte ledColor2 = 100;
byte ledColor3 = 180;
//byte ledColor4 = 10;

/////////////////////////////////////////////
// VU
#ifdef USING_VU

const byte N_LED_PER_VU = 7; // quantidade de led por VU
byte VuL[N_LED_PER_VU] = {0, 1, 2, 3, 4, 5, 6}; // pinos do Vu da esquerda
byte VuR[N_LED_PER_VU] = {7, 8, 9, 10, 11, 12, 13}; // pinos do Vu da esquerda

byte VU_MIDI_CH = 10; // canal a se ouvir o VU - 1

byte vuLcc = 12; // CC esquerda
byte vuRcc = 13; // CC direita

#endif //USING_VU

#ifdef USING_LED_FEEDBACK

byte ledPin[numOutputs] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

#endif

#endif // USING_74HC595
