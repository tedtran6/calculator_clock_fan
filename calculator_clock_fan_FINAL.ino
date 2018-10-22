//-------------------------------------------------------------CALC---------------------------------------------
#include "CBL2.h"
#include "TIVar.h"

CBL2 cbl;
const int lineRed = 10; //DEFAULT_TIP;
const int lineWhite = 11; //DEFAULT_RING;

const int on = 0x80;    //midi values for on and off
const int off = 0x90;

//don't instantiate the notes now because we want to make the notes interchangable 
int note1, note2, note3, note4, note5, note6, note7, note8, note9, 
    note10, note11, note12, note13, note14, note15, note16;

const int notePlay = 22;    //these are pitches that I don't use when playing
const int notePause = 21;   //so I map them to play pause, record, etc...
const int globalRecord = 24;
const int sessionRecord = 23;

const int metronome = 1;
const int undecided2 = 2;
const int masterPlayButton = 3;
const int undecided3 = 4;
//15 is reserved for our exit button

const int arm1 = 6; //I need to change everything from here to below if I start getting the real values to work
const int arm2 = 7;
const int arm3 = 8;
const int arm4 = 9;
const int arm5 = 10;

const int enable1 = 11;
const int enable2 = 12;
const int enable3 = 13;
const int enable4 = 14;
const int enable5 = 15;

//const int = 16;
//const int = 17;
//const int = 18;
//const int = 19;
//const int = 20;

int masterChannel = 1;

int channel = 1;
int previousChannel = 0;

const int recordChannel1 = 1;
const int recordChannel2 = 2;
const int recordChannel3 = 3;
const int recordChannel4 = 4;
const int recordChannel5 = 5;

// Lists are 2 + (9 * dimension) bytes,
// so incidentally a 255-byte max data length
// limits this demo's lists to 28 elements.
#define MAXDATALEN 255

uint8_t header[16];
uint8_t data[MAXDATALEN];

//-------------------------------------------------------------FAN-----------------------------------------------
int NbTopsFan;      //Varibles used for RPM calculations
int CalculateRMP;           //the rmp calculated
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

const long intervals = 62.5;
long currentTime;
long previousTime = 0;

boolean enabled = false;

int fanVal;
int lastFanVal = 0;
//-------------------------------------------------------------CLOCK---------------------------------------------
int minutePin = 0;
int secondPin = 1;

int minuteVal = 0;
int secondVal = 0;
int previousMinuteVal = 0;
int previousSecondVal = 0;

int sample1, sample2, sample3, sample4, sample5, sample6;

//----------------------------------------------------------------------------

// Run once when the Arduino resets. This first sets
// the LED pins to OUTPUT, then initializes a new CBL2
// instance. This is responsible for dealing with the
// actual calculator<->Arduino message exchange.
void setup() {

  Serial.begin(115200);
  //CLOCK
  pinMode(minutePin, INPUT);
  pinMode(secondPin, INPUT);
  //FAN
  pinMode(hallsensor, INPUT);
  attachInterrupt(0, rpm, RISING);          //the interrupt is attached
  //CALC
  //cbl = new CBL2(lineRed, lineWhite);
  //cbl->resetLines(lineRed, lineWhite);
  cbl.setLines(lineRed, lineWhite);
  cbl.resetLines();
  // cbl->setVerbosity(true, &Serial);      // Comment this in for verbose message information
  
  // The following registers buffers for exchanging data, the maximum
  // allowed data length, and functions to call on Get() and Send().
  //cbl->setupCallbacks(header, data, MAXDATALEN, onGetAsCBL2, onSendAsCBL2);
  cbl.setupCallbacks(header, data, MAXDATALEN, onGetAsCBL2, onSendAsCBL2);
}

// Repeatedly check to see if the calculator has initiated a Send() operation yet. 
//If it has, then onGetAsCBL2() will be invoked, since it was registered in
// setup() above. onSendAsCBL2() is also registered above but it isn't used at all
// during this program so it is commented below
void loop() {
    //FAN STUFF-------------------------------------------------------------
    currentTime = millis();

      if(enabled == false){
       NbTopsFan = 0;  //Set NbTops to 0 ready for calculations
     //sei();   //Enables interrupts
       enabled = true;
      }

   if(currentTime - previousTime >= intervals){
     previousTime = currentTime;
     //cli();   //Disable interrupts
     enabled = false;
     CalculateRMP = ((NbTopsFan * 960)/fanspace[fan].fandiv);
     if(CalculateRMP > 1440)
      CalculateRMP = 1440;
      
     fanVal = map(CalculateRMP, 0, 1440, 0, 123);
     if(fanVal > lastFanVal + 3 || fanVal < lastFanVal - 3){
      
      //Serial.println(CalculateRMP);
      MIDIMessage(176, 1, fanVal);
      lastFanVal = fanVal;
     }
   }

   
   
    //CALCULATOR STUFF------------------------------------------------------
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
  //rval = cbl->eventLoopTick();
  rval = cbl.eventLoopTick();
  if (rval && rval != ERR_READ_TIMEOUT) {                 //if this occurs, this will cause an unexpected value
    Serial.print("Failed to run eventLoopTick: code ");   //to show in Hairless MIDI
    Serial.println(rval);
  }
    //CLOCK---------------------------------------------------------------
    
    //switiching the selections of samples with minute hand-----------
    previousMinuteVal = minuteVal;
    previousSecondVal = secondVal;
    minuteVal = analogRead(minutePin);
    secondVal = analogRead(secondPin);
    
//    Serial.print(secondVal); Serial.print(" ");
//    Serial.println(minuteVal);
    
    if(minuteVal >900 && minuteVal != previousMinuteVal){  
      sample1 = 52;
      sample2 = 53;
      sample3 = 54;
      sample4 = 55;
      sample5 = 56;
      sample6 = 57;
      previousMinuteVal = minuteVal;
    }
    else if(minuteVal >700 && minuteVal != previousMinuteVal){
      sample1 = 58;
      sample2 = 59;
      sample3 = 60;
      sample4 = 61;
      sample5 = 62;
      sample6 = 63;
      previousMinuteVal = minuteVal;
    }
    else if(minuteVal >550 && minuteVal != previousMinuteVal){
      sample1 = 64;
      sample2 = 65;
      sample3 = 66;
      sample4 = 67;
      sample5 = 68;
      sample6 = 69;
      previousMinuteVal = minuteVal;
    }

    //to trigger each sample on the clock------------------------
    if(secondVal >= 7 && previousSecondVal < 7){
      MIDIQuickButton(sample1);
    }
    else if(secondVal >= 170 && previousSecondVal < 170){
      MIDIQuickButton(sample2);
    }
    else if(secondVal >= 314 && previousSecondVal < 314){
      MIDIQuickButton(sample3);
    }
    else if(secondVal >= 444 && previousSecondVal < 444){
      MIDIQuickButton(sample4);
    }
    else if(secondVal >= 585 && previousSecondVal < 585){
      MIDIQuickButton(sample5);
    }
    else if(secondVal >= 726 && previousSecondVal < 726){
      MIDIQuickButton(sample6);
    }
}

