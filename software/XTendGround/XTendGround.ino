#define GPSLEN 100 //max length of GPS output to be sent

#include <AltSoftSerial.h>
AltSoftSerial altSerial; // TX Pin 9, RX Pin 8
int ledPin = 13;
int tLed = 200;
char pkt_buffer[200];
boolean sentencePending, sentenceRdy;
boolean txFlag;
char sentenceBuf[GPSLEN];
char sentenceBuf2[GPSLEN];
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
  pinMode( ledPin, OUTPUT );
}
void loop(){
  // Copy any completed sentence to 2nd buffer
  if ( sentenceRdy ) {
    for ( int i = 0 ; i < GPSLEN ; i++ ) {
        sentenceBuf2[i] = sentenceBuf[i];
    }
    sentenceRdy = false;
  }
  
  char lastChar;
  while(altSerial.available()){
    lastChar = altSerial.read();
    if(lastChar == 10 && msgIn != ""){ //end of RX string
//  Commented out so ground station always replies to received packet
//      if(msgIn.substring(msgIn.indexOf(' ')) == " ok\r"){ //if this is just the acknowledgement message
//        //we're done, payload rcvd our msg
//        msgIn = "";
//      }else{
      Serial.print( "TX:" );
      //sendMyGPS(); 
      altSerial.println(" "+msgIn.substring(1 + msgIn.indexOf(' ')));
      //filters out the counter number at the end of rx string and send it
      Serial.println(" "+msgIn.substring(1 + msgIn.indexOf(' ')));//for logging
      Serial.println( "RX:" + msgIn );
      msgIn = "";
//      if ( sentenceRdy ) {
//        int i = 0;
//        while( sentenceBuf[i+1] != 0 ) {
//            //altSerial.write( sentenceBuf[i] );
//            Serial.write(sentenceBuf[i]);//for logging
//            //sentenceRdy = false;
//            i++;
//        }
//        Serial.print( 10 );
//      }
//      }
      sendMyGPS();
      flashLed( ledPin, tLed, 3 );
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
        sentenceRdy = false;
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
  if ( sentenceRdy ) {
    while( sentenceBuf[i+1] != 0 ) {
        //altSerial.write( sentenceBuf[i] );
        Serial.write( sentenceBuf[i] );//for logging
        i++;
    }
    sentenceRdy = false;
  } else { // Latest data not ready yet, use 2nd buffer
    Serial.print( "BUF" );
    while( sentenceBuf2[i+1] != 0 ) {
        //altSerial.write( sentenceBuf[i] );
        Serial.write( sentenceBuf2[i] );//for logging
        i++;
    }
  }
  Serial.write( 10 );
}

void printAndSend(String whatToSend){ //sends message to both logger and rf module
  altSerial.print(whatToSend);
  Serial.print(whatToSend);
}

void flashLed( int ledPin, int t, int num ) {
  for ( int i = 0 ; i < num ; i++ ) {
    digitalWrite( ledPin, HIGH );
    delay( t );
    digitalWrite( ledPin, LOW );
    delay( t );
  }
}


