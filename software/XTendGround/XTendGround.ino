
#include <AltSoftSerial.h>

//those vars from other file
AltSoftSerial altSerial; // TX Pin 9, RX Pin 8
char pkt_buffer[200];
boolean sentencePending, sentenceRdy;
boolean txFlag;
char sentenceBuf[100];
String gpsTime, gpsDate, gpsLat, gpsLon, gpsAlt, gspVgnd, gpsHdng;
int sentenceBufIndex = 0;
//end those vars

boolean rxDone = false;
boolean rxStart = false;
String thisGPS;
char msgIn[100];
char c = '*';

void setup(){
  Serial.begin(9600);
  altSerial.begin( 4800 ); 
  sentencePending = false;
  sentenceRdy = false;
  sentenceBuf[0] = 0;
  sentenceBufIndex = 0;
}

void loop(){
  while (Serial.available()){
    c = Serial.read();
    if (c=='$'){                     //check for start
      for(int i = 0; i < 100; i++){
        if(c == 10){                 //check for end
          msgIn[i+1]= '\0';
          rxDone = true;
          break;
        }
        msgIn[i]=c;                  //otherwise, add on to msg
        
        /* README --KN
          I've discovered that there is a delay when reading the serial:
          It should be $GPRMS <some other stuff here>
          but it turns into $  <...some time passes...> P <...some more time passes...> RMS <and the other stuff comes really fast through the serial read>
        
         So the line underneath this comment was causing issues: the Y with a ^ characters was caused by a lack of stuff to read (Serial.read returns -1 if there is nothing to read)
         Need to get around this with Serial.available and some more waiting loops.
         
         This was working a while back, with the messages having extra YYYYYYY's in it, but that version was destroyed as I tried to fix it.
         Most of the code here is logical. Need to figure out why nothing is printing out now.
        */
        
        
        c = Serial.read();           //take next byte
      }//end for
    }//done with receive
  }  
    //printing what's in msgIn
    if(rxDone == true){              //only if we get a new message
      Serial.print("Received: ");
      
      /*
      Also, serial.print(msgIn) vs Serial.write(msgIn), vs. the loop below?
      I think they all work.
      */
      int j = 0;
      while (msgIn[j] != 0){
        Serial.write(msgIn[j]);
        j++;
      }
      
      //make our response
      thisGPS = getCharGPS();
      if(rxDone){
        Serial.print(thisGPS);
      }
      rxDone == false;
    }
}



String getCharGPS() { 
  char c;
  if ( altSerial.available() ) {
    c = altSerial.read();
    if ( !sentenceRdy ) {
      if ( c == 36 ) { //36 is '$', start of GPS statement - KN
        sentenceBufIndex = 0;
        sentencePending = true;
      }
      if ( sentencePending && c == 10 ) { //10 is new line - KN
        sentenceRdy = true;
        sentencePending = false;
      }
      sentenceBuf[sentenceBufIndex] = c;
      sentenceBufIndex++;
      sentenceBuf[sentenceBufIndex] = 0;
    }
  }
  return (String) sentenceBuf;
}
