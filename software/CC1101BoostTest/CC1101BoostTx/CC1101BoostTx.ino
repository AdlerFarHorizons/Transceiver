/*
  Sample CC1101 Transmitter program.
 
 Lou Nigra
 Far Horizons Lab
 
 Adapted from basic Transmit.ino program established by:
 Vidya Gopalakrishnan, Anuja Mahashabde, Michael Horton.
 
 Version 1.0 Dec 3, 2013
 Notes: Basically Transmits packet data ever and prints it out.
 */
#define CC1190
#include <SPI.h>
//#include <RADIOFH.h>
#include "RADIOFH_B.h"
// Local non-default settings exported from TI SmartRF Studio
#include "cc1101_RemBase_27mhz_9020000_26khz_gfsk_2400_4940_man_15dbm.h"

int packet_length = 7; // Max is 61
byte i;
String rssi;

void setup () {

  Serial.begin (9600);
  Serial.println();
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset();
  RegConfig();
  delay(1000);
}


void loop () {
  
  CC1101.SetReceive();
  rssi = String( CC1101.ReadRSSI( RSSI_OFFSET ) );
  rssi.getBytes( Pkt_Buffer, packet_length );
  for ( int i = 0 ; i < packet_length ; i++ ) {
    Serial.print((char)Pkt_Buffer[i]);
  }
  Serial.println("");
  
  delay(100);
  
  CC1101.SendData(Pkt_Buffer,packet_length);
  delay(2000);
}


