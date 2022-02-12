#include <FastLED.h>
#include <controller.h>
#include <LibPrintf.h>

//////////// MY SENSORS STUFF /////////////
// https://www.mysensors.org/build/dimmer
// https://www.home-assistant.io/integrations/light.mysensors/
// Enable debug prints
#define MY_DEBUG

#define MY_RF24_CE_PIN 7
#define MY_RF24_CS_PIN 8

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>

uint32_t SLEEP_TIME = 30000; // Sleep time between reports (in milliseconds)
#define MOTDET_ID 11   // Id of the sensor child
#define BED_LEDS_DIMMER 12   // Id of the sensor child
#define DESK_LEDS_DIMMER 13   // Id of the sensor child
#define MONITORS_LEDS_DIMMER 14   // Id of the sensor child

// Initialize motion message
MyMessage motdet_msg(MOTDET_ID, V_TRIPPED);
MyMessage bed_leds_level_msg(BED_LEDS_DIMMER, V_PERCENTAGE );
MyMessage desk_leds_level_msg(DESK_LEDS_DIMMER, V_PERCENTAGE );
MyMessage monitors_leds_level_msg(MONITORS_LEDS_DIMMER, V_PERCENTAGE );
MyMessage bed_leds_state_msg(BED_LEDS_DIMMER, V_LIGHT );
MyMessage desk_leds_state_msg(DESK_LEDS_DIMMER, V_LIGHT );
MyMessage monitors_leds_state_msg(MONITORS_LEDS_DIMMER, V_LIGHT );

#define NUM_LEDS 175  // 177
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

#define MOTION_SENSOR_PIN 2
#define PERSON_PRESENT_LED A0
#define MOTION_DETECTED_LED A1

#define PURPLE_HUE 202
#define PURPLE_BRIGHTNESS 170
#define PULSE_WIDTH 25

#define BED_START 0  // 0 indexed
#define BED_END 77  // 0 indexed
#define DESK_START 78  // 0 indexed
#define DESK_END 138  // 0 indexed
#define MONITORS_START 139  // 0 indexed
#define MONITORS_END 174  // 0 indexed

#define PRESENCE_LIGHT_ANIMATION_INTERVAL ((long) 30000)
#define IR_MOTDET_TO ((long) 60000)

long last_time_presence_light_animation_triggered = 0;
long last_time_motion_detected = 0;
bool person_present = false;
bool motion_detected = false;
bool main_loop_entered = false;

int bed_leds_level = 100, desk_leds_level = 100, monitors_leds_level = 100;
int bed_leds_state = 1, desk_leds_state = 0, monitors_leds_state = 0;

