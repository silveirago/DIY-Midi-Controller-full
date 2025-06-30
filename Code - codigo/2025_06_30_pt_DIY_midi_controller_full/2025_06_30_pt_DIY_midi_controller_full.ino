/*
  Feito por Gustavo Silveira, 2025.

  http://www.musiconerd.com
  http://www.youtube.com/musiconerd
  http://facebook.com/musiconerdmusiconerd
  http://instagram.com/musiconerd/
  http://www.gustavosilveira.net
  gustavosilveira@musiconerd.com

  Se você estiver usando para algo que não seja uso pessoal, não se esqueça de dar o crédito.

  PS: Apenas altere o valor que tem um comentário como " //* "

*/

/////////////////////////////////////////////
// Escolhendo sua placa
// Defina sua placa, escolha:
// "ATMEGA328" se estiver usando ATmega328 - Uno, Mega, Nano...
// "ATMEGA32U4" se estiver usando com ATmega32U4 - Micro, Pro Micro, Leonardo...
// "TEENSY" se estiver usando uma placa Teensy
// "BLEMIDI" se estiver usando BLE MIDI (ESP32)
// "DEBUG" se você só quiser depurar o código no monitor serial

#define DEBUG 1  //* coloque aqui o uC que você está usando, como nas linhas acima seguido de "1", por exemplo "ATMEGA328 1", "DEBUG 1", etc.

/////////////////////////////////////////////
// Você está usando botões?
#define USING_BUTTONS 1  //* comente se não estiver usando botões

/////////////////////////////////////////////
// Você está usando potenciômetros?
#define USING_POTENTIOMETERS 1  //* comente se não estiver usando potenciômetros

// Você está usando faders de alta resolução?
//#define USING_HIGH_RES_FADERS 1 //* comente se não estiver usando faders de alta resolução (qualquer fader pode ser de alta resolução)

// Você está usando faders motorizados?
//#define USING_MOTORIZED_FADERS 1 //* comente se não estiver usando um fader motorizado

/////////////////////////////////////////////
// Você está usando um multiplexador?
//#define USING_MUX 1  //* comente se não estiver usando um multiplexador, descomente se estiver usando.

/////////////////////////////////////////////
// Você está usando encoders?
//#define USING_ENCODER 1  //* comente se não estiver usando encoders, descomente se estiver usando.
//#define USING_ENCODER_MCP23017 1
//#define TRAKTOR 1 // descomente se estiver usando com traktor, comente se não

// Você está usando encoder de alta resolução?
//#define USING_HIGH_RES_ENC 1  //* comente se não estiver usando encoder de alta resolução (qualquer encoder pode ser de alta resolução)

/////////////////////////////////////////////
// Você está usando neopixels (qualquer fita endereçável)?
//#define USING_NEOPIXEL 1 //* comente se não estiver usando neopixels, descomente se estiver usando.

/////////////////////////////////////////////
// Você está usando um Display OLED I2C?
//#define USING_DISPLAY 1 //* comente se não estiver usando um Display OLED I2C.

/////////////////////////////////////////////
// Você está usando bancos que podem ser trocados com 2 botões?
//#define USING_BANKS_WITH_BUTTONS 1 //* comente se não estiver usando bancos com botões.

//#define BANKS_FOR_BUTTONS 1
//#define BANKS_FOR_POTS 1
//#define BANKS_FOR_ENCODERS 1

// Você está usando feedback de nota com LED?
//#define USING_LED_FEEDBACK 1  //* comente se não estiver usando um VU

/////////////////////////////////////////////
// Você está usando um conversor de bits 74HC595?
// Abaixo, use ou "USING_VU" ou "USING_LED_FEEDBACK"
// Use VU para monitorar níveis de áudio, ou LED feedback para obter feedback de notas

//#define USING_74HC595 1 //* comente se não estiver usando conversor de bits 74HC595

// Você está usando um VU (medidor de LED)?
//#define USING_VU 1//* comente se não estiver usando um VU

