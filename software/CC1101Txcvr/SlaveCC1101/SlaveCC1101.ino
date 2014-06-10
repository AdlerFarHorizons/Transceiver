/*
 Basic example CC1101 Transceiver slave unit.
 
 Lou Nigra
 Far Horizons Lab
 
 Adapted from basic TestRx and TestTx programs.
 
 Transmits a short packet upon receipt of a packet from
 master. Monitors index (first byte) of master packet to
 see if a packet was missed. First byte of transmitted
 packet is an index count.
*/

#include <SPI.h>
#include <RADIOFH.h>

// Local non-default settings exported from TI SmartRF Studio
#include "cc1101_70cm_gfsk_2kb_5khz_manchester_N30dbm.h"

int packet_length = 7; // Max is 61
byte i;
byte size;
int lastIndex; // Used to check if any master packets were missed.
byte index; // First transmitted byte: counts to 255 and repeats.
int rssi;

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
  CC1101.SetReceive(); // Start with waiting for packet
  index = 0;
  Pkt_Buffer[0]=index;
  Pkt_Buffer[1]=0X0A;
  Pkt_Buffer[2]=0X0B;
  Pkt_Buffer[3]=0X0C;
  Pkt_Buffer[4]=0X0D;
  Pkt_Buffer[5]=0X0E;
  Pkt_Buffer[6]=0X0F;  
}

void loop() {
  
  if(CC1101.CheckReceiveFlag()) {
    
    size=CC1101.ReceiveData(Pkt_Buffer);
    rssi = CC1101.ReadRSSI();
    delayMicroseconds(100);
    Serial.print( (int)Pkt_Buffer[0] - lastIndex - 1 );
    lastIndex = (int)Pkt_Buffer[0];
    
    Serial.print( " missed ");
    Serial.print( Pkt_Buffer[0] );
    Serial.print( " " );
    Serial.print( rssi );
    Serial.println("");
    Serial.flush();
    
    transmit(); 
  }
}

void transmit() {
  CC1101.SendData(Pkt_Buffer,packet_length);
  /* 
    A delay of 50 us from SendData to SetReceive seems to be the
    threshold for transmitting the packet properly most of the
    time.
  */
  delayMicroseconds(100);  
  CC1101.SetReceive();
  index++;
  
  Serial.print( "Sent");
  Serial.println("");
  Serial.flush();
}

String byteToHexStr( byte byteVal ) {
  if ( byteVal < 16 ) {
    return "0" + String( byteVal, HEX );
  } else {
    return String( byteVal, HEX );
  }
}








