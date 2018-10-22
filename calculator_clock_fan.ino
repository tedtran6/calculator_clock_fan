#include "CBL2.h"
#include "TIVar.h"
//-------------------------------------------------------------FAN-----------------------------------------------
int NbTopsFan;      //Varibles used for RPM calculations
int CalculateRPM;           //the RPM calculated
int hallsensor = 2; //The pin location of the RPM sensor??? no it's not
               
typedef struct{                  //Defines the structure for multiple fans and their dividers
  char fantype;
  unsigned int fandiv;
}fanspec;

fanspec fanspace[3]={{0,1},{1,2},{2,8}}; //Definitions of the fans

char fan = 1;   //This is the varible used to select the fan and it's divider, set 1 for unipole hall effect sensor 
               //and 2 for bipole hall effect sensor 

void rpm ()      //This is the function that the interupt calls 
{ 
 NbTopsFan++; 
}

const long intervals = 124;
long currentTime;
long previousTime = 0;

boolean enabled = false;

//-------------------------------------------------------------CLOCK---------------------------------------------
int minutePin = 0;
int secondPin = 1;

int lastVal1 = 0;
int val1 = 0;


//-------------------------------------------------------------CALCULATOR----------------------------------------
//#include "CBL2.h"
//#include "TIVar.h"

CBL2* cbl;
const int lineRed = 10; //DEFAULT_TIP;
const int lineWhite = 11; //DEFAULT_RING;

const int on = 0x80;    //midi values for on and off
const int off = 0x90;

//don't instantiate the notes now because we want to make the notes interchangable 
int note1, note2, note3, note4, note5, note6, note7, note8, note9, 
    note10, note11, note12, note13, note14, note15, note16;

const int notePlay = 20;    //these are pitches that I don't use when playing
const int notePause = 21;   //so I map them to play pause and record

int channel = 1;
int previousChannel = 0;
int recordChannel = 1;

const int noteChannel1 = 1;
const int noteChannel2 = 2;
const int noteChannel3 = 3;
const int noteChannel4 = 4;
const int noteChannel5 = 5;
const int noteChannel6 = 6;

// Lists are 2 + (9 * dimension) bytes,
// so incidentally a 255-byte max data length
// limits this demo's lists to 28 elements.
#define MAXDATALEN 255

uint8_t header[16];
uint8_t data[MAXDATALEN];

//---------------------------------------------------------SETUP-----------------------------------------------
void setup() 
{
 Serial.begin(115200);
 //FAN
 pinMode(hallsensor, INPUT);
 attachInterrupt(0, rpm, RISING);  //and the interrupt is attached
 
 //CLOCK
 pinMode(minutePin, INPUT);
 pinMode(secondPin, INPUT);

 //CALCULATOR
 cbl = new CBL2(lineRed, lineWhite);
  cbl->resetLines();
  // cbl->setVerbosity(true, &Serial);      // Comment this in for verbose message information
  
  // The following registers buffers for exchanging data, the maximum
  // allowed data length, and functions to call on Get() and Send().
  cbl->setupCallbacks(header, data, MAXDATALEN, onGetAsCBL2, onSendAsCBL2);
} 

void loop() 
{
   val1 = analogRead(0)/8;
   if(val1 != lastVal1)
   {
     MIDImessage(176,1, val1);
     lastVal1 = val1;
   }
   
   currentTime = millis();
   
   if(enabled == false){
     NbTopsFan = 0;  //Set NbTops to 0 ready for calculations
     sei();   //Enables interrupts
     enabled = true;
   }

   if(currentTime - previousTime >= intervals){
    //delay(124);
     previousTime = currentTime;
     cli();   //Disable interrupts
     enabled = false;
     CalculateRPM = ((NbTopsFan * 480)/fanspace[fan].fandiv); //Times NbTopsFan (which is apprioxiamately the fequency the fan is spinning at) by 60 seconds before dividing by the fan's divider
     if(CalculateRPM > 3600)
      CalculateRPM = 3600;
      
//     Serial.print(CalculateRPM);
//     Serial.print(" ");
//     Serial.print(analogRead(minutePin));
//     Serial.print(" ");
//     Serial.println(analogRead(secondPin));

        if(channel == 1 && previousChannel !=1){
        note1 = 36; note2 = 37; note3 = 38; note4 = 39;
        note5 = 40; note6 = 41; note7 = 42; note8 = 43;
        note9 = 44; note10 = 45; note11 = 46; note12 = 47;
        note13 = 48; note14 = 49; note15 = 50; note16 = 51;
        previousChannel = 1;
      }
      else if(channel == 2 && previousChannel != 2){
        note1 = 52; note2 = 53; note3 = 54; note4 = 55;
        note5 = 56; note6 = 57; note7 = 58; note8 = 59;
        note9 = 60; note10 = 61; note11 = 62; note12 = 63;
        note13 = 64; note14 = 65; note15 = 66; note16 = 67;
        previousChannel = 2;
      }
      else if(channel == 3 && previousChannel != 3){
        note1 = 68; note2 = 69; note3 = 70; note4 = 71;
        note5 = 72; note6 = 73; note7 = 74; note8 = 75;
        note9 = 76; note10 = 78; note11 = 79; note12 = 80;
        note13 = 81; note14 = 82; note15 = 83; note16 = 84;
        previousChannel = 3;
      }
      
    int rval;
    rval = cbl->eventLoopTick();
    if (rval && rval != ERR_READ_TIMEOUT) {                 //if this occurs, this will cause an unexpected value
      Serial.print("Failed to run eventLoopTick: code ");   //to show in Hairless MIDI
      Serial.println(rval);
    }
   }
}

