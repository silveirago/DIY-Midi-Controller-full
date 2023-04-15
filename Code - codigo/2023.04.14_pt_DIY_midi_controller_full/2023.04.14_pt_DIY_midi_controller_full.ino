/*
Feito por Gustavo Silveira, 2023.

[http://www.musiconerd.com](http://www.musiconerd.com/)[http://www.youtube.com/musiconerd](http://www.youtube.com/musiconerd)[http://facebook.com/musiconerdmusiconerd](http://facebook.com/musiconerdmusiconerd)[http://instagram.com/musiconerd/](http://instagram.com/musiconerd/)[http://www.gustavosilveira.net](http://www.gustavosilveira.net/)[gustavosilveira@musiconerd.com](mailto:gustavosilveira@musiconerd.com)

Se você estiver usando para algo que não seja para uso pessoal, não se esqueça de dar crédito.

PS: Basta alterar o valor que tem um comentário como " //* "

*/

/////////////////////////////////////////////
// Escolhendo sua placa
// Defina sua placa, escolha:
// "ATMEGA328" se estiver usando ATmega328 - Uno, Mega, Nano...
// "ATMEGA32U4" se estiver usando com ATmega32U4 - Micro, Pro Micro, Leonardo...
// "TEENSY" se estiver usando uma placa Teensy
// "BLEMIDI" se estiver usando BLE MIDI (ESP32)
// "DEBUG" se você quiser apenas depurar o código no monitor serial

#define ATMEGA32U4 1  //* coloque aqui o uC que você está usando, como nas linhas acima seguido por "1", como "ATMEGA328 1", "DEBUG 1", etc.
//#define DEBUG 1

/////////////////////////////////////////////
// Você está usando botões?
//#define USING_BUTTONS 1  //* comente se não estiver usando botões

/////////////////////////////////////////////
// Você está usando potenciômetros?
//#define USING_POTENTIOMETERS 1  //* comente se não estiver usando potenciômetros

// Você está usando faders de alta resolução?
//#define USING_HIGH_RES_FADERS 1 //* comente se não estiver usando faders de alta resolução (qualquer fader pode ser de alta resolução)

// Você está usando faders motorizados?
//#define USING_MOTORIZED_FADERS 1 //* comente se não estiver usando um fader motorizado

/////////////////////////////////////////////
// Você está usando um multiplexador?
//#define USING_MUX 1 //* comente se não estiver usando um multiplexador, descomente se estiver usando.

/////////////////////////////////////////////
// Você está usando encoders?
//#define USING_ENCODER 1  //* comente se não estiver usando encoders, descomente se estiver usando.
//#define USING_ENCODER_MCP23017 1
//#define TRAKTOR 1 // descomente se estiver usando com o traktor, comente se não

// Você está usando ENCODER de alta resolução?
//#define USING_HIGH_RES_ENC 1  //* comente se não estiver usando faders de alta resolução (qualquer fader pode ser de alta resolução)

/////////////////////////////////////////////
// Você está usando neopixels (tiras endereçáveis)?
//#define USING_NEOPIXEL 1 //* comente se não estiver usando neopixels, descomente se estiver usando.

/////////////////////////////////////////////
// Você está usando um Display I2C Oled?
//#define USING_DISPLAY 1 //* comente se não estiver usando um Display I2C Oled.

/////////////////////////////////////////////
// Você está usando bancos que podem ser alternados com 2 botões?
//#define USING_BANKS_WITH_BUTTONS 1 //* comente se não estiver usando bancos com botões.

//#define BANKS_FOR_BUTTONS 1
//#define BANKS_FOR_POTS 1
//#define BANKS_FOR_ENCODERS 1

// Você está usando feedback de nota LED?
//#define USING_LED_FEEDBACK 1  //* comente se não estiver usando um VU

