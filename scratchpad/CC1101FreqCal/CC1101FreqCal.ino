/*
  CC1101 Crystal frequency error measurement.
  
  Lou Nigra
  Far Horizons Lab
  
*/

#include <SPI.h>
#include <RADIOFH.h>

// Local non-default config header exported from TI SmartRF Studio
#include "cc1101_70cm_gfsk_2kb_5khz_manchester_P10dbm.h"

const float xtalNomFreq = 26e6; // 70cm CC1101EM
const int ppsPin = 3;

volatile long count, oldCount, newCount;
int clkDiv;
volatile boolean active;

void setup() {
  
  Serial.begin (9600); 
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset(); // Default register settings
  RegConfig(); // Non-default settings defined in config header.
  noInterrupts();

  // PPS from tracking GPS or other source 
  pinMode( ppsPin, INPUT );
  
  // XTAL freq / 4 => GDO0 output
  CC1101.WriteSingleReg( RADIO_IOCFG0, 0x34 );
  count = 0;
  clkDiv = 4;
    
  attachInterrupt( 0, xtalDiv, RISING );
  attachInterrupt( 1, pps, RISING );
  active = false;
  newCount = 0;
  oldCount = 0;
}

void loop() {
  
  interrupts();
  while( !active );
  while( active );
  noInterrupts();
  Serial.print( oldCount );Serial.println( newCount );
  Serial.println( (float)( 
      newCount - oldCount ) * clkDiv / xtalNomFreq - 1.0 );
  active = false;
  newCount = 0;
  oldCount = 0;
  
}

void xtalDiv() {
  count++;
}

void pps() {
  oldCount = newCount;
  newCount = count;
  active = !active;
}








