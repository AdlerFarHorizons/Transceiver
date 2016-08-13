/*
 * Serial:  Serial Monitor through USB
 * Serial1: GPS
 * Serial2: RF IN/OUT
 */
#include <TimeLib.h>
const boolean DEBUG = false;
const int GPSLEN = 100; //max length of GPS output to be sent
char pkt_buffer[200];
boolean sentencePending, sentenceRdy, sentenceBufRdy;
boolean txFlag, help;
char sentence[GPSLEN];
char sentenceBuf[GPSLEN];
String gpsTime, gpsDate, gpsLat, gpsLon, gpsAlt, gspVgnd, gpsHdng;
int sentenceIndex = 0;
int rcvdNum;
String msgIn = "";

int numb = 0;
IntervalTimer txTimer;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);  
  Serial2.begin(9600); 
  setSyncProvider( getTeensy3Time );
  sentencePending = false;
  sentenceRdy = false;
  sentence[0] = 0;
  sentenceIndex = 0;
  txTimer.begin(transmit, 10000000);
  pinMode(5,INPUT_PULLUP);
  while( !Serial1 );
  while( !Serial2 );
}
void loop(){
  // Copy any completed sentence to the buffer, set up for next sentence.
  if ( sentenceRdy ) {
    sentenceBufRdy = false;
    for ( int i = 0 ; i < GPSLEN ; i++ ) {
        sentenceBuf[i] = sentence[i];
    }
    sentenceBufRdy = true;
    sentenceRdy = false;
    if ( DEBUG ) {
      Serial.print( " BUF:" );Serial.print( sentence );
      Serial.print( "BUF2:" );Serial.print( sentenceBuf );
    }
  }
  //RX section, READ from RF
  char lastChar;
  while(Serial2.available()){
    lastChar = Serial2.read();
    if(lastChar == 10 && msgIn != ""){ //end of RX string
      Serial.print( "RX " );digitalClockDisplay( now() );Serial.print( ':' );
      Serial.println( msgIn );
      rcvdNum = (int)msgIn.substring(1 + msgIn.indexOf(' ')).toInt();
      msgIn = "";
    }else{
      msgIn += lastChar;
    }
  }
  //TX
  if ( txFlag ) {
    Serial.print("TX ");digitalClockDisplay( now() );Serial.print( ':' );
    sendData();
    numb++;
    txFlag = false;
  }
  getCharGPS();
}

void getCharGPS() {
  char c;
  if ( Serial1.available() ) {
    c = Serial1.read();
    if ( DEBUG ) Serial.write( c );
    if ( !sentenceRdy ) {
      if ( c == 36 ) { //36 is '$', start of GPS statement - KN
        sentenceIndex = 0;
        sentencePending = true;
      }
      if ( sentencePending && c == 10 ) { //10 is new line - KN
        sentenceRdy = true;
        sentencePending = false;
      }
      sentence[sentenceIndex] = c;
      sentenceIndex++;
      sentence[sentenceIndex] = 0;
    }
  }
}

void sendData() {
  //print out sentence, all of it - KN
  int i = 0;
  
  if ( sentenceBufRdy ) {
    while( sentenceBuf[i+1] != 0 ) {
      printAndSendChar( sentenceBuf[i] );
      i++;
    }
    sentenceRdy = false;
  } else { // Buffer data is being updated. Use current sentence
    while( sentence[i+1] != 0 ) {
      printAndSendChar( sentenceBuf[i] );
      i++;
    }
  }

  printAndSendString( " " + (String)numb ); //also puts the 0 at the end of TX string
  printAndSendChar( 10 );
}

void transmit() {
  txFlag = true;
  help = 0;
}

void printAndSendString( String whatToSend ){ //sends message to both terminal & rf module
  Serial2.print( whatToSend ); // RF module
  Serial.print( whatToSend ); // Terminal
}

void printAndSendChar( char whatToSend ){ //sends message to both terminal & rf module
  Serial2.write( whatToSend ); // RF module
  Serial.write( whatToSend ); // Terminal
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void digitalClockDisplay(time_t t) {
  // digital clock display of the time
  int hr = hour(t);
  if ( hr < 10 ) Serial.print( '0' );
  Serial.print(hr);
  printDigits(minute(t));
  printDigits(second(t));
  Serial.print(" ");
  Serial.print(day(t));
  Serial.print(" ");
  Serial.print(month(t));
  Serial.print(" ");
  Serial.print(year(t)); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10) Serial.print('0');
  Serial.print(digits);
}