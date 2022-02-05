#include <FastLED.h>
#include <controller.h>
#define NUM_LEDS 175  
#define DATA_PIN 5

CRGB leds[NUM_LEDS];


  

#define PURPLE_HUE 200
#define PURPLE_BRIGHTNESS 150
#define PULSE_WIDTH 30

int target_led_index = 0;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);
  FastLED.setCorrection(CRGB(255, 180, 200));
}

void loop() {
  int int_parsed = Serial.parseInt();
  if (int_parsed) 
  {
    target_led_index = int_parsed;
  }
  fill_solid(&(leds[0]), NUM_LEDS, CHSV(0,0,0));
  leds[target_led_index] = CRGB(0, 0, 255);
  FastLED.show();
  delay(25);
  
}
