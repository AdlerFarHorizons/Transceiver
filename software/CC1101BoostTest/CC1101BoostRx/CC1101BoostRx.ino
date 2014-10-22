/*
  Sample CC1101 Receiver program.
  
  Lou Nigra
  Far Horizons Lab
  
  Adapted from basic Receive.ino program established by:
  Vidya Gopalakrishnan, Anuja Mahashabde, Michael Horton.
  
  Version 1.0 Dec 3, 2013
  Notes: Basically receives packet data, measures RSSI and prints them out.
*/
#define CC1190
#include <SPI.h>
//#include <RADIOFH.h>
#include "RADIOFH_B.h"
// Local non-default settings exported from TI SmartRF Studio
#include "cc1101_RemBase_27mhz_9020000_26khz_gfsk_2400_4940_man_15dbm.h"

byte i;
byte size;

void setup() {
  
  Serial.begin (9600); 
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset(); // Default register settings
  RegConfig(); // Non-default settings defined in test_setup.h
  delay(1000);
  CC1101.SetReceive();
  
}

void loop() {
  
  if(CC1101.CheckReceiveFlag()) {
    
    size=CC1101.ReceiveData(Pkt_Buffer);
    Serial.print( "Msg:" );
    
    for(i=0;i<size;i++) {      
      Serial.print((char)Pkt_Buffer[i]);
    }
    
    Serial.print(" ");Serial.print( CC1101.ReadRSSI( RSSI_OFFSET ) );
    Serial.println("");
    CC1101.SetReceive();
    
  }
}