// Você está usando o Protocolo Mackie?
//#define USING_MACKIE 1

// Você está usando dois botões para mudança de oitava?
//#define USING_OCTAVE 1

// Você está usando conector MIDI DIN 5 pinos físico?
//#define MIDI_DIN 1


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
// BIBLIOTECAS
// -- Define a biblioteca MIDI -- //

//////////////////////////////////////
// Se estiver usando Fast Led
#ifdef USING_NEOPIXEL

#include "FastLED.h"  // por Daniel Garcia - http://fastled.io

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
#include <MIDI.h>  // adiciona a biblioteca MIDI para usar a serial hardware com um cabo MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi2);
#endif  //MIDI_DIN

#endif  // ATMEGA32U4

#ifdef BLEMIDI
#include <BLEMidi.h>  // https://github.com/max22-/ESP32-BLE-MIDI
// Documentação: https://www.arduino.cc/reference/en/libraries/esp32-ble-midi/
char bleMIDIDeviceName[] = { "BLE Controller" };  // coloque aqui o nome que você deseja para o seu dispositivo
#endif

//////////////////////////////////////
// se estiver usando o conversor de bits 74HC595
#ifdef USING_74HC595

// Você pode escolher o pino latch manualmente.
const int ShiftPWM_latchPin = 18;

// ** descomente esta parte para NÃO usar a porta SPI e alterar os números dos pinos. Isso é 2.5x mais lento **
#define SHIFTPWM_NOSPI
const int ShiftPWM_dataPin = 9;
const int ShiftPWM_clockPin = 19;


// Se seus LEDs acendem quando o pino está baixo, defina isto como true, caso contrário, defina como false.
const bool ShiftPWM_invertOutputs = false;

// Você pode habilitar a opção abaixo para deslocar a fase do PWM de cada registrador de deslocamento em 8 em relação ao anterior.
// Isso aumentará um pouco a carga de interrupção, mas impedirá que todos os sinais PWM fiquem altos ao mesmo tempo.
// Isso será um pouco melhor para sua fonte de alimentação, pois os picos de corrente serão distribuídos.
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>  // inclua ShiftPWM.h depois de definir os pinos!
// Download: https://github.com/elcojacobs/ShiftPWM

#endif  // USING_74HC595

//////////////////////
// Adicione esta lib se estiver usando um multiplexador cd4067
#ifdef USING_MUX
#include <Multiplexer4067.h>  // Biblioteca Multiplexer CD4067 >> https://github.com/sumotoy/Multiplexer4067
#endif

//////////////////////
// Threads
#include <Thread.h>            // Biblioteca Threads >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h>  // Mesmo que acima

//////////////////////
// Encoder
#ifdef USING_ENCODER
// No gerenciador de downloads, baixe a biblioteca Encoder de Paul Stoffregen (ela vem com o Teensy)
#include <Encoder.h>  // faz todo o trabalho para você na leitura do encoder
#endif

#ifdef USING_ENCODER_MCP23017
#include <Adafruit_MCP23X17.h>  // Inclua a biblioteca Adafruit_MCP23X17 >> https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
//Adafruit_MCP23X17 mcp; // Crie uma instância da classe Adafruit_MCP23X17
#endif

//////////////////////
// Display OLED I2C
#ifdef USING_DISPLAY
//#include <Adafruit_GFX.h>  // Inclua a biblioteca core de gráficos para o display
#include <Adafruit_SSD1306.h>  // Inclua a biblioteca Adafruit_SSD1306 para controlar o display
//#include <Fonts/FreeMonoBold12pt7b.h>  // Adicione uma fonte customizada
//#include <Fonts/FreeMono9pt7b.h>  // Adicione uma fonte customizada
Adafruit_SSD1306 display(128, 64);  // Cria o display - tamanho do display em pixels
#endif

///////////////////////////////////////////
// MULTIPLEXADORES
#ifdef USING_MUX

#define N_MUX 1  // número de multiplexadores