/////////////////////////////////////////////
// Você está usando um Bit Shifter 74HC595?
// Abaixo, use "USING_VU" ou "USING_LED_FEEDBACK"
// Use VU para monitorar níveis de áudio ou feedback de LED para obter feedback de notas

//#define USING_74HC595 1 //* comente se não estiver usando o Bit Shifter 74HC595

// Você está usando um VU (medidor led)?
//#define USING_VU 1//* comente se não estiver usando um VU

// Você está usando o protocolo Mackie?
//#define USING_MACKIE 1

// Você está usando dois botões para alterar a oitava?
//#define USING_OCTAVE 1

// Você está usando um conector físico MIDI din de 5 pinos?
//#define MIDI_DIN 1

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// BIBLIOTECAS
// -- Define a biblioteca MIDI -- //

//////////////////////////////////////
// Se estiver usando o Fast Led
#ifdef USING_NEOPIXEL

#include "FastLED.h"  // por Daniel Garcia - [http://fastled.io](http://fastled.io/)

FASTLED_USING_NAMESPACE

#define DATA_PIN 9
//#define CLK_PIN   4
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 4

CRGB leds[NUM_LEDS];
byte ledIndex[NUM_LEDS] = { 0, 1, 2, 3 };

#define BRIGHTNESS 96  // 0-255
#define FRAMES_PER_SECOND 120

#endif
//////////////////////////////////////

// se estiver usando com ATmega328 - Uno, Mega, Nano...
#ifdef ATMEGA328
#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();
#endif

// se estiver usando com ATmega32U4 - Micro, Pro Micro, Leonardo...
#ifdef ATMEGA32U4
#include <MIDIUSB.h>

#ifdef MIDI_DIN
#include <MIDI.h>  // adiciona a biblioteca MIDI para usar a serial de hardware com um cabo MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi2);
#endif  //MIDI_DIN

#endif  // ATMEGA32U4

#ifdef BLEMIDI
#include <BLEMidi.h>  // [https://github.com/max22-/ESP32-BLE-MIDI](https://github.com/max22-/ESP32-BLE-MIDI)
// Documentação: [https://www.arduino.cc/reference/en/libraries/esp32-ble-midi/](https://www.arduino.cc/reference/en/libraries/esp32-ble-midi/)
char bleMIDIDeviceName[] = { "BLE Controller" };  // coloque aqui o nome que você deseja para o seu dispositivo
#endif

//////////////////////////////////////
// se estiver usando o 74HC595 bit shifter
#ifdef USING_74HC595

// Você pode escolher o pino de latch você mesmo.
const int ShiftPWM_latchPin = 18;

// ** descomente esta parte para NÃO usar a porta SPI e alterar os números dos pinos. Isso é 2,5 vezes mais lento **
#define SHIFTPWM_NOSPI
const int ShiftPWM_dataPin = 9;
const int ShiftPWM_clockPin = 19;

// Se seus LEDs acenderem se o pino estiver baixo, defina-o como true, caso contrário, defina-o como false.
const bool ShiftPWM_invertOutputs = false;

// Você pode habilitar a opção abaixo para deslocar a fase do PWM de cada registro de deslocamento em 8 em comparação com o anterior.
// Isso aumentará ligeiramente a carga de interrupção, mas evitará que todos os sinais PWM se tornem altos ao mesmo tempo.
// Isso será um pouco mais fácil em sua fonte de alimentação, porque os picos de corrente são distribuídos.
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>  // inclua ShiftPWM.h após definir os pinos!
// Download: [https://github.com/elcojacobs/ShiftPWM](https://github.com/elcojacobs/ShiftPWM)

#endif  // USING_74HC595

//////////////////////
// Adicione esta biblioteca se estiver usando um multiplexador cd4067
#ifdef USING_MUX
#include <Multiplexer4067.h>  // Biblioteca Multiplexer CD4067 >> [https://github.com/sumotoy/Multiplexer4067](https://github.com/sumotoy/Multiplexer4067)
#endif

