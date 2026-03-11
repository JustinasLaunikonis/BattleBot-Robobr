#include <Adafruit_NeoPixel.h>

const int PIN_NEO = 12;
const int NUM_PIXELS = 4;

// NEO_RGB: these LEDs use RGB byte order, not the more common GRB
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEO, NEO_RGB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.setBrightness(100);

  pixels.setPixelColor(0, pixels.Color(0, 255, 0));    // front-left  = green
  pixels.setPixelColor(1, pixels.Color(255, 0, 0));    // front-right = red
  pixels.setPixelColor(2, pixels.Color(0, 255, 0));    // back-left   = green
  pixels.setPixelColor(3, pixels.Color(255, 0, 0));    // back-right  = red

  pixels.show();
}

void loop() {
  
}
