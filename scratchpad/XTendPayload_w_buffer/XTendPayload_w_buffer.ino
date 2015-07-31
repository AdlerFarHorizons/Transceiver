/*
 * 
 * 
 * GPS serial input on pin 8 (for AltSoftSerial) delivers RMC
 * sentence only every second at 4800 baud.
 * 
 * Payload (controller) transmits packet on MsTimer2 interrupt
 * and establishes time slot 0 for all Ground units (peripherals)
 */

#include <AltSoftSerial.h>
#include <MsTimer2.h>
// LN - Adding stuff for a GPS connected to soft serial port
AltSoftSerial altSerial; // TX Pin 9, RX Pin 8
byte pktBuffer[200];
byte pktLastByte;
boolean sentencePending, sentenceRdy, sentenceCpy;
boolean txFlag;
char sentenceBuf[100];
char sentence[100];
String gpsTime, gpsDate, gpsLat, gpsLon, gpsAlt, gspVgnd, gpsHdng;
int sentenceBufIndex, sentenceCpyIndex;

String output = "hello";
int numb = 0;

//const int pin2 = 2;

void setup(){
  Serial.begin(9600);
  
  altSerial.begin( 9600 ); 
  sentencePending = false;
  sentenceRdy = false;
  sentenceCpy = false;
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
    buildPacket();
    sendPacket();
    numb++;
    txFlag = false;    
  }
  checkRcv();
  getCharGPS();
  if ( sentenceRdy ) {
    // Make working copy of fresh GPS sentence
    int i = 0;
    while( sentenceBuf[i] != 0 ) {
      sentence[i] = sentenceBuf[i];
      i++;
    }
    sentenceRdy = false;
  }
}

void getCharGPS() {
  
  char c;
  if ( altSerial.available() ) {
    c = altSerial.read();
    if ( !sentenceRdy ) {
      if ( c == 36 ) {
        sentenceBufIndex = 0;
        sentencePending = true;
      }
      if ( sentencePending && c == 10 ) {
        sentenceRdy = true;
        sentencePending = false;
        c = 0;
      }
      sentenceBuf[sentenceBufIndex] = c;
      sentenceBufIndex++;
    }
  }
}

void buildPacket() {
  byte check = 0x00;
  byte temp;
  int i = -1;
  temp = (byte)( ( numb >> 8 ) & 0x00FF );
  pktBuffer[i++] = temp;
  check ^= temp;
  temp = (byte)( numb & 0x00FF );
  pktBuffer[i++] = temp;
  check ^= temp;
  int j = 6;
  while ( ( temp = (byte)sentence[j++] ) != '*' ) {
    pktBuffer[i++] = temp;
    check ^= temp;
  }
  pktBuffer[i++] = check;
  pktLastByte = i;  
}

void sendPacket() {
  for ( int i = 0 ; i < pktLastByte ; i++ ) {
    Serial.write( (char)pktBuffer[ i ] );
  }
  Serial.write( 10 );
  
}

void checkRcv() {
  byte check = 0xFF;
  byte temp = 0;
  int j = -1;
  while ( Serial.available() ) {
    check ^= temp;
    temp = Serial.read();
    pktBuffer[j++] = temp;
  }
  pktLastByte = j;
  isRcvd = ( pktLastByte >= 0 );  
}

void transmit() {
  txFlag = true;
}

byte checksum( byte* buf, int bufSize ) {
  
  byte check = 0x00;
  for ( int i = 0 ; i < bufSize ; i++ ) {
    check ^= *(buf + i);
  }
  return check;
}

