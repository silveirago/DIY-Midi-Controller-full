/*
     Por por Gustavo Silveira, 2019.
     - Esta Sketch lê as portas digitais e analógicas do Arduino e envia notas midi e MIDI Control Change
     - Você pode usar múltiplos multiplexers e encoders

     Quer aprender a fazer seus próprios códigos e entender os códigos de outras pessoas?
     Confira nosso curso completo do Fazendo Música com Arduino: http://musiconerd.com/curso-completo-b

     http://www.musiconerd.com
     http://www.youtube.com/musiconerd
     http://facebook.com/musiconerdmusiconerd
     http://instagram.com/musiconerd/
     http://www.gustavosilveira.net
     gustavosilveira@musiconerd.com

     Se você estiver usando este código para qualquer coisa que não seja de uso pessoal, não se esqueça de dar crédito.

     PS: Basta alterar o valor que tem um comentário como "// *"

*/


/////////////////////////////////////////////
// Escolhendo seu placa
// Defina seu placa, escolha:
// "ATMEGA328" se estiver usando o ATmega328 - Uno, Mega, Nano ...
// "ATMEGA32U4" se estiver usando com o ATmega32U4 - Micro, Pro Micro, Leonardo ...
// "TEENSY" se estiver usando uma placa Teensy
// "DEBUG" se você quer apenas debugar o código no monitor serial
// você não precisa comentar ou descomentar qualquer biblioteca MIDI abaixo depois de definir sua placa

#define ATMEGA328 1 //* coloque aqui o uC que você está usando, como nas linhas acima seguidas de "1", como "ATMEGA328 1", "DEBUG 1", etc.

/////////////////////////////////////////////
// BIBLIOTECAS
// -- Define a biblioteca MIDI -- //

// se estiver usando com ATmega328 - Uno, Mega, Nano ...
#ifdef ATMEGA328
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// se estiver usando com ATmega32U4 - Micro, Pro Micro, Leonardo ...
#elif ATMEGA32U4
#include "MIDIUSB.h"

#endif
// ---- //

//////////////////////
// Use se estver usando o multiplexer cd4067
#include <Multiplexer4067.h> // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067

//////////////////////
// Threads
#include <Thread.h> // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h>

//////////////////////
// Encoder
// No gerenciador de downloads baixe a Encoder lib de Paul Stoffregen (ele vem com o Teensy)
#include <Encoder.h>  // faz todo o trabalho para você na leitura do encoder

///////////////////////////////////////////
// MULTIPLEXERS
#define N_MUX 2 //* numero de multiplexers
//* Define s0, s1, s2, s2, e x pins
#define s0 2
#define s1 3
#define s2 4
#define s3 5
#define x1 A1 // pino analogico do primeiro mux
#define x2 A2 // pino analogico do segundo mux
// adicione mais "#define x" se precisar

// Inicializa o multiplexer
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1), //*
  Multiplexer4067(s0, s1, s2, s3, x2) //*
  // ...
};

/////////////////////////////////////////////
// BOTOES
const int N_BUTTONS = 1 + 2 + 2; //*  número total de botões. Número de botões no Arduino + número de botões no multiplexador 1 + número de botões no multiplexador 2 ...
const int N_BUTTONS_ARDUINO = 1; //* número de botões conectados diretamente ao Arduino (em ordem)
const int N_BUTTONS_PER_MUX[N_MUX] = {2, 2}; //* número de botões em cada mux (em ordem)

const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {7}; //* pinos de cada botão conectado diretamente ao Arduino
const int BUTTON_MUX_PIN[N_MUX][16] = { //* pino de cada botão de cada mux em ordem
  {2, 3}, //* pinos do primeiro mux
  {1, 2}  //* pinos do segundo mux
  // ...
};

//#define pin13 1 // descomente se você estiver usando o pino 13 (o pino com led), ou comente a linha se não
byte pin13index = 12; //* coloque o índice do pin 13 do array buttonPin[] se você estiver usando, se não, comente

int buttonCState[N_BUTTONS] = {};        // armazena o valor atual do botão
int buttonPState[N_BUTTONS] = {};        // armazena o valor anterior do botão

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // a última vez que o pino de saída foi alternado
unsigned long debounceDelay = 5;    //* o tempo de debounce; aumentar se a saída estiver mandando muitas notas de uma vez so

