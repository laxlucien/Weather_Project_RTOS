/****************************************************
 * This file will be responsible for getting the date and the time from the
 * internet so we can get it on our local machine, and on the ESP32
 * with more ease then doing so in other ways
 * 
 * By: Alphonse Crittenden
 *****************************************************/
// Library includes
#include <WiFi.h>
#include "time.h"

// Constant variables that we will need later on
// These are initialized to be in UTC time - since that is what the current server is on
const char *ntpServer = "pool.ntp.org";
float gmtOffsetSec = 0;
int daylightOffsetSec = 0;
bool timesBeenSet = false;

// Constant variable for setting the timezone offset
String geoApiKey = "76ff48cef2d140cf87650cce1116ca35";
const char *geoAPI = "api.geoapify.com";

// Store the current time so we can do things with it
const int timeBufferSize = 50;
char timeString[timeBufferSize];
char SQLTimeString[timeBufferSize];

// i make easy to retreive elsewhere
char dayOfWeek[10];
char month[10];
char dayOfMonth[3];
char year[5];
char hours[3];
char minutes[3];
char seconds[3];

// Server to retreive gmt offset and dst offset for time
HttpClient geo = HttpClient(wifi, geoAPI);

// This function will get and print out the local time to the device
void printTime()
{
   struct tm timeInfo;
   if (!getLocalTime(&timeInfo))
   {
      Serial.println("ERROR: Failed to get the local time...");
   }
   else
   {
      strftime(timeString, timeBufferSize, "%A, %b %d %Y %H:%M:%S", &timeInfo);

      strftime(dayOfWeek, sizeof(dayOfWeek), "%A", &timeInfo);
      strftime(month, sizeof(month), "%b", &timeInfo);
      strftime(dayOfMonth, sizeof(dayOfMonth), "%d", &timeInfo);
      strftime(year, sizeof(year), "%Y", &timeInfo);
      strftime(hours, sizeof(hours), "%H", &timeInfo);
      strftime(minutes, sizeof(minutes), "%M", &timeInfo);
      strftime(seconds, sizeof(seconds), "%S", &timeInfo);
      Serial.println(&timeInfo, "%A, %b %d %Y %H:%M:%S");
      if (currentScreen == 4) //|| currentScreen == 5)
      {
         displayTime();
      }
   }
}

// This function will get the current time and format it into a MYSQL based time string
void getSQLTimeString()
{
   struct tm timeInfo;
   if (!getLocalTime(&timeInfo))
   {
      Serial.println("ERROR: Failed to get the SQL time string...");
   }
   else
   {
      strftime(SQLTimeString, timeBufferSize, "%Y-%m-%d %H:%M:%S", &timeInfo);
      Serial.println(&timeInfo, "%Y-%m-%d %H:%M:%S");
   }
}

// This function will set the offset and the DST of the gps location
void setOffset()
{
   JsonDocument doc, timeZones;
   String contentType = "application/json";
   String serverPath = "/v1/geocode/reverse?lat=" + String(glattitude) + "&lon=" + String(glongitude) + "&apiKey=" + geoApiKey;

   geo.get(serverPath);

   // Read the status code and body of the response
   int statusCode = geo.responseStatusCode();
   String response = geo.responseBody();

   // Error checking api response to see if the getting offset values will fail
   DeserializationError error = deserializeJson(doc, response);
   if (error)
   {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
   }

   // Get the timezone array valaues from the api response
   JsonObject timezone = doc["features"][0]["properties"]["timezone"];

   gmtOffsetSec = timezone["offset_STD_seconds"];
   daylightOffsetSec = timezone["offset_DST_seconds"];

   if (getGPSTrue == 0 && timesBeenSet == false)
   {
      configTime(abs(gmtOffsetSec), abs(daylightOffsetSec) + 7200, ntpServer);
      vTaskDelay(pdMS_TO_TICKS(5000));
      printTime();
   }
}

// This is the function needed to set up the time for the ESP32
void setupTimeStamp()
{
   configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
   printTime();
}

// This fucntion is for getting the current time string so that we can set it equal to the current time and use it somewhere
void getUpdatedTimeString()
{
   // Update the current time
   printTime();
}

void handleDateTime()
{
   struct tm timeinfo;
   if (!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      server.send(500, "application/json", "{\"error\":\"Could not obtain time\"}");
      return;
   }

   char buffer[256];
   strftime(buffer, sizeof(buffer), "%A, %B %d %Y %H:%M:%S", &timeinfo);

   JsonDocument doc;
   doc["dayOfWeek"] = String(dayOfWeek);
   doc["month"] = String(month);
   doc["dayOfMonth"] = String(dayOfMonth);
   doc["year"] = String(year);
   doc["hours"] = String(hours);
   doc["minutes"] = String(minutes);
   doc["seconds"] = String(seconds);
   doc["fullDateTime"] = String(buffer);

   String output;
   serializeJson(doc, output);
   server.send(200, "application/json", output);
}