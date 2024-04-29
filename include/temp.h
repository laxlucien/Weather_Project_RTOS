/***********************************************
 * This is for getting data from the tempurature and humidity sensor
 * and relaying that information to other parts of the program
 * 
 * By: Dawson Burgess
***********************************************/
//Libraries to include
#include "Wire.h"
#include <math.h>

// Define the variables that we may need for the sensor data
#define HDC1080ADDRESS 0x40
#define MANDEVICEIDREG 0xFE
#define SERIALREG1 0xFB
#define SERIALREG2 0xFC
#define SERIALREG3 0xFD
#define DEVICEID 0xFF
#define TEMPERATUREREG 0x00
#define CONFIGREG 0x02
#define HUMIDREG 0x01

// Global tempurature define to get the temperature anywhere that we may need it
float temperatureCelcius = 0;
float temperatureFarenheit = 0;
float humidity_ = 0;

// Function to get and return the temperature in degrees celsius
float getTempC(){
    Wire.beginTransmission(HDC1080ADDRESS);
    Wire.write(TEMPERATUREREG);
    Wire.endTransmission();

    // Delay to allow the sensor to get the data needed
    vTaskDelay(pdMS_TO_TICKS(100));

    // Get the temperature value
    Wire.requestFrom(HDC1080ADDRESS, 2);
    uint16_t rawTemp = Wire.read() << 8 | Wire.read();

    float temperature = ((float)rawTemp / 65536.0) * 165.0 - 40.0;

    return temperature;
}

// Function to get and return the humidity
float getHumidity()
{
    Wire.beginTransmission(HDC1080ADDRESS);
    Wire.write(HUMIDREG);
    Wire.endTransmission();

    // Delay to allow the sensor to get the data needed
    vTaskDelay(pdMS_TO_TICKS(100));


    // Get the humidity value
    Wire.requestFrom(HDC1080ADDRESS, 2);
    uint16_t rawHum = Wire.read() << 8 | Wire.read();

    float humidity = ((float)rawHum / 65536.0) * 100.0;

    return humidity;
}

// Function to get the degrees in farenheight
float getTempF(float c){
    float far = c * 9 / 5 + 32;
    return far;
}

// Task to update the temperature information
void getTemperatureTask(void* pvParam){
    while(true){
        // read the temperature sensor
        temperatureCelcius = getTempC();
        Serial.print("The temperature in C is: "); Serial.println(temperatureCelcius);
        temperatureFarenheit = getTempF(temperatureCelcius);
        Serial.print("The temperature in F is: "); Serial.println(temperatureFarenheit);

        //humidity_ = getHumidity();
        //Serial.print("The humidity is: "); Serial.println(humidity_);
        if(currentScreen == 3)
        {
            displayRoomTemp();
        }

        // Delay the task so we aren't constantly running it (update every 5 seconds)
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void handleRoomTemp() {
    JsonDocument doc;
    doc["temp"] = String(temperatureFarenheit, 2);  // ensure you are actually updating these values

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

