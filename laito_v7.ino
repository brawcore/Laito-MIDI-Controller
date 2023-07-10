//==Лайто v7==//
#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include "IRLremote.h"
CHashIR IRLremote;


//---INPUTS---//
#define pinIR 3
#define sensorIRdistance A2
#define alavanca 18  //A0
#define button1 1
#define button2 0
#define buttonExt 5
#define buttonExt2 10
//#define piezoPin A10
#define pot1 A6       //4
#define pot2 A7       //6
#define pot3 A8       //8
#define pot4 A9       //9

//---OUTPUTS---//
#define ledInterno 7
#define led1 16
#define led2 14
#define led3 15

//---VARIABLES---//
int valueIRdistance;
int mapedValueIRdistance;
int MIDI_CH = 0;                // 0-15
int modeExt = 0;
int modeIR = 0;
long randNumber;
bool statealavanca;
bool previousstatealavanca;
int previousNote;
bool statebutton1;
bool statebutton2;
bool statebuttonExt;
bool statebuttonExt2;
unsigned long previousMillis = 0;
const unsigned long interval = 500; // Tempo em milissegundos

// Button debounce variables
bool lastStateButton1 = HIGH;
bool lastStateButton2 = HIGH;
bool lastStateButtonExt = HIGH;
bool lastStateButtonExt2 = HIGH;
unsigned long debounceDelay = 50;
unsigned long lastDebounceTimeButton1 = 0;
unsigned long lastDebounceTimeButton2 = 0;
unsigned long lastDebounceTimeButtonExt = 0;
unsigned long lastDebounceTimeButtonExt2 = 0;

bool sustainButton1 = false;
bool sustainButton2 = false;
bool sustainButtonExt = false;

