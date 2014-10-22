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
#define CC1190
#include <SPI.h>
//#include <RADIOFH.h>
#include "RADIOFH_B.h"
// Local non-default settings exported from TI SmartRF Studio
//#include "cc1101_70cm_gfsk_2kb_5khz_manchester_N30dbm.h"
//#include "cc1101_RBO_25khz_gfsk_2kb_3khz_manchester_N30dbm.h"
//#include "cc1101_RemBase_4464000_25khz_gfsk_2kb_3khz_manchester_N30dbm.h"
//#include "cc1101_RemBase_4464000_25khz_gfsk_2kb_3khz_manchester_P10dbm.h"
#include "Step4_CC1190_915.h"
int packet_length = 30; // Max is 61
String txMsg;
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

  // Set Channel to 0 - 3 for even channel configuration,
  // 0 - 2 for odd channel configuration
  CC1101.WriteSingleReg( RADIO_CHANNR,0x00 );  

  delay(1000);
  CC1101.SetReceive(); // Start with waiting for packet
  index = 0;
  txMsg = " WB9SKY Remote Base Response  ";
//  Pkt_Buffer[0]=index;
//  Pkt_Buffer[1]=0X0A;
//  Pkt_Buffer[2]=0X0B;
//  Pkt_Buffer[3]=0X0C;
//  Pkt_Buffer[4]=0X0D;
//  Pkt_Buffer[5]=0X0E;
//  Pkt_Buffer[6]=0X0F;  
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
    Serial.print( " Msg:" );
//    for ( int i = 1 ; i < packet_length ; i++ ) {
//      Serial.print( Pkt_Buffer[i] );
//    }
    Pkt_Buffer[0] = ' ';
    Serial.print( (char*) Pkt_Buffer );
    Serial.println("");
    Serial.flush();
    
    transmit(); 
  }
}

void transmit() {
  txMsg.getBytes( Pkt_Buffer, packet_length + 1 );
  CC1101.SendData(Pkt_Buffer,packet_length);
  Pkt_Buffer[0] = (byte)lastIndex;
  /* 
    A delay of 50 us from SendData to SetReceive seems to be the
    threshold for transmitting the packet properly most of the
    time.
  */
  delayMicroseconds(100);  
  CC1101.SetReceive();
  index++;
  
  //Serial.print( "Sent");
  //Serial.println("");
  Serial.flush();
}

String byteToHexStr( byte byteVal ) {
  if ( byteVal < 16 ) {
    return "0" + String( byteVal, HEX );
  } else {
    return String( byteVal, HEX );
  }
}








