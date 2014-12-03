/*
  Basic example CC1101 Transceiver master unit.
 
 Lou Nigra
 Far Horizons Lab
 
 Adapted from basic TestRx and TestTx programs.
 
 Transmits a short packet on timer interrupt and waits for
 a response packet from the slave unit. Tests for receipt
 of response.  First byte of packet is an index count. 
*/
#define CC1190
#include <Wire.h>
#include <SPI.h>
//#include <RADIOFH.h>
#include "RADIOFH_B.h"
//#include "cc1101_70cm_gfsk_2kb_5khz_manchester_N30dbm.h"
//#include "cc1101_RemBase_4464000_25khz_gfsk_2kb_3khz_manchester_N30dbm.h"
//#include "cc1101_RemBase_4464000_25khz_gfsk_2kb_3khz_manchester_P10dbm.h"
#include "Step4_CC1190_915.h"
#include <MsTimer2.h>

int packet_length = 30; // Max is 61
byte channel = 0;
int period = 5000; // ms
byte i;
byte size;
boolean rcvdFlag; // Used to identify a missing slave response.
byte index; // First transmitted byte: counts to 255 and repeats.
boolean intFlag;
int rssi;
int numMissed;

String txMsg;
String rxMsg;

void setup () {

  intFlag = false;
  Serial.begin (9600);
  Serial.println ();
  MsTimer2::set(period, transmit);
  MsTimer2::start();
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset();
  RegConfig();
  
  // Set Channel to 0 - 3 for even channel configuration,
  // 0 - 2 for odd channel configuration
  CC1101.WriteSingleReg( RADIO_CHANNR,channel );  

  delay(1000);
  // Note: there's no SetReceive() as Master starts with transmit
  rcvdFlag = true;
  index = 0;
  numMissed = 0;
  txMsg = " WB9SKY Remote Base Control   ";
//  Pkt_Buffer[0]=index;
//  Pkt_Buffer[1]=0X01;
//  Pkt_Buffer[2]=0X02;
//  Pkt_Buffer[3]=0X03;
//  Pkt_Buffer[4]=0X04;
//  Pkt_Buffer[5]=0X05;
//  Pkt_Buffer[6]=0X06;
  
}

void loop() {
  if ( intFlag ) {
    CC1101.SendData(Pkt_Buffer,packet_length);
    /* 
      A delay of 50 us from SendData to SetReceive seems to be the
      threshold for transmitting the packet properly most of the
      time.
    */
    delayMicroseconds(100);
    CC1101.SetReceive();
    
    if (!rcvdFlag ) {
      Serial.println( "!" );
      numMissed++;
    } else {
    }
    rcvdFlag = false;
    intFlag = false;
    Serial.flush();
  }
  
  if(CC1101.CheckReceiveFlag()) {
    
    size=CC1101.ReceiveData(Pkt_Buffer);
    rssi = CC1101.ReadRSSI();
    rcvdFlag = true;
    Serial.print( numMissed );
    Serial.print( " missed ");
    Serial.print( Pkt_Buffer[0] );
    Serial.print(" ");
    Serial.print( rssi );
    Serial.print( " Msg:" );
//    for ( int i = 1 ; i < packet_length ; i++ ) {
//      Serial.print( Pkt_Buffer[i] );
//    }
    Pkt_Buffer[0] = ' ';
    Serial.print( (char*) Pkt_Buffer );
    Serial.println("");
    Serial.flush();
  }
}

void transmit() {
  intFlag = true;
  txMsg.getBytes( Pkt_Buffer, packet_length + 1 );
  Pkt_Buffer[0]=index;
  index++;
}

String byteToHexStr( byte byteVal ) {
  if ( byteVal < 16 ) {
    return "0" + String( byteVal, HEX );
  } else {
    return String( byteVal, HEX );
  }
}

byte collecti2c(byte experiment, byte length, byte * storage)
{
  byte count=0;
  Wire.requestFrom(experiment,length);
  while(Wire.available())
  {
    storage[count]=Wire.read();
    count ++;
  }
  return count;
}

int getMaxRssi( int chan, int deltaTms, int numSamp ) {
  int  maxRssi = -999;
  int rssi = -999;
  for ( int i = 0 ; i < numSamp ; i++ ) {
    rssi = CC1101.ReadRSSI();
    if ( rssi > maxRssi ) maxRssi = rssi;
    delay( deltaTms); 
  }
  return(maxRssi);
}


