#include <SPI.h>
#include <RADIO.h>
 
 
byte RX_Buffer[61]={0};
byte size,i;
int rssiDec;
int rssiOffset = 74;
long rssiAve;
int rssiDbm;
float freqChan0;
unsigned long freqWord;
float freqScale = 2520.615384615;
 
 void setup()
{
  Serial.flush();
  Serial.begin (9600);
//  Serial.println ();
  
     

  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC110L.GDO_Set();
  CC110L.Reset();
  CC110L.RegConfig();
  //CC110L.PATABLE();
  // FREQ = (int)(2520.615384615 * fchan0_mhz)
  freqChan0 = 435.0;
  freqWord = (long)( freqScale * freqChan0 + 0.5 ); // 0.5 for rounding

  CC110L.WriteSingleReg(RADIO_MDMCFG4, 0x33 ); // channel bw = 58 kHz
  CC110L.WriteSingleReg(RADIO_DEVIATN, 0x15 ); // deviation = 5.2 kHz
  CC110L.WriteSingleReg(RADIO_FREQ2, freqWord >> 16 & 0x000000ff ); 
  CC110L.WriteSingleReg(RADIO_FREQ1, freqWord >>  8 & 0x000000ff ); 
  CC110L.WriteSingleReg(RADIO_FREQ0, freqWord & 0x000000ff );
  byte version = CC110L.ReadSingleReg(RADIO_PARTNUM);
  Serial.print( "Chip: " );Serial.println( version );
  CC110L.SetReceive();
  
}


void loop()
{
  //if(CC110L.CheckReceiveFlag())
  //{
    //size=CC110L.ReceiveData(RX_Buffer);
   // for(i=0;i<size;i++)
    //{
      //Serial.print(RX_Buffer[1],DEC);
//      Serial.print(' ',BYTE);
   // }
    //Serial.println("0");
    //CC110L.SetReceive();
    for ( int i = 0 ; i < 16 ; i++ ) {
      delay(1000);
      rssiDec = (int)CC110L.ReadStatusReg(RADIO_RSSI);
      if(rssiDec >=128)
      {
        rssiDbm = (rssiDec-256)/2-rssiOffset;
      }
      else
      {
        rssiDbm = (rssiDec)/2-rssiOffset;
      }
      Serial.println(rssiDbm);
      Serial.flush();
      rssiAve += pow(10.0, ((float)rssiDbm+120.0)/10);
    }
  Serial.print("Ave: ");Serial.println(10.0*log10(rssiAve/16.0));
  Serial.flush();
  while(true);
  //}
}











