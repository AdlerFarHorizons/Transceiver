

/*
  Sample CC1101 Transmitter program.
 
 Lou Nigra
 Far Horizons Lab
 
 Adapted from basic Transmit.ino program established by:
 Vidya Gopalakrishnan, Anuja Mahashabde, Michael Horton.
 
 Version 1.0 Dec 3, 2013
 Notes: Basically Transmits packet data ever and prints it out.
 **Master**
 */

#include <SPI.h>
#include <RADIOFH.h>
#include "cc1101_70cm_gfsk_2kb_5khz_manchester_N30dbm.h"
#include <TimerOne.h>

int packet_length = 7; // Max is 61
byte i;
const byte size;

void setup () {

  Serial.begin (9600);
  Serial.println ();
  pinMode(10,OUTPUT);
  Timer1.initialize(1000000);
  Timer1.pwm(9,512);
  
  //run loop every second... does this cause 
  //problems because loop runs on its own?
  Timer1.attachInterrupt(loop, 1000000); 
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
  //Serial.println("starting loop");
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
    Serial.print("Master sent Pkt_Buffer" + i);
  }
  Serial.println("");
  //delay(2000); //not necessary because looping every second??
  
  CC1101.SetReceive();
  if(CC1101.CheckReceiveFlag()) { 
    //should I call a new method if data is recieved, since loop 
    //needs to continue to send data regardless of when data is
    //received?
    
    size=CC1101.ReceiveData(Pkt_Buffer);
    
    for(i=0;i<size;i++) {
      
      Serial.print(Pkt_Buffer[i],HEX);
      Serial.print("Master received Pkt_Buffer"+i);
    }
    
    Serial.print(" ");Serial.print( CC1101.ReadRSSI() );
    Serial.println("");
    
    digitalWrite(10,digitalRead(10)^1); //overflow command?
    
  }
    
  
}


