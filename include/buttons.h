/*******************************************
 * This file is for driving tasks that the buttons may have.
 *
 * The buttons have been outlined to preform the following tasks:
 * Button 1: Acts as a light switch, allowing user to manually control the LEDs
 * Button 2: Serves as a kill switch for the entire system, providing emergency shutoff function
 * Button 3: Allows the user to cycle through different displays on the LED screen.
 *
 * 
 * By: Lucien Lee
 ********************************************/
// Libraries to include
#include <Arduino.h>

// Personal Libraries to include
#include "pixel.h"
#include "oledDisplay.h"
#include "getTime.h"
#include "lightSensor.h"
#include "index.h"
#include "temp.h"
#include <string>

// Default buttons for the ESP32
// GP 19
#define BUTTON1 18
// GP 9
#define BUTTON2 15
// GP 8
#define BUTTON3 32

// Constant global for the local website IP
String serverAddress = "EMPTY";

// Setup the buttons to get the correct data from them when it is needed
void buttonSetup()
{
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(BUTTON3, INPUT);

    Serial.println("Buttons have been initialized...");
}

// Task to read the buttons as they are pressed
void getButtonPressedTask(void *pvParam)
{
    String temp;
    while (true)
    {
        if (digitalRead(BUTTON1) == 1)
        {
            Serial.println("BUTTON1 has been pressed...");

            // Toggle the light functions on so when tasks are running the leds also light up and do various things
            if (enableLEDFunctionality == 0)
            {
                // LED pixels are currently disabled - so we need to enable them
                enableLEDFunctionality = 1;
            }
            else
            {
                // LED pixels are currently enabled - so we need to disable them
                enableLEDFunctionality = 0;
            }

            // Delay to only do that thing once...
            vTaskDelay(pdMS_TO_TICKS(150));
        }

        if (digitalRead(BUTTON2) == 1)
        {
            Serial.println("BUTTON2 has been pressed...");

            // This button will kill the entire system and stop all of the tasks that are currently executing
            vTaskEndScheduler();

            // Delay to only do that thing once... (This isn't really needed here but oh well)
            vTaskDelay(pdMS_TO_TICKS(150));
        }

        if (digitalRead(BUTTON3) == 1)
        {
            Serial.println("BUTTON3 has been pressed...");

            // Display a different information button when this is toggled
            clearScreen();
            if (currentScreen == TOTALSCREENDISPLAYS )
            {
                // reset to the beginning of the screen loop
                currentScreen = 0;
                strip.fill(strip.Color(0,0,0));
                strip.show();
            }
            else
            {
                currentScreen++;
            }

            switch (currentScreen)
            {
            case 0:
                // Display the webserver IP address
                if(weatherDataRecieved == 1)
                {
                    displayWifiStatus(true);
                }else{
                    displayWifiStatus(false);
                }
                
                break;
            case 1:
                // Display the light level on the display
                Serial.println("Getting light sensor value");
                displayLux();
                break;
            case 2:
                // Display the weather data
                if (weatherDataRecieved == 1)
                {
                    String WINDSPEED = "";
                    WINDSPEED.concat(windSpeed);
                    WINDSPEED.concat(" m/s");

                    Serial.println("Getting weather info");
                    displayWeather();
                    sendDataToDisplay((String)cityName, 1.75, 62, 0);
                    sendDataToDisplay("Wind speed", 1, 62, 10);
                    sendDataToDisplay("  " + WINDSPEED, 1, 62, 20);
                    display.drawLine(62, 28, 127, 28, WHITE);
                    sendDataToDisplay((String)outsideTempF, 2, 62, 32);
                    //sendDataToDisplay(, 1, 62, 48);
                    String scrollData = ("Feels " + String(outsideTempFeelsF) + "-" + mainWeatherDesc + ". ");
                    //scrollData.concat();
                    //String temp = );
                    //scrollData.concat(".  "); scrollData.concat(temp); scrollData.concat("F. "); 
                    sendDataToDisplay(scrollData, 1, 0, 56);
                    display.startscrollleft(0x07, 0x07);
                    display.display();
                    //display.stopscroll();
                }
                else
                {
                    Serial.println("The weather data has not been recieved, so we can not provide the information...");
                    displayWeatherAlert();
                }
                break;
            case 3:
                display.stopscroll();
                // Room temperature from temp sensor
                Serial.println("Getting the temperature data");
                displayRoomTemp();
                break;
            case 4:
                // Display current time based on location
                Serial.println("Displaying current time");
                displayTime();
                break;
            case 5:
                clearScreen();
                strip.rainbow();
                strip.show();
                break;
            default:
                Serial.println("ERROR: The screen requested could not be displayed...");
                break;
            }
            Serial.println(currentScreen);
            // Delay to only do that thing once...
            vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Wait between button presses to allow other tasks to run in the background
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}