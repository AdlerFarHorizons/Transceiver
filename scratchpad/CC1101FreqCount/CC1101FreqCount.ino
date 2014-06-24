/*
  Timer Interrupt using CC1101 GDO0 as external clock signal.
  
  Lou Nigra
  Far Horizons Lab
  June 2014
  
  Uses CC1101 GDO0 as external clock, by connecting externally
  Pin 2 (connected to GDO0) to Pin 5, which is configured as T1,
  the Timer 1 input clock.
  
  Blinks the LED at half the interrupt rate: 
  
  CC1101_XTAL_FREQ / 192 / OCR(=33854) = 2Hz (for 26MHz XTAL)
  
  Based on RoboFreak's "Blinking LED with compare match interrupt"
  example from:
  
  http://letsmakerobots.com/content/arduino-101-timers-and-interrupts

*/

#include <SPI.h>
#include <RADIOFH.h>

#define CLKDIV 16.0
#define GDOCFG 0x38
#define ledPin 13

const float xtalFreq = 26e6; // 70cm CC1101EM
int measSeconds;
float error;
volatile long ppsCount, clkCount, timerVal;
volatile boolean active;

void setup() {
  
  measSeconds = 10;
  Serial.begin (9600); 
  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC1101.GDO_Set();
  CC1101.Reset(); // Default register settings
  
  // Can't find max ext clock frequency for ATMega328, but
  // empirically, CLK_XOSC/12 (0x37) seems like the max.
  CC1101.WriteSingleReg( RADIO_IOCFG0, GDOCFG );
 
  // SPI active interferes with interrupt somehow
  SPI.end();
  
  pinMode(ledPin, OUTPUT);
  
  // Disable all interrupts while messing with them.
  noInterrupts();

  // initialize Timer1 
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Load Compare Match Register with counter value comparison. 
  // This is the number of counts minus 1 between interrupts. 
  OCR1A = 64000;            // compare match register
  
  // Set timer to Clear Timer on Compare mode. Timer set to zero
  // upon reaching compare value and continues counting.
  TCCR1B |= (1 << WGM12);
  
  // Setting CS0-2 in Timer 1 configuration register to 111
  // to enable ext clock on T1 input (pin 5) using rising edge.
  TCCR1B |= (1 << CS12);
  TCCR1B |= (1 << CS11);
  TCCR1B |= (1 << CS10); 
  
  // Enable Timer Compare Interrupt.
  TIMSK1 |= (1 << OCIE1A);
  
  attachInterrupt( 1, pps, RISING );
  
  ppsCount = 0;
  clkCount = 0;
  active = false;

}

/*
  Timer Compare Interrupt Service Routine
  Just toggles the LED.
  
  Uses the ISR() macro which can be used to attach an Interrupt
  Service Routine to any interrupt based on the argument selected
  from the Interrupt vector names table at:
  
  http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html#gad28590624d422cdf30d626e0a506255f  
*/   


void loop() {

  interrupts();
  while( ppsCount < measSeconds);
  // mask timer1 interrupt
  TIMSK1 &= ~(1 << OCIE1A);
  noInterrupts(); // disables pps interrupt as well.
  
  error = 1.0 - ( timerVal / ( xtalFreq / CLKDIV ) + clkCount / ( xtalFreq / CLKDIV / OCR1A ) ) / measSeconds; 
  Serial.println( error * 1e6 );
  ppsCount = 0;
  clkCount = 0;
  active = false;
  
}

ISR(TIMER1_COMPA_vect) { // Uses the ISR() macro
  
  clkCount++;

}

void pps() {  // INT1_vect

  if ( !active ) {
    active = true;
  //   reset timer1 to zero
  // unmask timer1 compare interrupt
    TIMSK1 |= (1 << OCIE1A);
  } else {
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
  //   read timer1 value
    timerVal = 0;
  //   increment pps count 
    ppsCount++;
  }
  
}