// Defina os pinos s0, s1, s2, s3 e x
const int mux_s[4] = { 7,6,5,4 };
const int mux_x[N_MUX] = { A2 };  // pino analógico de cada multiplexador

// Declare um array de ponteiros (não altere)
Multiplexer4067* mux[N_MUX];

#endif

#ifdef USING_POTENTIOMETERS
// inclua a biblioteca ResponsiveAnalogRead
#include <ResponsiveAnalogRead.h>  // https://github.com/dxinteractive/ResponsiveAnalogRead
#endif

#ifdef USING_MOTORIZED_FADERS
#include <CapacitiveSensor.h>  // Adiciona capacidades de toque para o fader motorizado
#endif

#ifdef USING_MACKIE
#include "MACKIE.h"
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Tipos de mensagens MIDI para enviar - NÃO ALTERE
byte NN = 0;  // Número da nota ou MACKIE
byte CC = 1;  // Mudança de controle
byte T = 2;   // Alternar
byte PC = 3;  // Mudança de programa
byte PB = 4;  // Pitch Bend

/////////////////////////////////////////////
// BOTÕES
#ifdef USING_BUTTONS

const byte N_BUTTONS = 2 + 3;                         //* número total de botões. Número de botões no Arduino + número de botões no multiplexador 1 + número de botões no multiplexador 2... (NÃO coloque botões de oitava e canal MIDI (banco) aqui)
const byte N_BUTTONS_ARDUINO = 2;                     //* número de botões conectados diretamente ao Arduino
const byte BUTTON_ARDUINO_PIN[N_BUTTONS] = { 2, 3 };  //* pinos de cada botão conectados diretamente ao Arduino

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX                              // Preencha se estiver usando multiplexador, caso contrário, deixe como está
const byte N_BUTTONS_PER_MUX[N_MUX] = { 3 };  //* número de botões em cada mux (em ordem)
const byte BUTTON_MUX_PIN[N_MUX][16] = {
  //* pino de cada botão de cada mux em ordem

  { 2, 3, 4 },  // 1
  // { 2, 1, 0, 5, 4, 3, 7, 6, 9, 8, 13, 12, 15 },              // 2
  // { 6, 5, 4, 3, 8 },
  // ...
};

int buttonMuxThreshold = 850;

#endif  //USING_MUX

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Que tipo de mensagem você deseja enviar?
// Número da Nota - Mudança de Controle - Alternar - Mudança de Programa

// NN: Número da Nota ou MACKIE
// CC: Mudança de controle
// T: Alternar
// PC: Mudança de programa

//* Coloque aqui o tipo de mensagem que você deseja enviar, na mesma ordem em que declarou os pinos dos botões
// "NN" para Número da Nota | "CC" para Mudança de Controle | "T" para Número da Nota em modo alternado | "PC" para Mudança de Programa
byte MESSAGE_TYPE[N_BUTTONS] = { NN, NN, NN, NN, NN };

//* Coloque aqui o número da mensagem que você deseja enviar, na ordem correta, não importando se é um número de nota, CC (ou MACKIE), Mudança de Programa
byte MESSAGE_VAL[N_BUTTONS] = { 36, 37, 39, 42, 43 };


// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_BANKS_WITH_BUTTONS

//#define USING_MUX_BANK_BUTTON_PIN 1; // Defina se você está usando os botões de banco no pino do multiplexador. Tem que ser o primeiro mux.
const byte BANK_BUTTON_PIN[2] = { 16, 10 };  //* o primeiro diminuirá o canal MIDI e o segundo aumentará

#endif  //USING_BANKS_WITH_BUTTONS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_OCTAVE

//#define USING_MUX_OCTAVE_PIN 1;              // Defina se você está usando os botões de oitava no pino do multiplexador. Tem que ser o primeiro mux.
const byte OCTAVE_BUTTON_PIN[2] = { 5, 4 };  //* o primeiro diminuirá o canal MIDI e o segundo aumentará

