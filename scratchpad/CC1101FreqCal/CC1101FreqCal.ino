/*
  Timer Interrupt using CC1101 GDO0 as external clock signal.
  
  Lou Nigra
  Far Horizons Lab
  June 2014
  
  Uses CC1101 GDO0 as external clock, by connecting externally
  Pin 2 (connected to GDO0) to Pin 5, which is configured as T1,
  the Timer 1 input clock. It is configured to interrupt on timer
  comparison.
  
  External GPS PPS output is connected to Pin 3 and configured as
  an external interrupt. The GPS should be configured to have PPS
  output only when it has a fix.  
*/

#include <SPI.h>
#include <RADIOFH.h>

#define CLKDIV 16.0
#define GDOCFG 0x38

const int id0Pin = 4;
const int id1Pin = 6;
const int id2Pin = 7;
const int id3Pin = 8;
const float xtalFreq = 26e6; // 70cm CC1101EM
int measSeconds;
long totalCount;
float error;
volatile long ppsCount, clkCount, timerVal;
volatile boolean active;

void setup() {
  pinMode( id0Pin, INPUT_PULLUP );
  pinMode( id1Pin, INPUT_PULLUP );
  pinMode( id2Pin, INPUT_PULLUP );
  pinMode( id3Pin, INPUT_PULLUP );
  measSeconds = 10;
  Serial.begin (9600); 
//  SPI.begin ();
//  SPI.setClockDivider(SPI_CLOCK_DIV4);   
//  SPI.setBitOrder(MSBFIRST);   
//  SPI.setDataMode(SPI_MODE0) ;
//  CC1101.GDO_Set();
//  CC1101.Reset(); // Default register settings
//  
//  // Can't find max ext clock frequency for ATMega328, but
//  // empirically, CLK_XOSC/12 (0x37) seems like the max.
//  CC1101.WriteSingleReg( RADIO_IOCFG0, GDOCFG );
// 
//  // SPI active interferes with interrupt somehow
//  SPI.end();
//  
//  // Disable all interrupts while messing with them.
//  noInterrupts();
//
//  // initialize Timer1 
//  TCCR1A = 0;
//  TCCR1B = 0;
//  TCNT1  = 0;
//
//  // Load Compare Match Register with counter value comparison. 
//  // This is the number of counts minus 1 between interrupts. 
//  OCR1A = 16000;            // compare match register
//  
//  // Set timer to Clear Timer on Compare mode. Timer set to zero
//  // upon reaching compare value and continues counting.
//  TCCR1B |= (1 << WGM12);
//  
//  // Setting CS0-2 in Timer 1 configuration register to 111
//  // to enable ext clock on T1 input (pin 5) using rising edge.
//  TCCR1B |= (1 << CS12);
//  TCCR1B |= (1 << CS11);
//  TCCR1B |= (1 << CS10); 
  
  // Enable Timer Compare Interrupt.
  // TIMSK1 |= (1 << OCIE1A);
  // Wait for PPS...
//  active = false; 
//  long time = millis();
//  attachInterrupt( 1, pps, RISING );
//  interrupts();
//  Serial.println( "Waiting for PPS from GPS..." );
//  while ( !active ) {
//    delay( 1000 );
//    Serial.println( "...still waiting..." );
//  }
//  
//  Serial.println( "PPS active: Measuring error, back in about 20 seconds...\n" );
  
  byte id = getId();
  Serial.print( "id:" );Serial.println( id );
  if ( id == 0x0F ) {
    Serial.println( "Can't determine CC1101 ID, enter single hex character:" );
    byte c = 0;    
    while ( !( c >= '0' and c <= '9' or c >= 'A' and c < 'F' ) ) { 
      c = Serial.read();

    }
    Serial.println( c );
    if ( c >= '0' and c <= '9' ) {
      id = c - 48;
    } else {
      id = c - 55;
    }
  }
  Serial.print( "ID:0x" );Serial.println( id, HEX );
  
  ppsCount = 0L;
  clkCount = 0L;
  active = false;
}

void loop() {
  
  // Run this twice and ignore the first. The first measurement 
  // is always bogus. Have no idea why.
  interrupts();
  while( ppsCount < measSeconds );
  // mask timer1 interrupt
  TIMSK1 &= ~(1 << OCIE1A);
  noInterrupts(); // disables pps interrupt as well.
  
  ppsCount = 0L;
  clkCount = 0L;
  active = false;
  
  interrupts();
  while( ppsCount < measSeconds );
  noInterrupts();
  TIMSK1 &= ~(1 << OCIE1A);  
  totalCount = CLKDIV * ( ( ( (long)OCR1A + 1 ) * 
                            ( clkCount - 1 ) ) + timerVal );
  error = 1.0e+6 * ( 1.0 - (float)totalCount / 
                    (float)ppsCount / xtalFreq );
  float correction =( -error * 1.0e-6 * 16384.0 );
  
  interrupts();
  Serial.print( "CC1101 XTAL Offset:" );
  Serial.print( error, 1 );Serial.println( " ppm");
  Serial.println( correction );
  while( true );
}

ISR(TIMER1_COMPA_vect) { // Uses the ISR() macro

  clkCount++;

}

void pps() {  // INT1_vect

  if ( !active ) {
    active = true;
  //   reset timer1 to zero
    TCNT1 = 0L;
    clkCount = 0L;
  // unmask timer1 compare interrupt
    TIMSK1 |= (1 << OCIE1A);
  } else {
  //   read timer1 value
    timerVal = TCNT1;
  //   increment pps count 
    ppsCount++;
  }
  
}

byte getId() {
  
  byte result;
  result = digitalRead( id0Pin );
  result |= ( digitalRead( id1Pin ) << 1 );
  result |= ( digitalRead( id2Pin ) << 2 );
  result |= ( digitalRead( id3Pin ) << 3 );
  return result;

}



