/*************************************
 * Code to drive the pixels on the board.
 * 
 * These pixel will mainly light up different colors to allow the user to tell what 
 * Stage the sensor is at, or what the device has completed/is looking for
 * 
 * By: Lucien Lee
**************************************/
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
    #include <avr/power.h>
#endif

// Define the things to make the pixels run on the ESP32
#define LED_PIN 19
#define LED_LENGTH 5

// Determine if the pixels need to be shown while the system is processing things
int enableLEDFunctionality = 0;             // Defaulted to disabled...

// Define for the use of the on board pixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_LENGTH, LED_PIN, NEO_GRB + NEO_KHZ800);

// Code to set up the pixels to they work properly
void pixelSetup(){
    strip.begin();
    strip.setBrightness(50);
    strip.show();  // Initialize all pixels to 'off'
}

/*
*
*   On boot the system will enable the leds in order the order below
*   1 - Wifi:       connecting = white, connected = green
*   2 - OLED:       fail = red, connected = green
*   3 - GPS:        fail = red, retrieved = green
*   4 - Web page:   fail = red, connected green
*
*/

// This is a function to reset the current lights that are on the pixel strip
void pixelLightClear(){
    strip.clear();
}

// Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j = 0; j < 10; j++) {  // do 10 cycles of chasing
        for (int q = 0; q < 3; q++) {
            for (uint16_t i = 0; i < strip.numPixels(); i = i + 4) {
                strip.setPixelColor(i + q, c);  // turn every third pixel on
            }
            strip.show();

            // "wait" variable will be entered in milliseconds
            vTaskDelay(pdMS_TO_TICKS(wait));

            for (uint16_t i = 0; i < strip.numPixels(); i = i + 4) {
                strip.setPixelColor(i + q, 0);  // turn every third pixel off
            }
        }
    }
}