#include <FastLED.h>
#include <controller.h>
#define NUM_LEDS 175  // 177
#define DATA_PIN 5

CRGB leds[NUM_LEDS];


#define PURPLE_HUE 202
#define PURPLE_BRIGHTNESS 200
#define PULSE_WIDTH 25

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
//  Serial.begin(9600);
  FastLED.setCorrection(CRGB(255, 190, 200));
}

void loop() {
  // put your main code here, to run repeatedly:

  fill_solid(&(leds[0]), NUM_LEDS, CHSV(0,0,0));
  for (int start_i = 0; start_i < NUM_LEDS; start_i ++)
  {
    int end_i = ((start_i + PULSE_WIDTH) < NUM_LEDS) ? (start_i + PULSE_WIDTH) : NUM_LEDS;


    fill_solid(&(leds[start_i]), end_i - start_i, CHSV(PURPLE_HUE, 255, PURPLE_BRIGHTNESS));
//    blur1d(&(leds[0]), NUM_LEDS, 30);
    //  if (end_i - start_i > 2)
    //  {
    //    fill_gradient(&(leds[start_i]), start_i, CHSV(PURPLE_HUE, 255, 150), CHSV(PURPLE_HUE, 255, 150), (start_i + end_i)/2);
    //  }



    FastLED.show();

    delay(20);

    leds[start_i] = CHSV(0, 0, 0);
  }
//
//  
//  fill_solid(&(leds[0]), NUM_LEDS, CHSV(0,0,0));
//
//  for (int brightness = 0; brightness < PURPLE_BRIGHTNESS * 2; brightness += 10)
//  {
//
//    if (brightness <= PURPLE_BRIGHTNESS)
//    {
//      fill_solid(&(leds[0]), NUM_LEDS, CHSV(PURPLE_HUE, 255, brightness) );
//    }
//    else
//    {
//      fill_solid(&(leds[0]), NUM_LEDS, CHSV(PURPLE_HUE, 255, PURPLE_B RIGHTNESS - (brightness % PURPLE_BRIGHTNESS)) );
//    }
//    FastLED.show();
//
//    delay(10);
//  }
  
  fill_solid(&(leds[0]), NUM_LEDS, CHSV(0,0,0));
  
    FastLED.show();

    delay(1000);
  
}
