#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 7
// #define DATA_PIN PB0
// #define ENABLE_LEDS_PIN PB1
#define DATA_PIN PB0
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

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(ENABLE_LEDS_PIN, OUTPUT);
  digitalWrite(ENABLE_LEDS_PIN, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= CYCLE_TIME) {
    dimRandom();
    if(random(SPARK_CHANCE) == 1) {
      changeFlameColor(random(NUM_LEDS));
    }
    previousMillis = currentMillis;
  }
  FastLED.show();
}
