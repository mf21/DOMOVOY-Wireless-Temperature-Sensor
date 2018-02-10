/*
  DOMOVOY Wireless Temperature Sensor firmware v.1.0a
  Specs: ATMEGA168P-AU 8MHz int, NRF24l0L+, LM75, KEYS for power control
  rev. 2.5.3

  This is a part of the DOMOVOY project which provides a smart home technology.
  More information about the project can be found here: http://www.mf21.ru

  created 2018
  by KWL
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

RF24 radio(10, 9); // CE, CSN

char text[] = "  ";
const byte address[6] = "DOM01";

volatile int f_wdt=1;

ISR(WDT_vect) {// watchdog procedure

  if(f_wdt == 0) {
    f_wdt=1;
  } else  {
    Serial.println("WDT Overrun!!!");
  }
  
}

void enterSleep(void) {
  
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  sleep_mode();
  sleep_disable();
  power_all_enable();
  
}

void setup() {

  Serial.begin(9600);
  Serial.println("Initialising...");
  delay(100); //Allow for serial print to complete.

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  MCUSR &= ~(1<<WDRF); /* Clear the reset flag. */
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* set new watchdog timeout prescaler value */ /* 8.0 seconds */
  WDTCSR |= _BV(WDIE); /* Enable the WD interrupt (note no reset). */

  ADCSRA = 0;
  
  Serial.println("Initialisation complete.");
  delay(100); //Allow for serial print to complete.
  
}

void loop() {

  if(f_wdt == 1) {
    delay(100);
    float t = 22;  //must be chenged for LM75
    text[0] = char(int(t));
    radio.write(&text, sizeof(text));
    Serial.print("t="); Serial.println(int(t));
    delay(100);

    f_wdt = 0; /* Don't forget to clear the flag. */
    enterSleep(); /* Re-enter sleep mode. */
    
  } else {
    /* Do nothing. */
  }
  
}