//////////////////////
// Threads
#include <Thread.h>            // Biblioteca Threads >> [https://github.com/ivanseidel/ArduinoThread](https://github.com/ivanseidel/ArduinoThread)
#include <ThreadController.h>  // O mesmo que acima

//////////////////////
// Encoder
#ifdef USING_ENCODER
// No gerenciador de downloads, baixe a biblioteca Encoder de Paul Stoffregen (ela vem com o Teensy)
#include <Encoder.h>  // faz todo o trabalho para você na leitura do encoder
#endif

#ifdef USING_ENCODER_MCP23017
#include <Adafruit_MCP23X17.h>  // Inclua a biblioteca Adafruit_MCP23X17 >> [https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library)
//Adafruit_MCP23X17 mcp; // Cria uma instância da classe Adafruit_MCP23X17
#endif

//////////////////////
// Oled Display I2C
#ifdef USING_DISPLAY
//#include <Adafruit_GFX.h>  // Inclua a biblioteca de gráficos principais para o display
#include <Adafruit_SSD1306.h>  // Inclua a biblioteca Adafruit_SSD1306 para controlar o display
//#include <Fonts/FreeMonoBold12pt7b.h>  // adicione uma fonte personalizada
//#include <Fonts/FreeMono9pt7b.h>  // adicione uma fonte personalizada
Adafruit_SSD1306 display(128, 64);  // Cria o display - tamanho do display em pixels
#endif

///////////////////////////////////////////
// MULTIPLEXADORES
#ifdef USING_MUX

#define N_MUX 1  //* número de multiplexadores
//* Defina os pinos s0, s1, s2, s3 e x
#define s0 18
#define s1 19
#define s2 20
#define s3 21

#define x1 A0  // pino analógico do primeiro mux
//#define x2 A1 // pino analógico do segundo mux
//#define x3 A2 // pino analógico do terceiro mux

// adicione mais #define e o número x se necessário

// *** IMPORTANTE: se você quiser adicionar mais de um mux! ***
// Na guia Configuração, linha 123, adicione outro "pinMode(x2, INPUT_PULLUP);" se quiser adicionar um segundo mux,
// e "pinMode(x3, INPUT_PULLUP);" para um terceiro mux, e assim por diante...

// Inicializa o multiplexador
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1),  // O pino SIG onde o multiplexador está conectado
  //  Multiplexer4067(s0, s1, s2, s3, x2), // O pino SIG onde o multiplexador está conectado
  //  Multiplexer4067(s0, s1, s2, s3, x3) // O pino SIG onde o multiplexador está conectado
  // ...
};
#endif

#ifdef USING_POTENTIOMETERS
// incluir a biblioteca ResponsiveAnalogRead
#include <ResponsiveAnalogRead.h>  // [https://github.com/dxinteractive/ResponsiveAnalogRead](https://github.com/dxinteractive/ResponsiveAnalogRead)
#endif

#ifdef USING_MOTORIZED_FADERS
#include <CapacitiveSensor.h>  // Adiciona capacidades de toque para o fader motorizado
#endif

#ifdef USING_MACKIE
#include "MACKIE.h"
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Tipos de mensagens MIDI a serem enviadas - NÃO TOQUE
byte NN = 0;  // Número da nota ou MACKIE
byte CC = 1;  // Mudança de controle
byte T = 2;   // Alternar
byte PC = 3;  // Programa mudança
byte PB = 4;  // Pitch Bend

/////////////////////////////////////////////
// BOTÕES
#ifdef USING_BUTTONS