// Callback when the CBL2 class has successfully received a variable
// from the attached calculator.
int onGetAsCBL2(uint8_t type, enum Endpoint model, int datalen) {
  
  //Serial.print("Got variable of type "); Serial.print(type); Serial.print(" from endpoint of type "); Serial.println((int)model); 
  //uncomment to debug
  
  uint16_t list_len = TIVar::sizeWordToInt(&(data[0]));   // Convert 2-byte size word to int
  if (list_len == 1) // It is indeed a 1-element list
  {
    
    int value = (int)TIVar::realToLong8x(&data[2], model); // First list element starts after 2-byte size word
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
    else if(value >= 91){
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
    else if(value >40){
      if(value == 41)
        MIDIQuickButton(enable1);
      else if(value == 42)
        MIDIQuickButton(enable2);
      else if(value == 43)
        MIDIQuickButton(enable3);
      else if(value == 44)
        MIDIQuickButton(enable4);
      else if(value == 45)
        MIDIQuickButton(enable5);
//      else if(value == )
//        MIDIQuickButton();
//      else if(value == )
//        MIDIQuickButton();
    }
    else if(value >20){
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
//      else if(value == 26){         //to adjust for the RECORD channels
//        recordChannel++;            //this one doesn't even work anyway
//        if(recordChannel > 6)
//          recordChannel = 1;
//      }
//      else if(value == 24){
//        recordChannel--;
//        if(recordChannel < 1)
//          recordChannel = 6;
//      }

      else if(value == 21)         //to press "play"
        MIDIQuickButton(notePlay);
      else if(value == 31)         //to press "pause"
        MIDIQuickButton(notePause);
      else if(value == 22)
        MIDIQuickButton(globalRecord);
      else if(value == 32)
        MIDIQuickButton(sessionRecord);
        
      else if(value == 23){ //calculator write master play channel ++
        masterChannel--;
        if(masterChannel<1)
          masterChannel = 5;
      }
      else if(value == 33){ //calculator write master play channel --
        masterChannel++;
        if(masterChannel>5)
          masterChannel = 1;
      }
        
    }
      else if(value >= 10){
        if(value == 11)                //these need to be mapped
          MIDIQuickButton(metronome);     //to record each different track
        else if(value == 12)
          MIDIQuickButton(30+masterChannel);
        else if(value == 13){
          MIDIQuickButton(30+masterChannel); //only this one should truly have the master channel but the ti 84 is wank
        }
        else if(value == 14)
          MIDIQuickButton(undecided3);
      }
  }
  else
    Serial.println("Must send a 1-element list!");

  return 0;
}

// this is never used throughout the execution because the demo code on your
// calculator should only ever use Send(). However, it's still left for you
// here to use if you want it. 
int onSendAsCBL2(uint8_t type, enum Endpoint model, int* headerlen, int* datalen, data_callback* data_callback)
{
//  Serial.print("Got request for variable of type ");
//  Serial.print(type);
//  Serial.print(" from endpoint of type ");
//  Serial.println((int)model);
//  return -1;    // -1 means we have no data to send.
}

// Sends MIDI messages to Hairless MIDI
// this turns the note on but then instantly turns the note back off. You
// will need to adjust your decay/sustain/release settings in your DAW to 
// make sure your sample doesn't immediately turn off.
void MIDIQuickButton(int pitch)            
{
   Serial.write(off); Serial.write(pitch); Serial.write(0x60);
              //on value                               //0x60 is a generic velocity. You can adjust this to your preference
   Serial.write(on); Serial.write(pitch); Serial.write(0x60);
              //off value
}

void MIDIMessage(byte command, byte data1, byte data2) //pass values out through standard Midi Command
{
   Serial.write(command);
   Serial.write(data1);
   Serial.write(data2);
}