/////////////////////////////////////////////
// POTENCIOMETROS
const int N_POTS = 1 + 2 + 1; //* número total de pots (slide e rotativo). Número de pots no Arduino + número de pots no multiplexador 1 + número de pots no multiplexador 2 ...
const int N_POTS_ARDUINO = 1; //* número de pots conectados diretamente ao Arduino
const int N_POTS_PER_MUX[N_MUX] = {2, 1}; //* número de pots em cada multiplexador (em ordem)

const int POT_ARDUINO_PIN[N_POTS_ARDUINO] = {A0}; //* pinos de cada pot conectado diretamente ao Arduino
const int POT_MUX_PIN[N_MUX][16] = { //* pinos de cada pot de cada mux na ordem que você quer que eles sejam lidos
  {0, 1}, // pinos do primeiro mux
  {0} // pinos do segundo mux
  // ...
};

int potCState[N_POTS] = {0}; // estado atual da porta analogica
int potPState[N_POTS] = {0}; // estado previo da porta analogica
int potVar = 0; // variacao entre o valor do estado previo e o atual da porta analogica

int midiCState[N_POTS] = {0}; // Estado atual do valor midi
int midiPState[N_POTS] = {0}; // Estado anterior do valor midi

const int TIMEOUT = 300; //* quantidade de tempo em que o potenciometro sera lido apos ultrapassar o varThreshold
const int varThreshold = 10; //* threshold para a variacao no sinal do potenciometro
boolean potMoving = true; // se o potenciometro esta se movendo
unsigned long PTime[N_POTS] = {0}; // tempo armazenado anteriormente
unsigned long timer[N_POTS] = {0}; // armazena o tempo que passou desde que o timer foi zerado

/////////////////////////////////////////////
// ENCODERS
// Você pode adicionar quantos encoders você quiser separados em quantos canais diferentes quiser
//#define TRAKTOR 1 // descomente se estiver usando traktor, comente se não
const int N_ENCODERS = 2; //* numero de encoders
const int N_ENCODER_CHANNELS = 1; //* numero de canais (bancos)
const int N_ENCODER_PINS = N_ENCODERS * 2; // número de pinos usados pelos encoders

Encoder encoder[N_ENCODERS] = {{9, 8}, {11, 10}}; // os dois pinos de cada codificador (de trás pra frente) - Use pinos com interrupts!

int encoderMinVal = 0; //* valor mínimo do encoder
int encoderMaxVal = 127; //* valor máximo do encoder

int preset[N_ENCODER_CHANNELS][N_ENCODERS] = { // armazena presetd para iniciar seus encoders
  {64, 64}, // preset do primeiro banco
  // ...
};

int lastEncoderValue[N_ENCODER_CHANNELS][N_ENCODERS] = {127};
int encoderValue[N_ENCODER_CHANNELS][N_ENCODERS] = {127};

// para os canais do encoder - Use para bancos diferentes
int encoderChannel = 0;
int lastEncoderChannel = 0;
/////////////////////////////////////////////

byte midiCh = 1; //* Canal MIDI a ser usado
byte note = 24; //* nota mais baixa a ser usada
byte cc = 11; //* O mais baixo MIDI CC a ser usado

/////////////////////////////////////////////
// THREADS
// Essa lib cria um thread "falso". Isso significa que você pode fazer coisas simultaneas acontecerem acontecer a cada x milissegundos
// Podemos usar isso para ler algo em um intervalo, em vez de ler a cada loop
// Neste caso, vamos ler os potenciômetros em um thread, tornando a leitura dos botões mais rápida
ThreadController cpu; // thread master, onde os outros threads serão adicionados
Thread threadPotentiometers; // thread para controlar os pots