const byte N_BUTTONS = 3;                                //* número total de botões. Número de botões no Arduino + número de botões no multiplexador 1 + número de botões no multiplexador 2... (NÃO coloque aqui botões de oitava e de canal MIDI (banco))
const byte N_BUTTONS_ARDUINO = 3;                        //* número de botões conectados diretamente ao Arduino
const byte BUTTON_ARDUINO_PIN[N_BUTTONS] = { 2, 3, 4 };  //* pinos de cada botão conectado diretamente ao Arduino

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX                                      // Preencha se estiver usando multiplexador, caso contrário, apenas deixe
const byte N_BUTTONS_PER_MUX[N_MUX] = { 16, 13, 5 };  //* número de botões em cada mux (em ordem)
const byte BUTTON_MUX_PIN[N_MUX][16] = {
  //* pino de cada botão de cada mux na ordem

  { 1, 2, 0, 5, 4, 3, 6, 7, 10, 9, 8, 13, 12, 11, 15, 14 },  // 1
  { 2, 1, 0, 5, 4, 3, 7, 6, 9, 8, 13, 12, 15 },              // 2
  { 6, 5, 4, 3, 8 },
  // ...
};

int buttonMuxThreshold = 850;

#endif  //USING_MUX

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Que tipo de mensagem você deseja enviar?
// Número da nota - Mudança de controle - Alternar - Programa de mudança

// NN: número da nota ou MACKIE
// CC: Mudança de controle
// T: Alternar
// PC: Programa de mudança

//* Coloque aqui o tipo de mensagem que você deseja enviar, na mesma ordem em que declarou os pinos do botão
// "NN" para número da nota | "CC" para mudança de controle | "T" para número da nota, mas no modo de alternância | "PC" para mudança de programa
byte MESSAGE_TYPE[N_BUTTONS] = { NN, NN, NN };

