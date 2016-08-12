/*
 * Serial1: GPS and LOGGER 
 * Serial2: RF IN/OUT
 */
#define GPSLEN 100 //max length of GPS output to be sent
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
IntervalTimer txTimer;
//const int pin2 = 2;

void setup(){
  Serial1.begin(9600);  
  Serial2.begin(9600); 
  sentencePending = false;
  sentenceRdy = false;
  sentenceBuf[0] = 0;
  sentenceBufIndex = 0;
  txTimer.begin(transmit, 10000);
  pinMode(5,INPUT_PULLUP);
}
void loop(){
  // Copy any completed sentence to the buffer, set up for next sentence.
  if ( sentenceRdy ) {
    for ( int i = 0 ; i < GPSLEN ; i++ ) {
        sentenceBuf2[i] = sentenceBuf[i];
    }
    sentenceRdy = false;
  }
  //RX section, READ from RF
  char lastChar;
  while(Serial2.available()){
    lastChar = Serial2.read();
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
  //print out sentence, all of it - KN
  int i = 0;
  
  if ( sentenceRdy ) {
    while( sentenceBuf[i+1] != 0 ) {
        Serial2.write( sentenceBuf[i] );
        Serial1.write( sentenceBuf[i] );//for logging
        i++;
    }
    sentenceRdy = false;
  } else { // Buffer data is being updated. Use current sentence
    Serial1.print( "BUF" );
    while( sentenceBuf2[i+1] != 0 ) {
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
