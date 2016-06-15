/******************************************************************************
 * Midi mumuDrum - Tinder Smith 2016
 * Listen for MIDI input data.
 * Sends ADC as note to MIDI out.
 * This project aim for build arduino midi drum.

HARDWARE NOTE:
Connect piezo to a resistor(e.g. 1MegOhm) and line it to ADC1,2,3.etc.,
and another to GND.
If possible, connecting Ardunio RX & TX to MIDI socket with opto-isolator.

SOFTWARE NOTE:
On Windows system, you can use "MIDI Yoke" to transform midi to VST audio lib,
and use "Hairless-MIDI" to connect serial to "MIDI Yoke".
It works on Windows 7 well,and it look like :
            Serial->Hairless-MIDI->MIDI Yoke->VST host(e.g. Cubase)
###############################################################################
update:
**Ver** ****Date***** ********Note********* 
V1.00   June15th2016  # start of the project
******************************************************************************/

  int  ADC0,ADC1,ADC2,ADC3,ADC4,ADC5;// 5 channels ADC input
  byte incomingByte;
  byte note;
  int  noteDown   = LOW;
  byte ADCtrigger = 0; // A flag if a ADC trigger
  int  state      = 0; // state machine variable
                       // 0 = command waiting, 1 = note waitin, 2 = velocity waiting
  byte channel    = 1; // MIDI channel = 'channel's value + 1
                       // e.g.there (MIDI channel)=2=(channel+1))

void setup() {
  Serial.begin(31250);    // The MIDI baudrate should be 31250
  //Serial.begin(115200);  // baudrate for debuging
  }

void loop () {
  //checkInput(); // check if any serial input
  checkAnalog();  // Read ADC and transform to MIDI note
  }

void checkAnalog() {
  // only rising edge do a trig
  if( ADC0 > (analogRead(A0) + 10) ) {
    ADC0 = ADC0 / 8;         // the ADC(0~1023) divide 8 get (0~127)
    noteSend(0x90, 44, ADC0);// send a note
    ADCtrigger = 1;          // flag enable
    }
  if( ADC1 > (analogRead(A1) + 10) ) {
    ADC1 = ADC1 / 8;
    noteSend(0x90, 44, ADC1);
    ADCtrigger = 1;
    }
  if( ADC2 > (analogRead(A2) + 10) ) {
    ADC2 = ADC2 / 8;
    noteSend(0x90, 38, ADC2);
    ADCtrigger = 1;
    }
  if( ADC3 > (analogRead(A3) + 10) ) {
    ADC3 = ADC3 / 8;
    noteSend(0x90, 45, ADC3);
    ADCtrigger = 1;
    }
  if( ADC4 > (analogRead(A4) + 10) ) {
    ADC4 = ADC4 / 8;
    noteSend(0x90, 50, ADC4);
    ADCtrigger = 1;
    }
  if( ADC5 > (analogRead(A5) + 10) ) {
    ADC5 = ADC5 / 8;
    noteSend(0x90, 49, ADC5);
    ADCtrigger = 1;
    }
  if(ADCtrigger) {
    ADCtrigger = 0;// flag clear
    delay(10);     // A delay to debouncing
    }
  ADC0 = analogRead(A0);
  ADC1 = analogRead(A1);
  ADC2 = analogRead(A2);
  ADC3 = analogRead(A3);
  ADC4 = analogRead(A4);
  ADC5 = analogRead(A5);
  // below this line should not a code
  }
// plays a MIDI note
void noteSend(byte cmd, byte note, byte velocity) {
  cmd = cmd | channel;    // merge command&channel
  Serial.write(cmd);     // merge result
  Serial.write(note);    // note number
  Serial.write(velocity);// speed
  }
  
void checkInput(){
  if ( Serial.available() ) {
    // read the incoming byte:
    incomingByte = Serial.read();

   switch (state){
      case 0:
    // look for as status-byte, our channel, note on
    // if (incomingByte== (144 | channel)){  // un comment and comment the line below - to read only one channel
      if ((incomingByte & 0xf0) == 0x90){    // this reads all channels
         noteDown = HIGH;
         state=1;
        }
    // look for as status-byte, our channel, note off
    // if (incomingByte== (128 | channel)){   // un comment and comment the line below - to read only one channel 
        if ((incomingByte & 0xf0) == 0x80){  // this reads all channels
         noteDown = LOW;
         state=1;
        }
        
       case 1:
       // get the note to play or stop
       if(incomingByte < 128) {
          note=incomingByte;
          state=2;
       }
       else{
       state = 0;  // reset state machine as this should be a note number
       }
       break;
       
       case 2:
       // get the velocity
       if(incomingByte < 128) {
         doNote(note, incomingByte, noteDown); // do something withh the note on message
       }
         state = 0;  // reset state machine to start            
     }
  }
}

void doNote(byte note, byte velocity, int down){
  // if velocity = 0 on a 'Note ON' command, treat it as a note off
  if ((down == HIGH) && (velocity == 0)){
      down = LOW; 
  }
  // do something with the note message 
  // this just toggles Pin 13 and ignores the note value and velocity
 if(down == LOW) digitalWrite(13, LOW); else  digitalWrite(13, HIGH);
}


