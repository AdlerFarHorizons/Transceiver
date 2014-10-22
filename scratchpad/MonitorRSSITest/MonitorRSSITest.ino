#include <SPI.h>
//#include <RADIO.h>
#include <RADIOFH.h>
#include "/home/nigra/farhorizonslab@gmail.com/projects/Transceiver/scratchpad/CC1101RemBaseMasterTest/cc1101_RemBase_4464000_25khz_gfsk_2kb_3khz_manchester_P10dbm.h" 
 
//byte RX_Buffer[61]={0};
//byte size,i;
//int rssiDec;
//int rssiOffset = 74;
int rssiDbm;
float freqChan0, deltaChan;
int numChan;
unsigned long freqWord;
float freqScale = 2520.615384615;
int rssiVals[256];
int delta[256];
unsigned long time, startTime;
 
 void setup()
{
  Serial.begin (9600);
//  Serial.println ();
  
     

  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset();
  RegConfig();
  //CC1101.PATABLE();
  // FREQ = (int)(2520.615384615 * fchan0_mhz)
  freqChan0 = 446.400;
  deltaChan = 0.025;
  numChan = 4;
  freqWord = (long)( freqScale * freqChan0 + 0.5 ); // 0.5 for rounding

//  CC1101.WriteSingleReg(RADIO_MDMCFG4, 0x33 ); // channel bw = 58 kHz
//  CC1101.WriteSingleReg(RADIO_DEVIATN, 0x15 ); // deviation = 5.2 kHz
//  CC1101.WriteSingleReg(RADIO_FREQ2, freqWord >> 16 & 0x000000ff ); 
//  CC1101.WriteSingleReg(RADIO_FREQ1, freqWord >>  8 & 0x000000ff ); 
//  CC1101.WriteSingleReg(RADIO_FREQ0, freqWord & 0x000000ff );
  
}


void loop()
{
  Serial.flush();
  for ( int i = 0 ; i < 256 ; i++ ) {
    rssiVals[i] = 0;
    delta[i] = 0;
  }
  
  for ( int j = 0 ; j < 1 ; j++ ) {
    delay(6);
    time = micros();
    startTime = time;
    for ( int i = 0 ; i < numChan ; i++ ) {
      for (int j = 0 ; j < 16 ; j++ ) {
        CC1101.SetReceive();
        byte ibyte = byte(i);
        CC1101.WriteSingleReg(RADIO_CHANNR, ibyte );
        delay(10);
        rssiVals[i] += (int)CC1101.ReadStatusReg(RADIO_RSSI);
        delta[i] += (int)( micros() - time );
        time = micros();
      }
    }
  }
  Serial.print("measurement time: ");Serial.println( (float)( micros() - startTime ) / 1000.0 );
  for ( int i = 0 ; i < numChan ; i++ ) {
    Serial.print((float)i * deltaChan + freqChan0, 3);Serial.print(" ");
    Serial.print( (float)( delta[i] / 16 / 1000.0 ) );Serial.print(" ");
    Serial.println( getRSSIdBm( rssiVals[i] / 16 ) );
  }
  //while(true);
}

int getRSSIdBm( int rssiVal ) {
  if ( rssiVal >= 128 ) {
    return ( rssiVal - 256 ) / 2 - 74;
  } else {
    return rssiVal / 2 - 74;
  }
}











