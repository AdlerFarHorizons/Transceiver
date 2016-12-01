/*
 * Serial:  Serial Monitor through USB
 * Serial1: GPS 
 * Serial2: RF IN/OUT
 * GPS data double buffering neccessary since packets are
 *  asynchronous with TX interrupt
 * Logging filename: 
 *  reflects creation time: yymmddhh.mms where "s" is 10s of seconds.
 * TX Message:
 *   PI<pktIdx>$PT<pktTime>MP<missedPktFlg>,GT<gpsTime>gpsLon,gpsLat,gpsAlt,vBatt,chkSum
 */
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

#define GPSLEN 100 //max length of GPS output to be sent
#define RXMSGLEN 100 // max length of RX message packet
#define DEBUG true // enables serial monitor outputs.
const int txPeriod = 10000000;
const float vRef = 3.3;

// 
const String gpsConfStr = "$PTNLSNM,0221,60*51\r\n";

const float battMonScale = 57.0 / 10.0 ;
const float regMonScale = 20.0 / 10.0 ;
const int battMonPin = 8;
const int regMonPin = 7;
const int tempMonPin = 6;
const int tempMonType = 0;
const int ppsPin = 2;
const float vBattThr = 6.5;
const float vRegMonThr = 4.5;
float vBatt;
float vReg;
float tempVal;
int battVal, regVal;
boolean vBattOk;
boolean vRegOk;

const int tx_nPin = 6;
boolean txIndFlg;

// SD card hard drive
const int chipSelect = 14;
File logFile;
char logFileName[12];

boolean isLog;

char packetBuf[200];
char gps[GPSLEN];
char gpsBuf[GPSLEN];
int gpsIndex = 0;
boolean gpsRdy, gpsChkFlg, gpsErrFlg, gpsBufCurrent, gpsValid;
byte gpsChk;
byte rxMsg[RXMSGLEN];
int rxMsgIndex = 0;
boolean rxMsgRdy,rxMsgChkFlg;
byte rxMsgChk;
boolean txFlag, help;
String gpsTime, gpsDate, gpsLat, gpsLon, gpsAlt, gspVgnd, gpsHdng;
int numb;
IntervalTimer txTimer;

char modeChange = ':';
char cmdBuf[82];
int sentenceBufIndex = 0;
int cmdBufIndex = 0;
boolean cmdMode, cmdRdy;

time_t ppsTime = 0L;
boolean ppsFlg, clkAdjFlg;

void setup(){

  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  
  if ( timeStatus() != timeSet ) {
    if ( DEBUG ) Serial.println("Unable to sync with the RTC");
  } else {
    if ( DEBUG ) Serial.println("RTC has set the system time");
  }

  Serial.begin(9600);
  Serial1.begin(9600);  
  Serial2.begin(9600); 
  gpsRdy = false;
  gps[0] = 0;
  gpsIndex = 0;
  rxMsgRdy = false;
  rxMsg[0] = 0;
  rxMsgIndex = 0;
  numb = 0;
  gpsBufCurrent = true;
  txTimer.begin( transmit, txPeriod );
  txFlag = false;
  pinMode( tx_nPin, INPUT );
  attachInterrupt( tx_nPin, txIndicator, FALLING );
  txIndFlg = false;
  ppsFlg = false;
  clkAdjFlg = false;
  
  delay( 1000 ); // Necessary to ensure serial ports are ready
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    if ( DEBUG ) Serial.println("SD card failure");
    isLog = false;
  } else {
    if ( DEBUG ) Serial.println( "SD card initialized" );
    isLog = true;
  }
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  String test = getTimeStamp();
  String logFileNameStr = test.substring( 2, 10 ) + "." + test.substring( 10, 13 );
  logFileNameStr.toCharArray( logFileName, 32 );
  logFile = SD.open( logFileName, FILE_WRITE );
  Serial.println( isLog );
  // if the file opened okay, write to it
  if (logFile) {
    if ( DEBUG ) Serial.println( logFileNameStr + " opened" );
    isLog = true;
  } else {
    // if the file didn't open, print an error:
    if ( DEBUG ) Serial.println("error opening " + logFileNameStr );
    isLog = false;
  }
  if ( isLog ) logFile.println( "--------------\n" );
  logFile.flush();
  Serial.println( "Configuring GPS..." );
  //Serial1.print( gpsConfStr );
  if ( DEBUG ) Serial.println( "Setup done." );

}

