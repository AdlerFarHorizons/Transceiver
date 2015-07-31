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
boolean txFlag;
char sentenceBuf[100];
String gpsTime, gpsDate, gpsLat, gpsLon, gpsAlt, gspVgnd, gpsHdng;
int sentenceBufIndex = 0;

String output = "hello";
int numb = 0;

//const int pin2 = 2;

void setup(){
  Serial.begin(9600);
  
  altSerial.begin( 4800 ); 
  sentencePending = false;
  sentenceRdy = false;
  sentenceBuf[0] = 0;
  sentenceBufIndex = 0;
  MsTimer2::set(5000, transmit);
  MsTimer2::start();
}

void loop(){
  //Serial.println(output+" "+numb);
  //numb++;
  //delay(1000);
  if ( txFlag ) {
    sendData();
    numb++;
    txFlag = false;
  } else {
    getCharGPS();
  }
}

void getCharGPS() {
  
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
}

void sendData() {
  //print out sentenceBuf, all of it - KN
  int i = 0;
  while( sentenceBuf[i] != 0 ) {
      Serial.write( sentenceBuf[i] );
      sentenceRdy = false;
      i++;
  }
}

void transmit() {
  txFlag = true;
}