void setup() {

  // Baud Rate
  // use se estiver usando with ATmega328 (uno, mega, nano...)
  // 31250 para MIDI class compliant | 115200 para Hairless MIDI
  Serial.begin(115200); //*

#ifdef DEBUG
Serial.println("Debug mode on");
Serial.println();
#endif

  // Buttons
  // Inicializar botões com pull up resistor
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef pin13 // inicializa o pino 13 como uma entrada
pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif

  /////////////////////////////////////////////
  // Multiplexers
  // Inicializa os multiplexadores
  for (int i = 0; i < N_MUX; i++) {
    mux[i].begin();
  }
  // Defina cada pino X como input_pullup (evita comportamento flutuante)
  pinMode(x1, INPUT_PULLUP);
  pinMode(x2, INPUT_PULLUP);

  /////////////////////////////////////////////
  // Threads
  threadPotentiometers.setInterval(10); // cada quantos milissegundos
  threadPotentiometers.onRun(potentiometers); // a função que será adicionada ao encadeamento
  cpu.add(&threadPotentiometers); // adicione todos as threads aqui

  /////////////////////////////////////////////
  // Encoders
  for (int i = 0; i < N_ENCODERS; i++) { // Se você quiser começar com um certo valor use presets
    encoder[i].write(preset[0][i]);
  }

  for (int z = 0; z < N_ENCODER_CHANNELS; z++) {
    for (int i = 0; i < N_ENCODERS; i++) {
      lastEncoderValue[z][i] = preset[z][i];
      encoderValue[z][i] = preset[z][i];
    }
  }
  /////////////////////////////////////////////

}

void loop() {

  cpu.run(); // para as threads
  buttons();
  encoders();
  // potentiometers();

}

/////////////////////////////////////////////
// BOTOES
void buttons() {

  // lê os pinos do arduino
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }

  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // compensa o buttonCState a cada leitura do mux

  // lê os pinos de cada mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
      // Scale values to 0-1
      if (buttonCState[i + nButtonsPerMuxSum] > 500) {
        buttonCState[i + nButtonsPerMuxSum] = HIGH;
      }
      else {
        buttonCState[i + nButtonsPerMuxSum] = LOW;
      }
    }
    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
  }


  for (int i = 0; i < N_BUTTONS; i++) {

#ifdef pin13
if (i == pin13index) {
buttonCState[i] = !buttonCState[i]; // inverte o pino 13 porque tem um resistor pull down em vez de um pull up
}
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {

          // Envia a nota MIDI de acordo com a placa escolhida
#ifdef ATMEGA328
// ATmega328 (uno, mega, nano...)
MIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel

#elif ATMEGA32U4
// ATmega32U4 (micro, pro micro, leonardo...)
noteOn(midiCh, note + i, 127);  // channel, note, velocity
MidiUSB.flush();

#elif TEENSY
// Teensy
usbMIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel

#elif DEBUG
Serial.print(i);
Serial.println(": button on");
#endif

        }
        else {

          // Envia a nota MIDI OFF de acordo com a placa escolhida
#ifdef ATMEGA328
// ATmega328 (uno, mega, nano...)
MIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel

#elif ATMEGA32U4
// ATmega32U4 (micro, pro micro, leonardo...)
noteOn(midiCh, note + i, 0);  // channel, note, velocity
MidiUSB.flush();

#elif TEENSY
// Teensy
usbMIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel

#elif DEBUG
Serial.print(i);
Serial.println(": button off");
#endif

        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

/////////////////////////////////////////////
// POTENTIOMETERS
void potentiometers() {

  /* para que seja feita apenas a leitura das portas analogicas quando elas sao utilizadas, sem perder resolucao,
    ´e  preciso estabelecer um "threshold" (varThreshold),  um valor minimo que as portas tenham que ser movimentadas
    para que se comece a leitura. Apos isso cria-se uma especie de "portao", um portao que se abre e permite
    que as porta analogicas sejam lidas sem interrupcao por determinado tempo (TIMEOUT). Quando o timer ´e menor que TIMEOUT
    significa que o potenciometro foi mexido ha muito pouco tempo, o que significa que ele provavelmente ainda esta se movendo,
    logo deve-se manter o "portao" aberto; caso o timer seja maior que TIMEOUT siginifica que ja faz um tempo que ele nao ´e movimentado,
    logo o portao deve ser fechado. Para que essa logica aconteca deve-se zerar o timer (linhas 99 e 100) a cada vez que a porta analogica
    variar mais que o varThreshold estabelecido.
  */

  // lê os pinos do arduino
  for (int i = 0; i < N_POTS_ARDUINO; i++) {
    potCState[i] = analogRead(POT_ARDUINO_PIN[i]);
  }

  int nPotsPerMuxSum = N_POTS_ARDUINO; // compensa o buttonCState a cada leitura do mux

  // leia os pinos de cada mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_POTS_PER_MUX[j]; i++) {
      potCState[i + nPotsPerMuxSum] = mux[j].readChannel(POT_MUX_PIN[j][i]);
    }
    nPotsPerMuxSum += N_POTS_PER_MUX[j];
  }

  //Debug somente
  //    for (int i = 0; i < nPots; i++) {
  //      Serial.print(potCState[i]); Serial.print(" ");
  //    }
  //    Serial.println();

  for (int i = 0; i < N_POTS; i++) { // Faz o loop de todos os potenciômetros

    midiCState[i] = map(potCState[i], 0, 1023, 0, 127); // Mapeia a leitura do potCState para um valor utilizável em midi

    potVar = abs(potCState[i] - potPState[i]); // Calcula o valor absoluto entre a diferença entre o estado atual e o anterior do pot

    if (potVar > varThreshold) { // Abre o portão se a variação do potenciômetro for maior que o limite (varThreshold)
      PTime[i] = millis(); // Armazena o tempo anterior
    }

    timer[i] = millis() - PTime[i]; // Reseta o timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) { // Se o timer for menor que o tempo máximo permitido, significa que o potenciômetro ainda está se movendo
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) { // Se o potenciômetro ainda estiver em movimento, envie control change
      if (midiPState[i] != midiCState[i]) {

        // Envia o MIDI CC de acordo com a placa escolhida
#ifdef ATMEGA328
// ATmega328 (uno, mega, nano...)
MIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc value, midi channel

#elif ATMEGA32U4
// ATmega32U4 (micro, pro micro, leonardo...)
controlChange(midiCh, cc + i, midiCState[i]); //  (channel, CC number,  CC value)
MidiUSB.flush();

#elif TEENSY
// Teensy
usbMIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc value, midi channel

#elif DEBUG
Serial.print("Pot: ");
Serial.print(i);
Serial.print(" ");
Serial.println(midiCState[i]);
//Serial.print("  ");
#endif

        potPState[i] = potCState[i]; // Armazena a leitura atual do potenciômetro para comparar com a próxima
        midiPState[i] = midiCState[i];
      }
    }
  }
}

