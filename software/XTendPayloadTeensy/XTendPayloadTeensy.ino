/*
 * Serial:  Serial Monitor through USB
 * Serial1: GPS and LOGGER 
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
String msgIn = "";

String output = "hello";
int numb = 0;
IntervalTimer txTimer;
//const int pin2 = 2;

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
    if( lastChar == 10 && msgIn != "" ){ //if I get a new line
      Serial.print( "RX " );digitalClockDisplay( now() );
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
    Serial.print("TX:");digitalClockDisplay( now() );
    Serial1.print( "TX:" ); // Identifies transmitted packet for log.
    sendData();
    numb++;
    txFlag = false;
    Serial1.print( "RX:" );
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
        Serial2.write( sentenceBuf[i] );
        Serial1.write( sentenceBuf[i] );//for logging
        i++;
    }
    sentenceRdy = false;
  } else { // Buffer data is being updated. Use current sentence
    while( sentence[i+1] != 0 ) {
        Serial2.write( sentenceBuf[i] );
        Serial1.write( sentenceBuf2[i] );//for logging
        i++;
    }
  }
  
  Serial2.print(" "+(String)numb); //also puts the 0 at the end of TX string
  Serial1.print(" "+(String)numb);
  Serial2.write(10);
  Serial1.write(10);
}

void transmit() {
  txFlag = true;
  help = 0;
}
void printAndSend(String whatToSend){ //sends message to both logger and rf module
  Serial2.print( whatToSend ); // RF module
  Serial1.print( whatToSend ); // Logger
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
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}