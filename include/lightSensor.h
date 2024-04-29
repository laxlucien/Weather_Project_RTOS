/*******************************************
 * This is the header file to deal with the different light sensor operations...
 * 
 * Additional led strips can be hooked up to the board for increased functionality with 
 * the light sensor.
 * 
 * An example use case would be if the light threshhold gets below a certain level, then you can toggle the led 
 * strips on to a certain value.
 * 
 * By: Dawson Burgess
********************************************/
// Libaraies to include
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

// Define the sensor identifier...
#define LIGHTSENSOR 2591

// Define the light sensor object...
Adafruit_TSL2591 tsl = Adafruit_TSL2591(LIGHTSENSOR);

// Define the light sensor lux data variable
float lightSensorLux = 0;

// This function will tell us some basic information about the sensor that we are using
void displayLightSensorInfo(){
    // Get the data of the sensor
    sensor_t sensor;
    tsl.getSensor(&sensor);

    // One big print statement to give us the information that we would like to recieve
    Serial.println(F("----------------------------------------------"));
    Serial.print(F("Sensor:         ")); Serial.println(sensor.name);
    Serial.print(F("Driver Version: ")); Serial.println(sensor.version);
    Serial.print(F("Unique ID:      ")); Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:      ")); Serial.println(sensor.max_value);
    Serial.print(F("Min Value:      ")); Serial.println(sensor.min_value);
    Serial.print(F("Resolution:     ")); Serial.println(sensor.resolution);
    Serial.println(F("----------------------------------------------\n\n"));
}

// This function will be for configuring the sensor to our light environment around us...
void configureLightSensor(){
    // This will set our sensor gain to 25x, which is the medium sensor configuration that we can have
    tsl.setGain(TSL2591_GAIN_MED);

    // This will define how long we give the sensor to sense light from the outside world
    // We are in a fairly light sensitive envioronment, so we don't need a ton of time for this
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS); // We will do a medium integration time
}

// This function will show the the reading of the IR spectrum and convert it to lux
void readSensor(){
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;

    // Read the top 16 bits of the IR, and the bottom 16 bits of the full spectrum
    ir = lum >> 16;
    full = lum & 0xFFFF;

    // Store the light sensor data
    lightSensorLux = tsl.calculateLux(full, ir);
    Serial.print("Lux: "); Serial.println(lightSensorLux);
    if (currentScreen == 1)
      {
         displayLux();
      }
}

// This function will need to get called in the startup of the main program so the light sensor works
void setupLightSensor(){
    Serial.println(F("Starting the Adafruit TSL2591 Light Sensor"));

    if(tsl.begin()){
        Serial.println(F("Found a light sensor..."));
    }else{
        Serial.println(F("ERROR: There was an issue getting the light sensor started..."));
    }

    // Display the sensor details
    displayLightSensorInfo();

    // Properly configure the sensor for our use case
    configureLightSensor();
}

// Test the light sensor to make sure that it is working properly
void testLightSensorTask(void* pvParam){
    while(true){
        readSensor();

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void handleLightSensor()
{
    JsonDocument doc;

    doc["lux"] = String(lightSensorLux, 2);

    String output;
    serializeJson(doc, output);

    server.send(200, "application/json", output);
}