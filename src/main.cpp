#include <Arduino.h>
#include "FastLED.h"

// edited from code by Mark Kriegsman, December 2014

// test code for my led strip
// edited to work with VS Code + PlatformIO + ESP32
// Evgeni Vi, 2021


// declaration
void rainbow();
void rainbowWithGlitter();
void confetti();
void sinelon();
void juggle();
void bpm ();
void nextPattern();
void addGlitter( fract8 chanceOfGlitter);

#define DATA_PIN    23
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  128 //96
#define ONBOARD_LED 1 // pin 1 is onboard LED
#define ONBOARD_BTN 0 // pin 0 is "boot" button on ESP32

void setup() {
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,HIGH); // onboard led is inverted
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbow };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  

  // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 60 ) { nextPattern(); } // change patterns periodically
  if (!digitalRead(ONBOARD_BTN)){
    nextPattern();
    while (!digitalRead(ONBOARD_BTN)) //debounce
    {
      digitalWrite(ONBOARD_LED,LOW);
      delay(50);
      digitalWrite(ONBOARD_LED,HIGH);
      delay(50);
    }
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(120); //80
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(26,0,NUM_LEDS); //13
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
