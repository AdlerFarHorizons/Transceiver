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
#include "RADIOFH_B.h"

// Local non-default settings exported from TI SmartRF Studio
#include "CC1101_CC1190_33cm_gfsk_2401_2884_Man_P26dbm.h"

int packet_length = 30; // Max is 61
String txMsg;
byte i;
byte size;
int lastIndex; // Used to check if any master packets were missed.
byte index; // First transmitted byte: counts to 255 and repeats.
int rssi;

float baseFreqMHz = 902.0;
int channel = 0;
float chanSpaceHz = 12500.0;

String cmd;

void setup() {
  
  Serial.begin (9600); 
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset(); // Default register settings
  RegConfig(); // Non-default settings defined in test_setup.h

  //CC1101.WriteSingleReg( RADIO_CHANNR,channel );  
  setFreq( baseFreqMHz + channel * chanSpaceHz );
  delay(1000);
  
  CC1101.SetReceive(); // Start with waiting for packet
  index = 0;
}

void loop() {
  
  if(CC1101.CheckReceiveFlag()) {
    
    size=CC1101.ReceiveData(Pkt_Buffer);
    rssi = CC1101.ReadRSSI( RSSI_OFFSET );
    delayMicroseconds(100);
    Serial.print("Missed:");
    Serial.print( (int)Pkt_Buffer[0] - lastIndex - 1 );
    lastIndex = (int)Pkt_Buffer[0];
    Serial.print( " Index:");
    Serial.print( Pkt_Buffer[0] );
    Serial.print( " " );
    Serial.print( rssi );
    Serial.print( " Msg:" );
    Pkt_Buffer[0] = ' ';
    Serial.print( (char*) Pkt_Buffer );
    Serial.println("");
    Serial.flush();
    String callSign = " WB9SKY ";
    txMsg = callSign + cmd; 
    Serial.print( "Sending:" );Serial.println( txMsg );
    transmit();
    cmd = "";
    // Check for command from terminal
    byte incoming = Serial.read();
    if ( incoming >= -1 ) {
      if ( incoming == '1' ) cmd = "L1";
      if ( incoming == '0' ) cmd = "L0";
    }
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
  
  Serial.flush();
}

String byteToHexStr( byte byteVal ) {
  if ( byteVal < 16 ) {
    return "0" + String( byteVal, HEX );
  } else {
    return String( byteVal, HEX );
  }
}

void setFreq( float freqMHz ) {
  
    long freq = (long)( freqMHz / (27.0 / 65536) + 0.5);
    
    byte freq0= (byte)(freq&0xFF);
    byte freq1= (byte)((freq&0xFF00)>>8);
    byte freq2= (byte)((freq&0xFF0000)>>16);
    
    CC1101.WriteSingleReg( RADIO_FREQ0, freq0);
    CC1101.WriteSingleReg( RADIO_FREQ1, freq1);
    CC1101.WriteSingleReg( RADIO_FREQ2, freq2);
    delay(100);
  
}

/*
  Add this function to your sketch and call it whenever you want the
  amount of free bytes with:
  Serial.println( freeRam() );
*/

int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