#endif  // USING_OCTAVE

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Isso acontecerá se você estiver usando neopixel
// Este botão abrirá o menu para que você possa mudar o canal MIDI
// ao pressionar um botão. Cada botão será um canal diferente

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

const byte N_POTS = 2 + 2;  //* número total de potenciômetros (deslizante & rotativo). Número de potenciômetros no Arduino + número de potenciômetros no multiplexador 1 + número de potenciômetros no multiplexador 2...

const byte N_POTS_ARDUINO = 2;  //* número de potenciômetros conectados diretamente ao Arduino
// Se estiver usando o Arduino, declare como "A1, A2"
// Se estiver usando ESP32, use apenas o número GPIO, como "11, 10"
const byte POT_ARDUINO_PIN[N_POTS_ARDUINO] = { A0, A1 };  //* pinos de cada potenciômetro conectados diretamente ao Arduino (não use "A" se estiver usando ESP32, apenas o número)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef USING_MUX
const byte N_POTS_PER_MUX[N_MUX] = { 2 };  //* número de potenciômetros em cada multiplexador (em ordem)
const byte POT_MUX_PIN[N_MUX][16] = {
  //* pinos de cada potenciômetro de cada mux na ordem que você deseja
  { 0, 1 },  //* pinos do primeiro mux
  //{ 10, 11, 14 },
  //{ 7 }
  // ...
};
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define USING_CUSTOM_CC_N 1  //* comente se não estiver usando NÚMEROS CUSTOMIZADOS DE CC, descomente se estiver usando.
#ifdef USING_CUSTOM_CC_N

// Que tipo de mensagem você deseja enviar?
// Mudança de Controle - Pitch Bend

// CC: Mudança de controle
// PB: Pitch Bend

//* Coloque aqui o tipo de mensagem que você deseja enviar, na mesma ordem em que declarou os pinos dos botões
// "CC" para Mudança de Controle | "PB" para Pitch Bend
byte MESSAGE_TYPE_POT[N_POTS] = { CC, CC, CC, CC };

byte POT_CC_N[N_POTS] = { 1, 2, 3, 4 };  // Adicione o NÚMERO CC ou MACKIE de cada potenciômetro que você deseja

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const int TIMEOUT = 300;      //* Quantidade de tempo que o potenciômetro será lido após exceder o varThreshold
const byte varThreshold = 10;  //* Limiar para a variação do sinal do potenciômetro - Aumente se estiver usando ESP32
// Para varThreshold, normalmente uso "8" para Arduino normal e "30" para ESP32

// coloque aqui a leitura mínima e máxima no potCState
// no potMin coloque um pouco mais e no potMax um pouco menos
// IMPORTANTE:
// Arduinos regulares têm resolução de 10 bits: 0 - 1023
// Placas ESP32 têm resolução de 12 bits: 0 - 4095
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
const unsigned int encoderMaxVal = 16383;  //* valor máximo do encoder ALTA RES 14bit
#else
const byte encoderMaxVal = 127;  //* valor máximo do encoder NORMAL MIDI 7bit
#endif

// Sensibilidade do Encoder
// escolha entre um número (float) maior que 0 (por exemplo, 0.2) e 4.
// "1" lhe dará 4 leituras por clique, incrementando de 4 em 4, como 4, 8, 12...
// "4" lhe dará 1 leitura por clique, como 1, 2, 3, 4...
// então "4" fornecerá a leitura mais precisa
const float encSensitivity = 4;  //* altere a sensibilidade do encoder aqui

const byte N_ENCODER_MIDI_CHANNELS = 16;  // número de canais MIDI do encoder (NÃO ALTERE)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PINS

#ifdef USING_ENCODER

const byte N_ENCODERS = 1;                   //* número de encoders
const byte N_ENCODER_PINS = N_ENCODERS * 2;  // número de pinos usados pelos encoders

//Encoder encoder[N_ENCODERS] = { { 10, 16 } };  // os dois pinos de cada encoder - Use pinos com interrupções!
Encoder encoder[N_ENCODERS] = { { 9, 8 } };  // os dois pinos de cada encoder - Use pinos com interrupções!

