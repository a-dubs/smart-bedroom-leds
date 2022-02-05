#include <FastLED.h>
#include <controller.h>
#include <LibPrintf.h>

//////////// MY SENSORS STUFF /////////////
// Enable debug prints
// #define MY_DEBUG

#define MY_RF24_CE_PIN 7
#define MY_RF24_CS_PIN 8

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>

uint32_t SLEEP_TIME = 30000; // Sleep time between reports (in milliseconds)
#define CHILD_ID 21   // Id of the sensor child

// Initialize motion message
MyMessage msg(CHILD_ID, V_TRIPPED);

#define NUM_LEDS 175  // 177
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

#define MOTION_SENSOR_PIN 2
#define PERSON_PRESENT_PIN A0
#define MOTION_DETECTED_LED A1

#define PURPLE_HUE 202
#define PURPLE_BRIGHTNESS 200
#define PULSE_WIDTH 25

#define BED_START 0  // 0 indexed
#define BED_END 77  // 0 indexed
#define DESK_START 78  // 0 indexed
#define DESK_END 138  // 0 indexed
#define MONITORS_START 139  // 0 indexed
#define MONITORS_END 174  // 0 indexed

#define IR_MOTDET_TO ((unsigned long) 60000)

unsigned long last_time_present = 0;
bool person_present = false;
bool motion_detected = false;
bool main_loop_entered = false;
void setup() {
    pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(PERSON_PRESENT_PIN, OUTPUT);
  pinMode(MOTION_DETECTED_LED, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  FastLED.setCorrection(CRGB(255, 190, 200));

  Serial.begin(115200);

}



void presentation()
{
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo("ADubs Room MyS Arduino", "1.1");

    // Register all sensors to gw (they will be created as child devices)
    present(CHILD_ID, S_MOTION);
}

void purple_snake_pulse(int offset_i, int stop_i)
{
  
  int width = (stop_i - offset_i) + 1;
  for (int pulse_no=1; pulse_no <= 2; pulse_no++)
  {
    fill_solid(&(leds[offset_i]), width + 2 , CHSV(0,0,0));
    for (int start_i = 0; start_i < width; start_i ++)
    {
      int end_i = ((start_i + PULSE_WIDTH) < width) ? (start_i + PULSE_WIDTH) : width;
  
  
      fill_solid(&(leds[offset_i + start_i]), end_i - start_i, CHSV(PURPLE_HUE, 255, PURPLE_BRIGHTNESS));
  
      FastLED.show();
  
      delay(20);
  
      leds[offset_i + start_i] = CHSV(0, 0, 0);
    }
    fill_solid(&(leds[offset_i]), width+2, CHSV(0,0,0));
    FastLED.show();
    delay(1000);
  }
  
}

void full_room_purple_snake_pulse()
{
  
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

    delay(15);

    leds[start_i] = CHSV(0, 0, 0);

    }
    fill_solid(&(leds[0]), NUM_LEDS, CHSV(0,0,0));
    

  

  //////// QUICK SINGLE STROBE AFTER SNAKE PULSE IS DONE ///////////
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
}

void loop() {

if (!main_loop_entered)
{
  printf("\n\n\n< ENTERED MAIN LOOP >\n\n\n"); 
  main_loop_entered = true;
}
 
 // Read digital motion sensor value
    motion_detected = digitalRead(MOTION_SENSOR_PIN) == HIGH;
digitalWrite(PERSON_PRESENT_PIN, motion_detected);
digitalWrite(MOTION_DETECTED_LED, motion_detected);
    if (motion_detected) 
    {
      printf(".");
      // heartbeat
      last_time_present = motion_detected ? millis() : last_time_present;
      send(msg.set(person_present?"1":"0"));  // Send person_present value to mysgw
      //////////////////////////////////////
      // PRESENCE DETECTION CODE // 
      /////////////////////////////////////
      if (!person_present)
      {
        printf("\n\nPerson IS present\n\n");
        person_present = true;
      }
      
//      purple_snake_pulse(DESK_START, DESK_END);
      purple_snake_pulse(BED_START, BED_END);
    }

    else
    {
      if (millis() - last_time_present > IR_MOTDET_TO)
      {
//        printf("Person NOT present\n");
        person_present = false;
        send(msg.set(person_present?"1":"0"));  // Send person_present value to mysgw
      }  
    }
    
    
//        digitalWrite(PERSON_PRESENT_PIN, person_present);
    
    delay(1000);
    digitalWrite(MOTION_DETECTED_LED, LOW);
    // Sleep until interrupt comes in on motion sensor. Send update every minute
    sleep(digitalPinToInterrupt(MOTION_SENSOR_PIN), RISING, SLEEP_TIME);
  
}