void setup() {
    pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(PERSON_PRESENT_LED, OUTPUT);
  pinMode(MOTION_DETECTED_LED, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  FastLED.setCorrection(CRGB(255, 190, 200));

  Serial.begin(115200);
  
}

void receive(const MyMessage &msg)
{
    if (msg.getType() == V_PERCENTAGE) {

        //  Retrieve the power or dim level from the incoming request message
        int requestedLevel = atoi( msg.data );

        // Clip incoming level to valid range of 0 to 100
        requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
        requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;
        
        if (msg.getSensor() == BED_LEDS_DIMMER)
        {
          bed_leds_level = requestedLevel;
        }
        else if (msg.getSensor() == DESK_LEDS_DIMMER)
        {
          desk_leds_level = requestedLevel;
        }
        else if (msg.getSensor() == MONITORS_LEDS_DIMMER)
        {
          monitors_leds_level = requestedLevel;
        }       
    }
    
    if (msg.getType() == V_LIGHT) {

        //  Retrieve the power or dim level from the incoming request message
        int requested_state = atoi( msg.data );

          
        if (msg.getSensor() == BED_LEDS_DIMMER)
        {
          bed_leds_state = requested_state;
          printf("\nBed leds state updated to %s\n", bed_leds_state ? "ON" : "OFF");
        }
        else if (msg.getSensor() == DESK_LEDS_DIMMER)
        {
          desk_leds_state = requested_state;
          printf("\nDesk leds state updated to %s\n", desk_leds_state ? "ON" : "OFF");

        }
        else if (msg.getSensor() == MONITORS_LEDS_DIMMER)
        {
          monitors_leds_state = requested_state;
          printf("\nMonitors leds state updated to %s\n", monitors_leds_state ? "ON" : "OFF");
        }       
    }
}

void presentation()
{
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo("ADubs Room MySensors Node", "1.2");

    // Register all sensors to gw (they will be created as child devices)
    present(MOTDET_ID, S_MOTION);
    present(BED_LEDS_DIMMER, S_DIMMER);
    present(DESK_LEDS_DIMMER, S_DIMMER);
    present(MONITORS_LEDS_DIMMER, S_DIMMER);
}

void purple_gradual_linear_fill(int offset_i, int stop_i, int level_pct)
{

  int width = (stop_i - offset_i) + 1;
//  for (int pulse_no=1; pulse_no <= 2; pulse_no++)
//  {
    fill_solid(&(leds[offset_i]), width , CHSV(0,0,0));
     FastLED.show();
     wait(250);
    for (int end_i = 0; end_i <= width; end_i ++)
    {

  
  
      fill_solid(&(leds[offset_i]), end_i, CHSV(PURPLE_HUE, 255, (PURPLE_BRIGHTNESS * level_pct)/100 ));
  
      FastLED.show();
  
      wait(15);
  
//      leds[offset_i + next_i] = CHSV(0, 0, 0);
    }
//    fill_solid(&(leds[offset_i]), width, CHSV(0,0,0));
    FastLED.show();
//    wait(1000);
//  }
}

void purple_snake_pulse(int offset_i, int stop_i) //, int dir)
{
  //  dir = dir < 0 ? -1 : 1;
  int width = (stop_i - offset_i) + 1;
  for (int pulse_no=1; pulse_no <= 2; pulse_no++)
  {
    fill_solid(&(leds[offset_i]), width , CHSV(0,0,0));
    for (int start_i = 0; start_i < width; start_i ++)
    {
      int end_i = ((start_i + PULSE_WIDTH) < width) ? (start_i + PULSE_WIDTH) : width;
  
  
      fill_solid(&(leds[offset_i + start_i]), end_i - start_i, CHSV(PURPLE_HUE, 255, PURPLE_BRIGHTNESS));
  
      FastLED.show();
  
      wait(15);
  
      leds[offset_i + start_i] = CHSV(0, 0, 0);
    }
    fill_solid(&(leds[offset_i]), width, CHSV(0,0,0));
    FastLED.show();
    wait(1000);
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

    wait(15);

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
//    wait(10);
//  }
}

void loop() {

if (!main_loop_entered)
{
  printf("\n\n\n< ENTERED MAIN LOOP >\n\n\n"); 
  main_loop_entered = true;
  // send mandatory initial messages to gateway for dimmers 
  send(bed_leds_level_msg.set(bed_leds_level)); 
  send(desk_leds_level_msg.set(desk_leds_level));  
  send(monitors_leds_level_msg.set(monitors_leds_level)); 
  send(bed_leds_state_msg.set(bed_leds_state)); 
  send(desk_leds_state_msg.set(desk_leds_state)); 
  send(monitors_leds_state_msg.set(monitors_leds_state)); 
}

if (monitors_leds_state) 
 
 // Read digital motion sensor value
    motion_detected = digitalRead(MOTION_SENSOR_PIN) == HIGH;
digitalWrite(PERSON_PRESENT_LED, motion_detected);
digitalWrite(MOTION_DETECTED_LED, motion_detected);
    if (motion_detected) 
    {
      printf(".");
      // heartbeat
//      if (millis() - last_time_presence_light_animation_triggered > PRESENCE_LIGHT_ANIMATION_INTERVAL)
//      {
        last_time_presence_light_animation_triggered = millis();
//        purple_snake_pulse(BED_START, BED_END);
        purple_gradual_linear_fill(BED_START, BED_END, bed_leds_level);
//      }
      last_time_motion_detected = motion_detected ? millis() : last_time_motion_detected;
      send(motdet_msg.set(person_present?"1":"0"));  // Send person_present value to mysgw
      //////////////////////////////////////
      // PRESENCE DETECTION CODE // 
      /////////////////////////////////////
      if (!person_present)
      {
        printf("\n\nPerson IS present\n\n");
        person_present = true; 
      }
      
//      purple_snake_pulse(DESK_START, DESK_END);
      
    }

    else
    {
      if (millis() - last_time_motion_detected > IR_MOTDET_TO)
      {
        last_time_presence_light_animation_triggered = 0;
       printf("\nPerson NOT present\n");
        person_present = false;
        // turn off lights
        fill_solid(&(leds[0]), NUM_LEDS, CHSV(0,0,0));
        send(motdet_msg.set(person_present?"1":"0"));  // Send person_present value to mysgw
      }  
    }
    
    
//        digitalWrite(PERSON_PRESENT_LED, person_present);
    
    wait(1000);
    digitalWrite(MOTION_DETECTED_LED, LOW);
    // Sleep until interrupt comes in on motion sensor. Send update every minute
    sleep(digitalPinToInterrupt(MOTION_SENSOR_PIN), RISING, SLEEP_TIME);
  
}