void loop(){

  if ( !gpsBufCurrent ) updateGPSBuffer();

  if ( Serial2.available() ) getRXByte();
  
  if ( txFlag ) sendData();
  
  if ( Serial1.available() ) getGPSByte();

  //if ( monFlg ) monitor();

  if ( txIndFlg ) {
    vBatt = battMonScale * vRef * battVal / 1024.0 ;
    vReg = regMonScale * vRef * regVal / 1024.0 ;
    tempVal = readTemp( tempMonPin, tempMonType, vRef );
    Serial.print( "T:" );Serial.print( tempVal, 1 );Serial.print( " Batt:" );Serial.print( vBatt, 1 );
    Serial.print( " Reg:" );Serial.println( vReg, 1 );
    txIndFlg = false;
  }

  //if ( ppsFlg ) attachInterrupt( ppsPin, ppsSvc, RISING );

//  if ( clkAdjFlg ) {
//    clkAdjFlg = 0;
//  }
  
}

void getGPSByte() {
  char c = Serial1.read();
  if ( c == '$' ) { // '$', start of GPS statement - KN
    gpsIndex = 0;
    gpsRdy = false;
    gpsChk = 0;
    if ( DEBUG ) Serial.println( "GPS msg started..." );
  }
  if ( c == '*' ) { // '*' indicates end of GPS sentence
    gpsChkFlg = true;
    if ( DEBUG ) {
      Serial.println( "GPS msg ended." );
      //Serial.println( gps );
    }
  }
  if ( !gpsRdy ) {
    if ( gpsChkFlg ) {
      gpsErrFlg = gpsChk != c;
      gpsChkFlg = false;
      gpsRdy = true;
      gpsBufCurrent = false;
    } else {
      gps[gpsIndex] = c;
      gpsChk ^= (byte)c;
    }
    gpsIndex++;
    gps[gpsIndex] = 0;
  }
}

void sendData() {
  // Set up to catch the beginning of the TX sequence for voltage measurements.
  attachInterrupt( tx_nPin, txIndicator, FALLING );
  //Serial.println( getTimeStamp() );
  //Serial.println( isLog );
  int i = 0;
  if ( gpsBufCurrent ) {
    while( gpsBuf[i+1] != 0 ) {
        Serial2.write( gpsBuf[i] );
        if ( isLog ) logFile.write( gpsBuf[i] );
        if ( DEBUG ) Serial.write( gpsBuf[i] );
        i++;
    }
  } else { // Buffer data is being updated. Use current sentence
    while( gps[i+1] != 0 ) {
        Serial2.write( gps[i] );
        Serial.write( gps[i] );
        if ( isLog ) logFile.write( gps[i] );
        if ( DEBUG ) Serial.write( gps[i] );
        i++;
    }
  }
  
  numb++;
  txFlag = false;
  Serial2.print(" "+(String)numb); //also puts the 0 at the end of TX string
  logFile.print( " " + (String)numb );
  if ( DEBUG ) Serial.print( " " + (String)numb );
  Serial2.write(10);
  if ( isLog ) logFile.write(10);
  if ( DEBUG ) Serial.write( 10 );

  if ( isLog ) logFile.flush();
}

void transmit() {
  txFlag = true;
  if ( DEBUG ) Serial.println( "TX Interrupt..." );
}

void updateGPSBuffer() {
    for ( int i = 0 ; i < GPSLEN ; i++ ) {
        gpsBuf[i] = gps[i];
    }
    gpsBufCurrent = true;
    if ( DEBUG ) Serial.println( "GPS Buffer updated." );
    int gpsQual = getField( String( gpsBuf ), 6, ',' ).toInt();
    gpsValid = gpsQual == 1 || gpsQual == 2;
}