//* Coloque aqui o número da mensagem que você deseja enviar, na ordem correta, não importa se é um número de nota
byte MESSAGE_VAL[N_BUTTONS] = { 36, 37, 38 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_BANKS_WITH_BUTTONS

//#define USING_MUX_BANK_BUTTON_PIN 1; // Defina se você está usando os botões do banco no pino Mux. Tem que ser o primeiro mux.
const byte BANK_BUTTON_PIN[2] = { 16, 10 };  //* primeiro diminuirá o canal MIDI e o segundo aumentará

#endif  //USING_BANKS_WITH_BUTTONS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_OCTAVE

#define USING_MUX_OCTAVE_PIN 1;              // Defina se você está usando os botões de oitava no pino Mux. Tem que ser o primeiro mux.
const byte OCTAVE_BUTTON_PIN[2] = { 5, 4 };  //* primeiro diminuirá o canal MIDI e o segundo aumentará

#endif  // USING_OCTAVE

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//#define pin13 1 // descomente se estiver usando o pino 13 (pino com led), ou comente a linha se não estiver
byte pin13index = 12;  //* coloque o índice do pino 13 do array buttonPin[] se estiver usando, se não, comente

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// isso acontecerá se você estiver usando o neo pixel
// este botão abrirá o menu para que você possa alterar o canal MIDI
// pressionando um botão. Cada botão será um canal diferente

#ifdef USING_NEOPIXEL
const byte CHANNEL_BUTTON_PIN = 8;
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// debounce
unsigned long debounceDelay = 50;  //* o tempo de debounce; aumente se a saída piscar

#endif  // USING_BUTTONS

/////////////////////////////////////////////
// POTENCIÔMETROS

#ifdef USING_POTENTIOMETERS

const byte N_POTS = 2;  //* número total de potenciômetros (slide e rotativo). Número de potenciômetros no Arduino + número de potenciômetros no multiplexador 1 + número de potenciômetros no multiplexador 2...

const byte N_POTS_ARDUINO = 2;  //* número de potenciômetros conectados diretamente ao Arduino
// Se estiver usando o Arduino, declare como "A1, A2"
// Se estiver usando o ESP32, use apenas o número GPIO como "11, 10"
const byte POT_ARDUINO_PIN[N_POTS_ARDUINO] = { A2, A1 };  //* pinos de cada potenciômetro conectado diretamente ao Arduino (não use "A" se estiver usando o ESP32, apenas o número)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX
const byte N_POTS_PER_MUX[N_MUX] = { 0, 3, 1 };  //* número de potenciômetros em cada multiplexador (em ordem)
const byte POT_MUX_PIN[N_MUX][16] = {
  //* pinos de cada potenciômetro de cada mux na ordem que você deseja que eles estejam
  {},  //* pinos do primeiro mux
  { 10, 11, 14 },
  { 7 }
  // ...
};
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define USING_CUSTOM_CC_N 1  //* comentar se não estiver usando números CC personalizados, descomentar se estiver usando.
#ifdef USING_CUSTOM_CC_N

// Que tipo de mensagem você deseja enviar?
// Mudança de controle - Pitch Bend

// CC: Mudança de controle
// PB: Pitch Bend

//* Coloque aqui o tipo de mensagem que você deseja enviar, na mesma ordem em que declarou os pinos do botão
// "CC" para Mudança de Controle | "PB" para Pitch Bend
byte MESSAGE_TYPE_POT[N_POTS] = { CC, PB };

byte POT_CC_N[N_POTS] = { 1, 2 };  // Adicione o NÚMERO CC ou MACKIE de cada potenciômetro que você deseja

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const int TIMEOUT = 300;      //* Quantidade de tempo que o potenciômetro será lido após exceder o varThreshold
const byte varThreshold = 8;  //* Limite para a variação do sinal do potenciômetro - Aumente se estiver usando ESP32
// Para varThreshold, geralmente uso "8" para o Arduino normal e "30" para o ESP32

// coloque aqui a leitura mínima e máxima no potCState
// coloque um pouco mais em potMin e um pouco menos em potMax
// IMPORTANTE:
// Arduinos regulares têm resolução de 10 bits: 0 - 1023
// As placas ESP32 têm resolução de 12 bits: 0 - 4095
const int potMin = 20;
const unsigned int potMax = 940;

#endif  // USING_POTENTIOMETERS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/////////////////////////////////////////////
// ENCODERS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CALIBRAÇÃO
byte encoderMinVal = 0;  //* valor mínimo do encoder

#ifdef USING_HIGH_RES_ENC
const unsigned int encoderMaxVal = 16383;  //* valor máximo do encoder de alta resolução de 14 bits
#else
const byte encoderMaxVal = 127;  //* valor máximo do encoder MIDI normal de 7 bits
#endif

//#endif

// Sensibilidade do Encoder
// escolha entre um número (float) maior que 0 (0,2, por exemplo) e 4.
// "1" fornecerá 4 leituras por clique, aumentará em 4, como 4, 8, 12 ...
// "4" fornecerá 1 leitura por clique, como 1, 2, 3, 4 ...
// então "4" dará a leitura mais precisa
const float encSensitivity = 0.1;  //* mude a sensibilidade do encoder aqui

const byte N_ENCODER_MIDI_CHANNELS = 16;  // número de ENCODER_MIDI_CHs (NÃO ALTERE)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PINOS

#ifdef USING_ENCODER

const byte N_ENCODERS = 2;                   //* número de encoders
const byte N_ENCODER_PINS = N_ENCODERS * 2;  //número de pinos usados pelos encoders

//Encoder encoder[N_ENCODERS] = { { 10, 16 } };  // os dois pinos de cada encoder - Use pinos com interrupções!
Encoder encoder[N_ENCODERS] = { { 10, 16 }, { 14, 15 } };  // os dois pinos de cada encoder - Use pinos com interrupções!

// Escolha o valor CC ou MACKIE
byte ENCODER_CC_N[N_ENCODERS] = { 11, 12 };  // Adicione o NÚMERO DO CC de cada encoder que desejar

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

byte preset[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {
  //* armazena predefinições para iniciar seus encoders
  // {64, 64}, // ch 1
  // {64, 64}, // ch 2
  // {64, 64}, // ch 3
  // {64, 64}, // ch 4
  // {64, 64}, // ch 5
  // {64, 64}, // ch 6
  // {64, 64}, // ch 7
  // {64, 64}, // ch 8
  // {64, 64}, // ch 9
  // {64, 64}, // ch 10
  // {64, 64}, // ch 11
  // {64, 64}, // ch 12
  // {64, 64}, // ch 13
  // {64, 64}, // ch 14
  // {64, 64}, // ch 15
  // {64, 64} // ch 16
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MACKIE
byte encoderSens = 1;  // 1-63
#endif

#endif  //USING_ENCODER

/////////////////////////////////////////////
// ENCODER MCP23017

#ifdef USING_ENCODER_MCP23017

const int I2C_ADDRESS = 0x20;  //* endereço I2C do MCP23017

const byte N_ENC_MCP23017 = 6;      // Número de encoders usados
const byte N_ENC_CH_MCP23017 = 16;  // número de ENCODER_MIDI_CHs

int encoderPin[N_ENC_MCP23017][2] = { { 8, 9 }, { 0, 1 }, { 12, 13 }, { 10, 11 }, { 2, 3 }, { 14, 15 } };  // Números de pino para os canais A e B de cada encoder
int INT_PIN = 8;                                                                                           // pino do microcontrolador conectado ao INTA/B

int count[N_ENC_MCP23017] = { 0 };      // Contagem atual de cada encoder
int lastCount[N_ENC_MCP23017] = { 0 };  // Contagem anterior de cada encoder

int encoderA[N_ENC_MCP23017] = { 0 };         // Estado atual do canal A de cada encoder
int encoderB[N_ENC_MCP23017] = { 0 };         // Estado atual do canal B de cada encoder
int lastEncoderA[N_ENC_MCP23017] = { HIGH };  // Estado anterior do canal A de cada encoder
int lastEncoderB[N_ENC_MCP23017] = { HIGH };  // Estado anterior do canal B de cada encoder

byte ENCODER_CC_N[N_ENC_MCP23017] = { 15, 16, 17, 18, 19, 20 };  // Adicione o NÚMERO DO CC de cada encoder que desejar

Adafruit_MCP23X17 mcp;  // Crie uma instância da classe Adafruit_MCP23X17

#endif  //USING_ENCODER_MCP23017

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/////////////////////////////////////////////
// CANAL MIDI
byte POT_MIDI_CH = 0;  //* canal MIDI a ser usado
byte BUTTON_MIDI_CH = 0;
byte ENCODER_MIDI_CH = 1;

byte NOTE = 36;      //* Nota mais baixa a ser usada - se não estiver usando um número de NOTA personalizado
byte CC_NUMBER = 1;  //* Menor CC MIDI a ser usado - se não estiver usando um número de CC personalizado

/////////////////////////////////////////////
// NEOPIXEL | MENU DE CANAL MIDI
#ifdef USING_NEOPIXEL

boolean channelMenuOn = false;
byte midiChMenuColor = 200;                 //* cor do menu HUE - 0-255
byte midiChOnColor = midiChMenuColor + 60;  // cor do menu ligado HUE
byte noteOffHue = 135;                      //* HUE quando as notas não são executadas - 135 (azul)
byte noteOnHue = 240;                       //* HUE das notas quando são executadas - 240 (magenta)

#endif  // USING_NEOPIXEL

/////////////////////////////////////////////
// DISPLAY
byte display_pos_x = 27;     // pos x
byte display_pos_y = 7;      // pos y
byte display_text_size = 7;  // tamanho da fonte de texto

#ifdef USING_LED_FEEDBACK
const byte numOutputs = 3;              // /* coloque aqui o número de LEDs que você usará
byte ledPin[numOutputs] = { 5, 6, 9 };  // pinos dos LEDs (arduino ou bit shifter)
#endif
/////////////////////////////////////////////
// 75HC595
#ifdef USING_74HC595

unsigned char maxBrightness = 255;  //*
unsigned int numRegisters = 2;      //*

unsigned char pwmFrequency = 75;
//unsigned int numOutputs = numRegisters * 8;
unsigned int numRGBLeds = numRegisters * 8 / 3;
unsigned int fadingMode = 0;  //comece com todos os LEDs desligados.

byte brightness = 0;

byte ledColor1 = 255;  // 0-255
byte ledColor2 = 100;
byte ledColor3 = 180;
//byte ledColor4 = 10;

/////////////////////////////////////////////
// VU
#ifdef USING_VU

const byte N_LED_PER_VU = 7;
byte VuL[N_LED_PER_VU] = { 0, 1, 2, 3, 4, 5, 6 };      // pinos VU esquerdo
byte VuR[N_LED_PER_VU] = { 7, 8, 9, 10, 11, 12, 13 };  // pinos VU direito

byte VU_MIDI_CH = 10;  // canal para ouvir o VU -1

byte vuLcc = 12;  // CC esquerdo
byte vuRcc = 13;  // CC direito

#endif  //USING_VU

#endif  // USING_74HC595

/////////////////////////////////////////////
// MOTORIZED FADERS
#ifdef USING_MOTORIZED_FADERS

//Atribuições de pinos Arduino
const byte N_M_FADERS = 1;
const byte motorDownPin[N_M_FADERS] = { 4 };   //Controle H-Bridge para fazer o motor descer
const byte motorUpPin[N_M_FADERS] = { 5 };     //Controle H-Bridge para fazer o motor subir
const byte faderSpeedPin[N_M_FADERS] = { 9 };  // pino de velocidade do motor (PWM)

byte faderSpeed[N_M_FADERS] = { 240 };  // 0-255
byte faderSpeedMin = 150;               // 0-255 - 140?
byte faderSpeedMax = 255;               // 0-255 -  250?

const byte motorStopPoint = 18;  // o motor irá parar X valores antes de atingir seu objetivo. Aumente para evitar jittery (será menos preciso).

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// variáveis que você não precisa mudar
int faderPos[N_M_FADERS] = { 0 };   // posição do fader
int faderPPos[N_M_FADERS] = { 0 };  // posição anterior do fader
int faderMax[N_M_FADERS] = { 0 };   //Valor lido pela posição máxima do fader (0-1023)
int faderMin[N_M_FADERS] = { 0 };   //Valor lido pela posição mínima do fader (0-1023)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Midi
byte pFaderInVal[16][N_M_FADERS] = { 0 };
byte pFaderPBInVal[N_M_FADERS] = { 0 };  // Pitch bend para Mackie

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Cap Sense
boolean isTouched[N_M_FADERS] = { false };   // O fader está sendo tocado atualmente?
boolean pIsTouched[N_M_FADERS] = { false };  // anterior O fader está sendo tocado atualmente?

long touchLine[N_M_FADERS] = { 0 };

unsigned long capTimeNow[N_M_FADERS] = { 0 };

// Sensor Capacitivo - Pino de Toque
// Resistor de 10M entre os pinos 7 e 8, o pino 2 é o pino do sensor, adicione um fio ou folha se desejar
CapacitiveSensor capSense[N_M_FADERS] = {
  CapacitiveSensor(7, 8)
};  // capSense = CapacitiveSensor(7, 8)

const int touchSendPin[N_M_FADERS] = { 7 };     // Pino de envio para o circuito de detecção de capacitância (Digital 7)
const int touchReceivePin[N_M_FADERS] = { 8 };  // Pino de recebimento para o circuito de detecção de capacitância (Digital 8)

byte capSenseSpeed = 15;   // número de amostras
int capSensitivity = 150;  // limiar de toque

int capTimeout = 500;

#endif  // USING_MOTORIZED_FADERS
