#define CC1190
#include <SPI.h>
#include "RADIOFH_B.h"
#include "cc1101_RemBase_27mhz_9020000_26khz_gfsk_2400_4940_man_15dbm.h"
int packet_length = 7;
String rssi; 
 void setup()
{
  Serial.flush();
  Serial.begin (9600);
  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset();
  RegConfig();
  delay(1000);  
}


void loop() {
  CC1101.SetReceive();
  rssi = String( CC1101.ReadRSSI( RSSI_OFFSET ) );
  rssi.getBytes( Pkt_Buffer, packet_length );
  for ( int i = 0 ; i < packet_length ; i++ ) {
    Serial.print((char)Pkt_Buffer[i]);
  }
  Serial.println("");
  delay(1000);
}