/////////////////////////////////////////////
// ENCODERS
void encoders() {

  for (int i = 0; i < N_ENCODERS; i++) {
    encoderValue[encoderChannel][i] = encoder[i].read(); // lê cada encoder e armazena o valor
  }

  for (int i = 0; i < N_ENCODERS; i++) {

    if (encoderValue[encoderChannel][i] != lastEncoderValue[encoderChannel][i]) {

#ifdef TRAKTOR // para usar com o Traktor
if (encoderValue[encoderChannel][i] > lastEncoderValue[encoderChannel][i]) {
encoderValue[encoderChannel][i] = 127;
} else {
encoderValue[encoderChannel][i] = 0;
}
#endif

      clipEncoderValue(i, encoderMinVal, encoderMaxVal); // verifica se é maior que o valor máximo ou menor que o valor mínimo

      // Envia o MIDI CC de acordo com o quadro escolhido
#ifdef ATMEGA328
// se usando com ATmega328 (uno, mega, nano ...)
MIDI.sendControlChange(i, encoderValue[encoderChannel][i], encoderChannel);

#elif ATMEGA32U4
// se usando com ATmega32U4 (micro, pro micro, leonardo...)
controlChange(i, encoderValue[encoderChannel][i], encoderChannel);
MidiUSB.flush();

#elif TEENSY
// se usando com o Teensy
usbMIDI.sendControlChange(i, encoderValue[encoderChannel][i], encoderChannel);

#elif DEBUG
Serial.print("encoder channel: "); Serial.print(encoderChannel); Serial.print("  ");
Serial.print("Encoder "); Serial.print(i); Serial.print(": ");
Serial.println(encoderValue[encoderChannel][i]);
#endif

      lastEncoderValue[encoderChannel][i] = encoderValue[encoderChannel][i];
    }
  }
}


////////////////////////////////////////////
// verifica se é maior que o valor máximo ou menor que o valor mínimo
void clipEncoderValue(int i, int minVal, int maxVal) {

  if (encoderValue[encoderChannel][i] > maxVal - 1) {
    encoderValue[encoderChannel][i] = maxVal;
    encoder[i].write(maxVal);
  }
  if (encoderValue[encoderChannel][i] < minVal + 1) {
    encoderValue[encoderChannel][i] = minVal;
    encoder[i].write(minVal);
  }
}

/////////////////////////////////////////////
// se estiver usando com ATmega32U4 (micro, pro micro, leonardo ...)
#ifdef ATMEGA32U4

// Arduino (pro)micro midi functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

#endif


