#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Wire.h> 
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
const char* ssid = "Pixel";
const char* password = "abcviolin123";
String serverName = "http://10.43.65.133:5000/tile";
int payload = 0;
int httpResponseCode = 0;
HTTPClient http;
NAU7802 myScale;
void setup(){
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }
    pinMode(1, INPUT);

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    
    http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      
  Wire.begin();

  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Scale detected!");

     
}

void loop(){
          

      //String serverPath = serverName + "?";
      
      // Your Domain name with URL path or IP address with path

      //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      //int httpResponseCode = http.POST("{\"name\":\"Alice\",\"city\":\"New York\"}");

      //String httpRequestData = "api_key=" + String(random(40));
      //int httpResponseCode = http.POST(httpRequestData);
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      //int httpResponseCode = http.POST("hellooo");
      //String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}"; 
       //int httpResponseCode = http.POST(httpRequestData);
       //String thing = String(analogRead(1));
       String thing = "";
       float tt = 0.0;
            for (int i = 0; i<500; i++) {
              if (i ==499) {
                thing = thing + String(tt, 1) + "," + String(myScale.getReading());
              } else {
            thing = thing + String(tt, 1) + "," + String(myScale.getReading()) + ",";
              }
            tt = tt + 0.01;
            delay(10);
       }
       httpResponseCode = http.POST("{\"uu\":\"" + thing + "\"}");
       Serial.println(("{\"uu\":\"" + thing + "\"}"));
    //int32_t currentReading = myScale.getReading();
    //Serial.print("Reading: ");
    //Serial.println(currentReading);
      if (httpResponseCode>0) {

        
        payload = http.getString().toInt();
         if (payload>0) {
          
          //httpResponseCode = http.POST("{\"uu\":\" + thing}");
          
          //put sensor spi data or wtvr
         }

      }
            else {
       // httpResponseCode = http.POST("{\"test\":\"test\"}");
        
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      } 
              String jsonthingy = http.getString();
              
        JSONVar myObject = JSON.parse(jsonthingy);
        Serial.println(myObject);
        Serial.println(payload);
       

      delay(100);
}
