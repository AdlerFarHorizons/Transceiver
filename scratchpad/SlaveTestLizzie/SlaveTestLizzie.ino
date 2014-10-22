/*
  Sample CC1101 Receiver program.
  
  Lou Nigra
  Far Horizons Lab
  
  Adapted from basic Receive.ino program established by:
  Vidya Gopalakrishnan, Anuja Mahashabde, Michael Horton.
  
  Version 1.0 Dec 3, 2013
  Notes: Basically receives packet data, measures RSSI and prints them out.
  **Slave**
*/

#include <SPI.h>
#include <RADIOFH.h>

// Local non-default settings exported from TI SmartRF Studio
#include "cc1101_70cm_gfsk_2kb_5khz_manchester_10dbm.h"

byte i;
const byte size;
const int packet_length = 7;

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
    
    for(i=0;i<size;i++) {
      
      Serial.print(Pkt_Buffer[i],HEX);
      Serial.print("Slave received Pkt_Buffer"+i)
      
    }
    
    Serial.print(" ");Serial.print( CC1101.ReadRSSI() );
    Serial.println("");
  }
    
    Pkt_Buffer[0]=0x00;
    Pkt_Buffer[1]=0X0A;
    Pkt_Buffer[2]=0X0B;
    Pkt_Buffer[3]=0X0C;
    Pkt_Buffer[4]=0X0D;
    Pkt_Buffer[5]=0X0E;
    Pkt_Buffer[6]=0X0F;
    
    CC1101.SendData(Pkt_Buffer,packet_length);
    
    for ( int i = 0 ; i < packet_length ; i++ ) {
   
       Serial.print(Pkt_Buffer[i], HEX);
       Serial.print("Slave sent Pkt_Buffer" + i);
    }
    
     CC1101.SetReceive();
  }
}








