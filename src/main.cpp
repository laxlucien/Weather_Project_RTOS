/***************************************************************
 * Final Project for CS452
 * 
 * We will be creating a server to display weather data and complete other functions
 * based off of the GPS location that we recieve from our device.
 * 
 * For full documentation, visit the project github page
 * 
 * Started on: 04 - 23- 24
 * 
 * By: Dawson Burgess, Alphonse Crittenden, and Lucien Lee
*****************************************************************/
// Library includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "HttpClient.h"
#include "FS.h"
#include "SPIFFS.h"
#include <string>

// Wifi Constants
int port = 80;
int wifiStatus = 0;             // Set the status of being connected to wifi to false
WiFiClient wifi;
WebServer server(port);         // This is for the local server to be hosted on the ESP32

// Personal Includes
#include "wifiConnect.h"
#include "weatherData.h"

// This function will start the html web page (stored in progmem)
void handleRoot(){
    File file = SPIFFS.open("/index.html", "r"); // Open Home page infile from system flash 
    
    // Checking if the file was found in the file system
    if(!file){
      Serial.printf("Failed to open file for reading.");
      return;

    }

    String s = file.readString();              // Read the contents of the HTML
    file.close();

    s.replace("TEMP_CITY_ID", cityId);

    server.send(200, "text/html", s);   //Send the web page

    strip.setPixelColor(4, strip.Color(0, 255, 0));
    strip.show();

    
}

// Update the web server so we can view the web server in reat time
void updateWebsiteTask(void* pvParam){
    while(true){
        if(weatherDataRecieved ==1)
        {
            server.handleClient();
        }
        server.on("/gps", HTTP_GET, handleGPSData);
        server.on("/dateTime", HTTP_GET, handleDateTime);
        server.on("/lightSensor", HTTP_GET, handleLightSensor);
        server.on("/roomTemp", HTTP_GET, handleRoomTemp);

        // Delay to allow other things to run in the background
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Task to setup the wifi Connection
void wifiConnectionTask(void* pvParam){
    int wifiConnectionComplete = 0;
    while(true){
        if(wifiStatus == 0){
            // Connect to wifi...

            // Set up the Access point
            WiFi.mode(WIFI_AP);
            WiFi.softAP(SECRET_SSID, SECRET_PASS);

            // Connect to the wifi...
            WiFi.mode(WIFI_STA);
            WiFi.begin(SECRET_SSID, SECRET_PASS);
            while(WiFi.status() != WL_CONNECTED){
                delay(1000);
                Serial.println("Connecting...");
                strip.setPixelColor(1, strip.Color(0, 255, 0));
                strip.show();
            }
            if(WiFi.status() == WL_CONNECTED){
                Serial.println("WIFI Connected!");
                strip.setPixelColor(1, strip.Color(0, 255,0));
                strip.show();
                wifiStatus = 1;
            }

            // Server handle calls
            server.on("/", handleRoot);                         //This is display page
            
            // Start server
            Serial.println(WiFi.localIP());
            server.begin();
            Serial.println("The web server has been established...");
        }else{
            // May want to kill the task after this is completed...

            //Start the other Tasks here
            if(wifiStatus == 1 && wifiConnectionComplete == 0){
                // Create the task for allowing the GPS Coordinates to be read
                xTaskCreate(gpsInfoTask, "gpsInfoTask", 4096, NULL, 1, NULL);
                xTaskCreate(queryWeatherDataTask, "queryWeatherDataTask", 4096, NULL, 1, NULL);
                xTaskCreate(updateWebsiteTask, "updateWebsiteTask", 4096, NULL, 1, NULL);

                // Start the display here
                serverAddress = WiFi.localIP().toString();
                clearScreen();
                //vTaskDelay(pdMS_TO_TICKS(3000));
                //displayWifiStatus(true);
                sendTextToDisplay(serverAddress, 2, 2, 0);
                wifiConnectionComplete = 1;
            }

            // Remind the user the local IP every 10 seconds
            Serial.println(WiFi.localIP());

            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }
}

// Setup Function
void setup(){
    // Start the serial interface for debugging purposes
    Serial.begin(115200);

    // Add a delay so we can see what is being output to the serial monitor
    delay(5000);

    GPSSerial.begin(9600);
    Wire.begin();
    pixelSetup();

    // Error check for acces to mounted files
    if (!SPIFFS.begin(true))
   {
      Serial.println("An error has occurred while mounting SPIFFS");
      return;
   }

    // Start the light sensor
    if(tsl.begin()){
        Serial.println("Found a light sensor...");\
    }else{
        Serial.println("ERROR: There was an issue getting the light sensor started...");
    }

    displayLightSensorInfo();
    configureLightSensor();

    // Start the oled display
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLEDDISPLAYADDR)){
        Serial.println("Display setup failed...");
        strip.setPixelColor(2, strip.Color(0, 0, 255));
        strip.show();
    }else{
        Serial.println("Display is working!!!");
        strip.setPixelColor(2, strip.Color(255, 0, 0));
        strip.show();
    }

    // Start the wifi connection task
    xTaskCreate(wifiConnectionTask, "wifiConnectionTask", 4096, NULL, 1, NULL);

    // Make sure it initialize the buttons
    buttonSetup();
}

// Loop function
void loop(){
    // This function will not really be used, just here for compilation purposes...
}