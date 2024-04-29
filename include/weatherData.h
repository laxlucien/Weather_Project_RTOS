/*********************************************************************
 * This file is for getting all of the relevant data from the web api
 * for displaying weather data. We will be using OpenWeather API.
 * 
 * This API will allow us 60 calls per minute, so we will query the server every 2 minutes to be safe.
 * The weather will display the current temperature and the forcast.
 * 
 * This will also give us web widgets to display the weather info to our website if we would like to.
 * 
 * By: Lucien Lee
**********************************************************************/
//Libraries to include
#include <WiFi.h>
#include "HTTPClient.h"
#include "ArduinoJson.h"

// Personal includes
#include "gps.h"

// Constant defined variables
const String openWeatherAPIKey = "4b300c7acd37fc75d4719cc99f74df74";
const String openWeatherWebsiteCall = "api.openweathermap.org";

HttpClient weather = HttpClient(wifi, openWeatherWebsiteCall);

// This function will format the whole openWeather website to get the parsed JSON
String makeWeatherWebsiteCallStream(){
    // This will concantenate all of the different strings that we need to make the full server call
    String temp = "https://api.openweathermap.org/data/2.5/weather?";
    temp.concat("lat=");
    temp.concat(String(glattitude));
    temp.concat("&lon=");
    temp.concat(String(glongitude));
    temp.concat("&appid=");
    temp.concat(openWeatherAPIKey);

    // Standard get call
    weather.get(temp);

    // Print out return data...
    int statusCode = weather.responseStatusCode();
    temp = weather.responseBody();

    // Serial.print("Connection status code: "); Serial.println(statusCode);
    // Serial.println("Response from the server: "); Serial.println(temp);

    return temp;
}

// This function is to convert the temperature that we recieve from kelvin to farenheight
float convertKToF(float number){
    float tempInFar = (number - 273.15) * 9 / 5 + 32;
    return tempInFar; 
}

// This is the function to recieve the parsed JSON string with all of the weather data
void parseIncommingWeather(String JSONData){
    JsonDocument ddoc;
    deserializeJson(ddoc, JSONData);
    Serial.println("--------------------------------------------------------");
    Serial.println("Printing weather data:");
    // This is getting the important data from the weather
    String mainWeather1 = ddoc["weather"][0]["main"];
    Serial.println(mainWeather1);
    mainWeather = mainWeather1;
    String mainWeatherDesc1 = ddoc["weather"][0]["description"];
    Serial.println(mainWeatherDesc1);
    mainWeatherDesc = mainWeatherDesc1;

    // This is getting the temperatures and converting them to farenhieght
    outsideTempF = ddoc["main"]["temp"];
    outsideTempF = convertKToF(outsideTempF);
    Serial.println(outsideTempF);

    outsideTempFeelsF = ddoc["main"]["feels_like"];
    outsideTempFeelsF = convertKToF(outsideTempFeelsF);
    Serial.println(outsideTempFeelsF);

    // Get the wind speed in meters per second
    windSpeed = ddoc["wind"]["speed"];
    Serial.print("Wind speed in m/s: "); Serial.println(windSpeed);

    String cityName1 = ddoc["name"];
    cityName = cityName1;
    Serial.println(cityName);

    String cityId1 = ddoc["id"];
    cityId = cityId1;
    Serial.print("CITYID: ");
    Serial.println(cityId);

    // Tell the rest of the program that the Weather data has been loaded, and can now be used
    weatherDataRecieved = 1;
    Serial.println("--------------------------------------------------------");

}

void queryWeatherDataTask(void* pvParam){
    Serial.println("Weather task started...");
    String JSONReturn;
    while(true){
        if(getGPSTrue == 0){
            JSONReturn = makeWeatherWebsiteCallStream();

            parseIncommingWeather(JSONReturn);
            // Delay the task for 2 minutes
            vTaskDelay(pdMS_TO_TICKS(120000));
        }else{
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}