void setup()//================================================================================================
{
  //Serial.begin(115200);
  //Serial.println("Startup");
  randomSeed(analogRead(0));
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(sensorIRdistance, INPUT);
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(pot3, INPUT);
  pinMode(pot4, INPUT);
  //pinMode(piezoPin, INPUT);
  pinMode(alavanca, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(buttonExt, INPUT_PULLUP);
  pinMode(buttonExt2, INPUT_PULLUP);
  pinMode(ledInterno, OUTPUT);

  //if (!IRLremote.begin(pinIR));
  IRLremote.begin(pinIR);
}

void readSensorIRdistanceNOTES() //==============================================================================
{
  valueIRdistance = analogRead(sensorIRdistance);
  mapedValueIRdistance = valueIRdistance;
  mapedValueIRdistance = map(mapedValueIRdistance, 100, 850, 0, 6);
  if (mapedValueIRdistance <= 0){mapedValueIRdistance = 0;} if(mapedValueIRdistance >= 6){mapedValueIRdistance = 6;}
  //Serial.println(valueIRdistance);                                    // T E S T E
  int note = -1; // Nota atual tocada

  switch (mapedValueIRdistance)
  {
    case 0:
      note = 81; // A4
      break;
    case 1:
      note = 79; // G4
      break;
    case 2:
      note = 77; // F4
      break;
    case 3:
      note = 76; // E4
      break;
    case 4:
      note = 74; // D4
      break;
    case 5:
      note = 72; // C4
      break;
    case 6:
      note = 69; // A3
      break;
  }

  

 if (statealavanca == 1)
  {
    digitalWrite(led2, HIGH);
    if(previousstatealavanca==0){noteOn(1, note, 127); MidiUSB.flush();}
    previousstatealavanca=1;
    if (note != previousNote)
  {
    if (previousNote != -1)
    {
      noteOn(1, previousNote, 0); // Envie noteOff para a nota anterior com velocity 0
      MidiUSB.flush();
    }    
    if (note != -1)
    {
      noteOn(1, note, 127); // Envie noteOn para a nova nota com velocity 127
      MidiUSB.flush();
    }
    previousNote = note;
  }
 }
 else
 {
    digitalWrite(led2, LOW);
    if(previousstatealavanca == 1){
    noteOn(1, previousNote, 0);
    noteOn(1, note, 0);
    MidiUSB.flush();
    previousstatealavanca = 0;
  }
}
}
void readSensorIRdistanceCC()//===================================================================================
{
  valueIRdistance = analogRead(sensorIRdistance);
  mapedValueIRdistance = valueIRdistance;
  if (mapedValueIRdistance < 60)
  {
    mapedValueIRdistance = map(mapedValueIRdistance, 0, 59, 0, 200); // para corrigir log dos valores mais baixos
  }
  else
  {
    mapedValueIRdistance = map(mapedValueIRdistance, 60, 1023, 200, 1023);
  }

  mapedValueIRdistance = map(mapedValueIRdistance, 140, 940, 127, 0); // definir aqui o range do IR
  if (mapedValueIRdistance <= 0)
  {
    mapedValueIRdistance = 0;
  }
  if (mapedValueIRdistance >= 127)
  {
    mapedValueIRdistance = 127;
  }
  //Serial.println(mapedValueIRdistance);  // T E S T E
  
  if (statealavanca == 1)
  {
    digitalWrite(led2, HIGH);
    controlChange(MIDI_CH, 10, mapedValueIRdistance); // channel, CC, value
    MidiUSB.flush();
    delay(50);
  }
  else
  {
    digitalWrite(led2, LOW);
  }
}


void readIRremote(){ //=======================================================================================
   if (IRLremote.available())
  {
    digitalWrite(ledInterno, HIGH);
    previousMillis = millis(); // Armazena o tempo atual
    auto data = IRLremote.read();

    if((data.command)==(0xC517CDAD)){      // botãoSet
     modeExt = 0;
    }
    if((data.command)==(0xC20308AD)){      // botãoTvIn
     modeExt = 1;
    }
    if((data.command)==(0x67491AAD)){      // botãoVol-
     modeExt = 2;
    }
    if((data.command)==(0x643455AD)){      // botãoVol+
     modeExt = 3;
    }
    if((data.command)==(0x134F11AD)){      // botãoVermelho
     modeIR = 0;
    }
   if((data.command)==(0xFEDCFAD)){        // botãoVerde
     modeIR = 1;
    }
   if((data.command)==(0x67F48AD)){        // botãoAmarelo
     modeIR = 2;
    }
    if((data.command)==(0x5AC3A6AD)){      // botãoAzul
     modeIR = 3;
    }  
  } //end of ir remote actions
  if (millis() - previousMillis >= interval)
  {
    digitalWrite(ledInterno, LOW); // Desliga o LED
  }
}



void readButtonsNOTE() //====================================================================================
{
  // Read the state of button1 with debounce
  int readingButton1 = digitalRead(button1);
  if (readingButton1 != lastStateButton1)
  {
    lastDebounceTimeButton1 = millis();
  }
  if ((millis() - lastDebounceTimeButton1) > debounceDelay)
  {
    if (readingButton1 != statebutton1)
    {
      statebutton1 = readingButton1;
      if (statebutton1 == LOW)
      {
        sustainButton1 = true; // Habilitar sustentação ao pressionar o botão
        noteOn(1, 0, 127);
        MidiUSB.flush();
      }
      else
      {
        sustainButton1 = false; // Desabilitar sustentação ao soltar o botão
        noteOn(1, 0, 0); // Enviar noteOff
        MidiUSB.flush();
      }
    }
  }
  lastStateButton1 = readingButton1;

  // Read the state of button2 with debounce
  int readingButton2 = digitalRead(button2);
  if (readingButton2 != lastStateButton2)
  {
    lastDebounceTimeButton2 = millis();
  }
  if ((millis() - lastDebounceTimeButton2) > debounceDelay)
  {
    if (readingButton2 != statebutton2)
    {
      statebutton2 = readingButton2;
      if (statebutton2 == LOW)
      {
        sustainButton2 = true; // Habilitar sustentação ao pressionar o botão
        noteOn(1, 1, 127);
        MidiUSB.flush();
      }
      else
      {
        sustainButton2 = false; // Desabilitar sustentação ao soltar o botão
        noteOn(1, 1, 0); // Enviar noteOff
        MidiUSB.flush();
      }
    }
  }
  lastStateButton2 = readingButton2;

}

void readButtonsExtNOTE(){
  
  // Read the state of buttonExt with debounce
  int readingButtonExt = digitalRead(buttonExt);
  if (readingButtonExt != lastStateButtonExt)
  {
    lastDebounceTimeButtonExt = millis();
  }
  if ((millis() - lastDebounceTimeButtonExt) > debounceDelay)
  {
    if (readingButtonExt != statebuttonExt)
    {
      statebuttonExt = readingButtonExt;
      if (statebuttonExt == LOW)
      {
        sustainButtonExt = true; // Habilitar sustentação ao pressionar o botão
        noteOn(1, 3, 127);
        MidiUSB.flush();// Envie a nota correspondente ao botãoExt
        digitalWrite(led1, HIGH);
      }
      else
      {
        sustainButtonExt = false; // Desabilitar sustentação ao soltar o botão
        noteOn(1, 3, 0);
        MidiUSB.flush();// Envie o evento noteOff correspondente ao botãoExt
        digitalWrite(led1, LOW);
      }
    }
  }
  lastStateButtonExt = readingButtonExt;

  statebuttonExt2 = !digitalRead(buttonExt2);

}

void readButtonsExtRANDOM(){
  randNumber = random(72, 79); //72 =C4 . dó central

  // Read the state of buttonExt with debounce
  int readingButtonExt = digitalRead(buttonExt);
  if (readingButtonExt != lastStateButtonExt)
  {
    lastDebounceTimeButtonExt = millis();
  }
  if ((millis() - lastDebounceTimeButtonExt) > debounceDelay)
  {
    if (readingButtonExt != statebuttonExt)
    {
      statebuttonExt = readingButtonExt;
      if (statebuttonExt == LOW)
      {
        sustainButtonExt = true; // Habilitar sustentação ao pressionar o botão
        noteOn(1, randNumber, 127);
        MidiUSB.flush();// Envie a nota correspondente ao botãoExt
        digitalWrite(led1, HIGH);
      }
      else
      {
        sustainButtonExt = false; // Desabilitar sustentação ao soltar o botão
        noteOn(1, randNumber, 0);
        MidiUSB.flush();// Envie o evento noteOff correspondente ao botãoExt
        digitalWrite(led1, LOW);
      }
    }
  }
  lastStateButtonExt = readingButtonExt;

}

void readButtonsCC() //====================================================================================
{
  // Read the state of button1 with debounce
  int readingButton1 = digitalRead(button1);
  if (readingButton1 != lastStateButton1)
  {
    lastDebounceTimeButton1 = millis();
  }
  if ((millis() - lastDebounceTimeButton1) > debounceDelay)
  {
    if (readingButton1 != statebutton1)
    {
      statebutton1 = readingButton1;
      if (statebutton1 == LOW)
      {
        sustainButton1 = true; // Habilitar sustentação ao pressionar o botão
        controlChange(1, 6, 127);
        MidiUSB.flush();
      }
      else
      {
        sustainButton1 = false; // Desabilitar sustentação ao soltar o botão
        controlChange(1, 6, 0); // Enviar CC com valor 0
        MidiUSB.flush();
      }
    }
  }
  lastStateButton1 = readingButton1;

  // Read the state of button2 with debounce
  int readingButton2 = digitalRead(button2);
  if (readingButton2 != lastStateButton2)
  {
    lastDebounceTimeButton2 = millis();
  }
  if ((millis() - lastDebounceTimeButton2) > debounceDelay)
  {
    if (readingButton2 != statebutton2)
    {
      statebutton2 = readingButton2;
      if (statebutton2 == LOW)
      {
        sustainButton2 = true; // Habilitar sustentação ao pressionar o botão
        controlChange(1, 7, 127);
        MidiUSB.flush();
      }
      else
      {
        sustainButton2 = false; // Desabilitar sustentação ao soltar o botão
        controlChange(1, 7, 0); // Enviar CC com valor 0
        MidiUSB.flush();
      }
    }
  }
  lastStateButton2 = readingButton2;
}



void readButtonsExtCC (){ //=============================================================
  
  // Read the state of buttonExt with debounce
  int readingButtonExt = digitalRead(buttonExt);
  if (readingButtonExt != lastStateButtonExt)
  {
    lastDebounceTimeButtonExt = millis();
  }
  if ((millis() - lastDebounceTimeButtonExt) > debounceDelay)
  {
    if (readingButtonExt != statebuttonExt)
    {
      statebuttonExt = readingButtonExt;
      if (statebuttonExt == LOW)
      {
        sustainButtonExt = true; // Habilitar sustentação ao pressionar o botão
        controlChange(1, 8, 127);
        MidiUSB.flush();
        digitalWrite(led1, HIGH);
      }
      else
      {
        sustainButtonExt = false; // Desabilitar sustentação ao soltar o botão
        controlChange(1, 8, 0); // Enviar CC com valor 0
        MidiUSB.flush();
        digitalWrite(led1, LOW);
      }
    }
  }
  lastStateButtonExt = readingButtonExt;

  statebuttonExt2 = !digitalRead(buttonExt2);
}

void potentiometers() //=========================================================================================================
{
  const int N_POTS = 4; // Number of pots
  int potPin[N_POTS] = {pot1, pot2, pot3, pot4};
  int CC[N_POTS] = {20, 21, 22, 23}; // Control Change numbers for pots
  int varThreshold = 5; // Threshold for potentiometer variation

  static int potState[N_POTS] = {0}; // Current potentiometer state
  static int potPState[N_POTS] = {0}; // Previous potentiometer state
  static int midiState[N_POTS] = {0}; // MIDI state corresponding to potentiometer value
  static int midiPState[N_POTS] = {0}; // Previous MIDI state

  static unsigned long lastPot[N_POTS] = {0}; // Time when potentiometer value changed
  static unsigned long potTimer[N_POTS] = {0}; // Timer for potentiometer timeout
  const unsigned long potTimeout = 200; // Timeout for potentiometer changes (in milliseconds)

  for (int i = 0; i < N_POTS; i++)
  {
    potState[i] = analogRead(potPin[i]);  // Read the potentiometer state (0-1023)
    midiState[i] = map(potState[i], 0, 1020, 0, 127);

    int potVar = abs(potState[i] - potPState[i]);

    if (potVar > varThreshold)
    {
      lastPot[i] = millis();  // Save the current time
    }

    potTimer[i] = millis() - lastPot[i];  // Run the timer

    if (potTimer[i] < potTimeout)
    {
      if (midiState[i] != midiPState[i])  // If the current state is different from the previous state
      {
        // Send CC
        controlChange(MIDI_CH, CC[i], midiState[i]);
        MidiUSB.flush();
/*
        Serial.print("Pot ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(midiState[i]);
//*/
        midiPState[i] = midiState[i];
        potPState[i] = potState[i];
      }
    }
  }
}

void loop() //================================================================================================================
{
  statealavanca = digitalRead(alavanca);
  potentiometers();
  readIRremote();
  readButtonsNOTE();

switch (modeExt) {
  case 1:
    readButtonsExtCC();
    break;
  case 2:
    readButtonsExtRANDOM();
    break;
  default:             //'mode' inicia em 0
    readButtonsExtNOTE();
    break;
}

switch (modeIR) {
  case 1:
    readSensorIRdistanceNOTES();
    break;
  default:             //'mode' inicia em 0
    readSensorIRdistanceCC();
    break;
}


}

void noteOn(byte channel, byte pitch, byte velocity)  //==============================================================================================
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void controlChange(byte channel, byte control, byte value)  //=========================================================================================
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}