void getRXByte() {
  byte b = Serial2.read();
  if ( b == '&' ) { // '&', start of RX Message
    rxMsgIndex = 0;
    rxMsgRdy = false;
    if ( DEBUG ) Serial.println( "RX Msg started..." );
  }
  if ( b == 10 ) { //10 is new line - KN
    rxMsgRdy = true;
    if ( DEBUG ) {
      Serial.println( "RX Msg ended" );
    }
  }
  rxMsg[gpsIndex] = b;
  rxMsgIndex++;
  rxMsg[gpsIndex] = 0; // String terminator
}

void getCharTerm() {
  
  char c;
  if (Serial.available() ) {
    c = Serial.read();
    if ( c == ':' ) {
      cmdMode = !cmdMode;     
      while ( Serial.read() >= 0 );
      if ( !cmdMode ) {
        Serial.println("\n");
      } else {
        Serial.print( "\ncmd:" );
      }
    } else if ( cmdMode && !cmdRdy) {
      if ( c == 8 ) { // Backspace
        cmdBuf[cmdBufIndex] = 0;
        if ( cmdBufIndex != 0 ) cmdBufIndex -=  1;
      } else {
        if ( c == 10 ) {
          cmdRdy = true;
        }
        cmdBuf[cmdBufIndex] = c;
        cmdBufIndex +=1;
      }
    }
  }
}

String getField( String str, int fieldIndex, char delim ) {
  int startIndex = -1;
  int endIndex = 0;
  for ( int i = 0 ; i <= fieldIndex ; i++ ) {
    startIndex = endIndex + 1;
    endIndex = str.indexOf( delim, startIndex );
  }
  if ( endIndex == -1 ) endIndex = str.length() - 1;
  return str.substring( startIndex, endIndex );
}

void txIndicator() {
  txIndFlg = true;
  // Disable interrupt for subsequent TX subpackets
  detachInterrupt( tx_nPin );
  // Grab battery voltages while TX is still active
  battVal = analogRead( battMonPin );
  regVal = analogRead( regMonPin );
  if ( DEBUG ) Serial.println( "TX Indicator interrupt" );
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

String getTimeStamp() {
  char timeStr[15];
  time_t t = now();
  sprintf( timeStr, "%04i%02i%02i%02i%02i%02i",
           year(t), month(t), day(t), hour(t), minute(t), second(t) );
  return String( timeStr );
//  Serial.print( String(hour(t)) + ":" + String(minute(t)) );Serial.print( ":" );
//  Serial.print( minute(t) );Serial.print( ":" );
//  Serial.print( second(t) );Serial.print( " " );
//  Serial.print( day(t) );Serial.print( "," );
//  Serial.print( month(t) );Serial.print( "," );
//  Serial.print( year(t) );Serial.println( "" );
}

/*
  This is a function you can add to read the temperature
  from one of several devices:
  
  Arguments:
  pin (int) Analog pin number attached to sensor.
  sensType (int) Selects one of three sensor types.
  vRef (float) The arduino's supply voltage (5.0 for UNO)
*/

float readTemp( int pin, int sensType, float vRef ) {
  // sensType parameter vs. Temperature Sensor type:
  //   0  LM60
  //   1  MAX6605
  //   2  TMP36
  int mVoltsAtRefTemp[] = { 424, 744, 750 };
  int refTempC[] = { 0, 0, 25 };
  float mVperDegC[] = { 6.25, 11.9, 10.0 };

  int reading = analogRead( pin );
  float mVolts = reading * vRef / 1.024;

  return( ( mVolts - mVoltsAtRefTemp[sensType] ) / 
            ( mVperDegC[sensType] ) + 
            refTempC[sensType]);
  
}

void ppsSvc() {
  ppsTime = now();
  detachInterrupt( ppsPin );
  if ( DEBUG ) {
    Serial.print( "PPS Int:" );
    Serial.println( ppsTime );
  }
  clkAdjFlg = true;
  ppsFlg = false;
}


