/*
 * 
 * 
 * GPS serial input on pin 8 (for AltSoftSerial) delivers RMC
 * sentence only every second at 4800 baud.
 * 
 * Payload (controller) transmits packet on MsTimer2 interrupt
 * and establishes time slot 0 for all Ground units (peripherals)
 */
#define GPSLEN 100 //max length of GPS output to be sent

#include <AltSoftSerial.h>
#include <MsTimer2.h>

// LN - Adding stuff for a GPS connected to soft serial port
AltSoftSerial altSerial; // TX Pin 9, RX Pin 8
char pkt_buffer[200];
boolean sentencePending, sentenceRdy;
boolean txFlag, help;
char sentenceBuf[GPSLEN];
char sentenceBuf2[GPSLEN];
String gpsTime, gpsDate, gpsLat, gpsLon, gpsAlt, gspVgnd, gpsHdng;
int sentenceBufIndex = 0;
String msgIn = "";

String output = "hello";
int numb = 0;

//const int pin2 = 2;

void setup(){
  Serial.begin(9600);
  
  altSerial.begin( 9600 ); 
  sentencePending = false;
  sentenceRdy = false;
  sentenceBuf[0] = 0;
  sentenceBufIndex = 0;
  MsTimer2::set(10000, transmit);
  MsTimer2::start();
  pinMode(5,INPUT_PULLUP);
  help = false;
}
void loop(){
  //for testing
  if(!digitalRead(5) && !help){
    printAndSend("321654 ok");
    help = 1;
  }

  // Copy any completed sentence to 2nd buffer
  if ( sentenceRdy ) {
    for ( int i = 0 ; i < GPSLEN ; i++ ) {
        sentenceBuf2[i] = sentenceBuf[i];
    }
    sentenceRdy = false;
  }
  //RX section
  char lastChar;
  while(altSerial.available()){
    lastChar = altSerial.read();
    if(lastChar == 10){ //if I get a new line
      Serial.println( msgIn );
      int rcvdNum = (int)msgIn.substring(1 + msgIn.indexOf(' ')).toInt();
      boolean ok = false;
      if (rcvdNum == numb-1){
        ok = true;
      }
      //altSerial.print(msgIn.substring(1 + msgIn.indexOf(' ')) + "#"+(String)(numb-1));
      //Serial.print(msgIn.substring(1 + msgIn.indexOf(' ')) + "#"+(String)(numb-1));//for logging
      if(ok){
        //altSerial.println(" ok");
        //Serial.println(" ok");//for logging
      }else{
        //altSerial.println("-not ok");//the space is important here
        //Serial.println("-not ok");//for logging
      }
      msgIn = "";
    }else{
      msgIn += lastChar;
    }
  }
  //TX
  if ( txFlag ) {
    Serial.print( "TX:" ); // Identifies transmitted packet for log.
    sendData();
    numb++;
    txFlag = false;
    Serial.print( "RX:" );
  }
  getCharGPS();
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

void sendData() {
  //print out sentenceBuf, all of it - KN
  int i = 0;
  
//  if ( sentenceBuf[0] == 0 || msgIn == "" ) {
//    Serial.println("");
//  }
  if ( sentenceRdy ) {
    while( sentenceBuf[i+1] != 0 ) {
        altSerial.write( sentenceBuf[i] );
        Serial.write( sentenceBuf[i] );//for logging
        i++;
    }
    sentenceRdy = false;
  } else { // Latest data not ready yet, use 2nd buffer
    Serial.print( "BUF" );
    while( sentenceBuf2[i+1] != 0 ) {
        altSerial.write( sentenceBuf[i] );
        Serial.write( sentenceBuf2[i] );//for logging
        i++;
    }
  }
  
  altSerial.print(" "+(String)numb); //also puts the 0 at the end of TX string
  Serial.print(" "+(String)numb);
  altSerial.write(10);
  Serial.write(10);
}

void transmit() {
  txFlag = true;
  help = 0;
}
void printAndSend(String whatToSend){ //sends message to both logger and rf module
  altSerial.print(whatToSend);
  Serial.print(whatToSend);
}