// Escolha o valor de CC ou MACKIE
byte ENCODER_CC_N[N_ENCODERS] = { 21 };  // Adicione o NÚMERO CC de cada encoder que você deseja

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

byte preset[N_ENCODER_MIDI_CHANNELS][N_ENCODERS] = {
  //* armazena os presets para iniciar seus encoders
  //  {64, 64}, // canal 1
  //  {64, 64}, // canal 2
  //  {64, 64}, // canal 3
  //  {64, 64}, // canal 4
  //  {64, 64}, // canal 5
  //  {64, 64}, // canal 6
  //  {64, 64}, // canal 7
  //  {64, 64}, // canal 8
  //  {64, 64}, // canal 9
  //  {64, 64}, // canal 10
  //  {64, 64}, // canal 11
  //  {64, 64}, // canal 12
  //  {64, 64}, // canal 13
  //  {64, 64}, // canal 14
  //  {64, 64}, // canal 15
  //  {64, 64}  // canal 16

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

const byte N_ENC_MCP23017 = 6;      // Número de encoders utilizados
const byte N_ENC_CH_MCP23017 = 16;  // número de canais MIDI dos encoders

int encoderPin[N_ENC_MCP23017][2] = { { 8, 9 }, { 0, 1 }, { 12, 13 }, { 10, 11 }, { 2, 3 }, { 14, 15 } };  // Números dos pinos para os canais A e B de cada encoder
int INT_PIN = 8;                                                                                           // pino do microcontrolador conectado a INTA/B

int count[N_ENC_MCP23017] = { 0 };      // Contagem atual de cada encoder
int lastCount[N_ENC_MCP23017] = { 0 };  // Contagem anterior de cada encoder

int encoderA[N_ENC_MCP23017] = { 0 };         // Estado atual do canal A de cada encoder
int encoderB[N_ENC_MCP23017] = { 0 };         // Estado atual do canal B de cada encoder
int lastEncoderA[N_ENC_MCP23017] = { HIGH };  // Estado anterior do canal A de cada encoder
int lastEncoderB[N_ENC_MCP23017] = { HIGH };  // Estado anterior do canal B de cada encoder

byte ENCODER_CC_N[N_ENC_MCP23017] = { 15, 16, 17, 18, 19, 20 };  // Adicione o NÚMERO CC de cada encoder que você deseja

Adafruit_MCP23X17 mcp;  // Cria uma instância da classe Adafruit_MCP23X17

#endif  //USING_ENCODER_MCP23017

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/////////////////////////////////////////////
// CANAL MIDI
byte POT_MIDI_CH = 0;  //* Canal MIDI a ser usado
byte BUTTON_MIDI_CH = 0;
byte ENCODER_MIDI_CH = 1;

byte NOTE = 36;      //* Nota mais baixa a ser usada - se não estiver usando número de nota customizado
byte CC_NUMBER = 1;  //* Primeiro CC MIDI a ser usado - se não estiver usando número de CC customizado

/////////////////////////////////////////////
// NEOPIXEL | MENU DE CANAL MIDI
#ifdef USING_NEOPIXEL

boolean channelMenuOn = false;
byte midiChMenuColor = 200;                 //* COR do menu (HUE) - 0-255
byte midiChOnColor = midiChMenuColor + 60;  // COR do canal no menu (HUE)
byte noteOffHue = 135;                      //* HUE quando as notas não são tocadas - 135 (azul)
byte noteOnHue = 240;                       //* HUE das notas quando são tocadas - 240 (magenta)

#endif  // USING_NEOPIXEL


/////////////////////////////////////////////
// DISPLAY
byte display_pos_x = 27;     // posição x
byte display_pos_y = 7;      // posição y
byte display_text_size = 7;  // tamanho da fonte do texto

#ifdef USING_LED_FEEDBACK
const byte numOutputs = 3;              // /* coloque aqui o número de LEDs que você irá usar
byte ledPin[numOutputs] = { 5, 6, 9 };  // pinos dos LEDs (Arduino ou conversor de bits)
#endif
/////////////////////////////////////////////
// 75HC595
#ifdef USING_74HC595

unsigned char maxBrightness = 255;  //*
unsigned int numRegisters = 2;      //*

unsigned char pwmFrequency = 75;
//unsigned int numOutputs = numRegisters * 8;
unsigned int numRGBLeds = numRegisters * 8 / 3;
unsigned int fadingMode = 0;  // inicia com todos os LEDs apagados.

byte brightness = 0;

byte ledColor1 = 255;  // 0-255
byte ledColor2 = 100;
byte ledColor3 = 180;
//byte ledColor4 = 10;


/////////////////////////////////////////////
// VU
#ifdef USING_VU

const byte N_LED_PER_VU = 7;
byte VuL[N_LED_PER_VU] = { 0, 1, 2, 3, 4, 5, 6 };      // pinos esquerdos do VU
byte VuR[N_LED_PER_VU] = { 7, 8, 9, 10, 11, 12, 13 };  // pinos direitos do VU

byte VU_MIDI_CH = 10;  // canal para ouvir o VU -1

byte vuLcc = 12;  // CC esquerdo
byte vuRcc = 13;  // CC direito

#endif  //USING_VU


#endif  // USING_74HC595


/////////////////////////////////////////////
// FADERS MOTORIZADOS
#ifdef USING_MOTORIZED_FADERS

// Atribuições de pinos do Arduino
const byte N_M_FADERS = 1;
const byte motorDownPin[N_M_FADERS] = { 4 };   // Controle H-Bridge para fazer o motor descer
const byte motorUpPin[N_M_FADERS] = { 5 };     // Controle H-Bridge para fazer o motor subir
const byte faderSpeedPin[N_M_FADERS] = { 9 };  // pino de velocidade do motor (PWM)

byte faderSpeed[N_M_FADERS] = { 240 };  // 0-255
byte faderSpeedMin = 150;               // 0-255 - 140?
byte faderSpeedMax = 255;               // 0-255 -  250?

const byte motorStopPoint = 18;  // o motor irá parar X valores antes de atingir seu objetivo. Aumente para evitar tremores (será menos preciso).

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// variáveis que você não precisa alterar
int faderPos[N_M_FADERS] = { 0 };   // posição do fader
int faderPPos[N_M_FADERS] = { 0 };  // posição anterior do fader
int faderMax[N_M_FADERS] = { 0 };   // Valor lido na posição máxima do fader (0-1023)
int faderMin[N_M_FADERS] = { 0 };   // Valor lido na posição mínima do fader (0-1023)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Midi
byte pFaderInVal[16][N_M_FADERS] = { 0 };
byte pFaderPBInVal[N_M_FADERS] = { 0 };  // Pitch bend para Mackie

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Cap Sense
boolean isTouched[N_M_FADERS] = { false };   // O fader está sendo tocado no momento?
boolean pIsTouched[N_M_FADERS] = { false };  // Anterior: O fader está sendo tocado?

long touchLine[N_M_FADERS] = { 0 };

unsigned long capTimeNow[N_M_FADERS] = { 0 };

// Sensor Capacitivo - Pino de Toque
// resistor de 10M entre os pinos 7 & 8, o pino 2 é o pino do sensor, adicione um fio e/ou folha de alumínio se desejar
CapacitiveSensor capSense[N_M_FADERS] = {
  CapacitiveSensor(7, 8)
};  // capSense = CapacitiveSensor(7, 8)

const int touchSendPin[N_M_FADERS] = { 7 };     // Pino de envio para o circuito de detecção de capacitância (Digital 7)
const int touchReceivePin[N_M_FADERS] = { 8 };  // Pino de recepção para o circuito de detecção de capacitância (Digital 8)

byte capSenseSpeed = 15;   // número de amostras
int capSensitivity = 150;  // limiar de toque

int capTimeout = 500;

#endif  // USING_MOTORIZED_FADERS
