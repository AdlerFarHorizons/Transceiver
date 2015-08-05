#include <AltSoftSerial.h>
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
String msgIn = "";



void setup(){
  Serial.begin(9600);
  altSerial.begin( 9600 ); 
  sentencePending = false;
  sentenceRdy = false;
  sentenceBuf[0] = 0;
  sentenceBufIndex = 0;
}
void loop(){
  char lastChar;
  while(altSerial.available()){
    lastChar = altSerial.read();
    if(lastChar == 10 && msgIn != ""){ //end of RX string
      if(msgIn.substring(msgIn.indexOf(' ')) == " ok\r"){ //if this is just the acknowledgement message
        //we're done, payload rcvd our msg
        msgIn = "";
      }else{
        sendMyGPS(); 
        //filters out the counter number at the end of rx string and send it
        altSerial.println(" "+msgIn.substring(1 + msgIn.indexOf(' ')));
        Serial.println(" "+msgIn.substring(1 + msgIn.indexOf(' ')));//for logging
        msgIn = "";
      }
    }else{
      msgIn += lastChar;
      getCharGPS();
    }
  }
}

void getCharGPS() {
  
  char c;
  if ( Serial.available() ) {
    c = Serial.read();
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
}
void sendMyGPS() {
  //print out sentenceBuf, all of it - KN
  int i = 0;
  while( sentenceBuf[i+1] != 0 ) {
      altSerial.write( sentenceBuf[i] );
      Serial.write(sentenceBuf[i]);//for logging
      sentenceRdy = false;
      i++;
  }
}

void printAndSend(String whatToSend){ //sends message to both logger and rf module
  altSerial.print(whatToSend);
  Serial.print(whatToSend);
}