void MIDImessage(byte command, byte data1, byte data2) //pass values out through standard Midi Command
{
   Serial.write(command);
   Serial.write(data1);
   Serial.write(data2);
}

void MIDIQuickButton(int pitch)            
{
   Serial.write(0x80); Serial.write(pitch); Serial.write(0x60);
              //on value                               //0x60 is a generic velocity. You can adjust this to your preference
   Serial.write(0x90); Serial.write(pitch); Serial.write(0x60);
              //off value
}

int onGetAsCBL2(uint8_t type, enum Endpoint model, int datalen) {
  
  // Serial.print("Got variable of type "); Serial.print(type); Serial.print(" from endpoint of type "); Serial.println((int)model); 
  //uncomment to debug

  
  uint16_t list_len = TIVar::sizeWordToInt(&(data[0]));   // Convert 2-byte size word to int
  if (list_len == 1) // It is indeed a 1-element list
  {
    int value = (int)TIVar::realToFloat8x(&data[2], model); // First list element starts after 2-byte size word
    //Serial.println(value);  //for debugging the values that come through

    if(value >= 102){
        if(value == 102)          //to play every single note
          MIDIQuickButton(note1);
        else if(value == 103)
          MIDIQuickButton(note2);
        else if(value == 104)
          MIDIQuickButton(38);
        else if(value == 105)
          MIDIQuickButton(note4);
    }
    else if(value >= 92){
      if(value == 92)
        MIDIQuickButton(note5);
      else if(value == 93)
        MIDIQuickButton(note6);
      else if(value == 94)
        MIDIQuickButton(note7);
      else if(value == 95)
        MIDIQuickButton(note8);
    }
    else if(value >= 82){
      if(value == 82)
        MIDIQuickButton(note9);
      else if(value == 83)
        MIDIQuickButton(note10);
      else if(value == 84)
        MIDIQuickButton(note11);
      else if(value == 85)
        MIDIQuickButton(note12);
    }
    else if(value >= 72){
      if(value == 72)
        MIDIQuickButton(note13);
      else if(value == 73)
        MIDIQuickButton(note14);
      else if(value == 74)
        MIDIQuickButton(note15);
      else if(value == 75)
        MIDIQuickButton(note16);
    }
    else if(value >10){
      if(value == 25){         //to the ajust for the NOTE channels
        channel++;
        if(channel > 3)
          channel = 1;
      }
      else if(value == 34){
        channel--;
        if(channel < 1)
          channel = 3;
      }
      else if(value == 26){         //to adjust for the RECORD channels
        recordChannel++;
        if(recordChannel > 6)
          recordChannel = 1;
      }
      else if(value == 24){
        recordChannel--;
        if(recordChannel < 1)
          recordChannel = 6;
      }
      else if(value == 21){         //to press "play"
        MIDIQuickButton(notePlay);
      }
      else if(value == 31){         //to press "pause"
        MIDIQuickButton(notePause);
      }
      else if(value == 41){
        if(recordChannel == 1)                //these need to be mapped
           MIDIQuickButton(noteChannel1);     //to record each different track
        else if(recordChannel == 2)
          MIDIQuickButton(noteChannel2);
        else if(recordChannel == 3)
          MIDIQuickButton(noteChannel3);
        else if(recordChannel == 4)
          MIDIQuickButton(noteChannel4);
        else if(recordChannel == 5)
          MIDIQuickButton(noteChannel5);
        else if(recordChannel == 6)
          MIDIQuickButton(noteChannel6);
      }
    }
  }
  else
    Serial.println("Must send a 1-element list!");

  return 0;
}

int onSendAsCBL2(uint8_t type, enum Endpoint model, int* headerlen, int* datalen, data_callback* data_callback)
{
//lol  
}


