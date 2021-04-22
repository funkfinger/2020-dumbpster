#include <Arduino.h>
#include <FastLED.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>


#define MILLIS_PER_SECOND 1000
#define MILLIS_PER_MINUTE 60000
#define MILLIS_PER_HOUR 3600000

#define NUM_LEDS 7
// #define DATA_PIN PB0
// #define ENABLE_LEDS_PIN PB1
#define DATA_PIN PB0
#define VIBRATION_PIN PB4
#define ENABLE_LEDS_PIN PB3
#define BRIGHTNESS 100

#define FADE_SPEED 30
#define SPARK_CHANCE 12 // 1 in X
#define CYCLE_TIME 30 // every X ms chance of spark and fade...

// globals...
CRGB leds[NUM_LEDS];
uint8_t pixels[NUM_LEDS];
unsigned long previousMillis = 0;

uint8_t flameColor() {
  return random(0,64);
}

void lightPixels() {
  for(int pixel=0; pixel < NUM_LEDS; pixel++) {
    leds[pixel] = CHSV(pixels[pixel], 254, 254);
    FastLED.show();
  }
}

void changeFlameColor(uint8_t pixel) {
  pixels[pixel] = flameColor();
  leds[pixel] = CHSV(pixels[pixel], 254, 254);
}

void changeAllFlameColor() {
  for(int pixel=0; pixel < NUM_LEDS; pixel++) {
    changeFlameColor(pixel);
  }
}

void dimRandom() {
  fadeToBlackBy(leds, NUM_LEDS, FADE_SPEED);
}

void goToSleep() {
  // listen for pin change interrupts...
  bitSet(GIMSK, PCIE);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;
  power_all_disable();
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  power_all_enable();
}

void setupInterrupts() {
  // set PCINT2 (PB4) as interrupt (ATTiny85 pin 73
  bitSet(PCMSK, PCINT4);
  // set the interrupt flag to one - will clear when interrupt routine executed
  bitSet(GIFR, PCIF);
  // enable pin change interrupt
  bitSet(GIMSK, PCIE);
}

void setup() {
  delay(500);
  setupInterrupts();
  pinMode(VIBRATION_PIN, INPUT);
  digitalWrite(ENABLE_LEDS_PIN, HIGH);  

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(ENABLE_LEDS_PIN, OUTPUT);
  digitalWrite(ENABLE_LEDS_PIN, LOW);
}

void loop() {
  goToSleep(); // night night...
  unsigned long currentMillis = millis();
  do {
    if (currentMillis - previousMillis >= CYCLE_TIME) {
      dimRandom();
      if(random(SPARK_CHANCE) == 1) {
        changeFlameColor(random(NUM_LEDS));
      }
      previousMillis = currentMillis;
    }
    FastLED.show();    
  } while (currentMillis + 10000 >= millis());
}

ISR(PCINT4_vect) {
  // wake up and start where we last left off...
}