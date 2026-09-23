
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Wire.h>






String data = "";
String dataType = "";
const char* ssid = "PhoneName";
const char* password = "password";
String line = "";
int linebuff = 0;
String serverName = "https://172.16.143.133:5000/data"; //points to "soil_data" route in server
int httpResponseCode = 0;
NAU7802 myScale;
HTTPClient http;
WiFiClient client;
float   cf = 0.04; //Calibration factor - we found that it was consistently 0.04, so we initialized it to facilitate data collection
int32_t zero = 0; //This is the zero offset, we cannot set this as a constant because it fluctuates slightly whenver the scale is used
int32_t averageRaw() { //take the average of 32 samples
  int32_t sum = 0;
  for (int i = 0; i < 32; i++) {
    while (!myScale.available()) delayMicroseconds(100);
    sum += myScale.getReading();
  }
  return sum / 32;
}


void setup(){
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }
    Serial.println(WiFi.localIP());
    pinMode(1, INPUT);
    http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      Wire.begin();
      
  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Please check wiring.");
    while (1);
  }
  Serial.println("Scale detected!");

  myScale.setSampleRate(NAU7802_SPS_80);
  myScale.setGain(NAU7802_GAIN_128);
  myScale.calibrateAFE();          
} 




int32_t sum;

void loop(){

  Serial.println(F("zero or start recording"));
  while (Serial.available()) Serial.read(); //Clear anything in RX buffer
  while (Serial.available() == 0) delay(10); //Wait for user to press key
  char p = Serial.read();
  if (p == 'z') { //zero scale
linebuff = 1;

  }
  else if (p == 's') { //start recording, press key again to stop
    linebuff = 2;
  }


    if (linebuff == 1) {
      delay(100);
      data = "";
      float tt = 0.0;
      float start = millis(); //gets current time in milliseconds
      int32_t rawW;
      dataType = "getdata";
      while(1)
      {
        while (!myScale.available()) delayMicroseconds(10);
        
      rawW=(myScale.getReading() - zero)*cf;
      tt = (millis()-start)/1000; //find how much time has passed and convert into seconds
      data = data + String(tt, 1) + "," + String(rawW)+ ",";

      delay(10);
      Serial.println("reading");
      if (Serial.available()){ //Stop recording if any key is pressed
        break;
      }
      }
  
   
      http.POST("{\"gg\":\"" + data + "\",\"ff\":\"" + dataType + "\"}");
        Serial.println(data);
      linebuff =0; //reset buffer
    }
        else if (linebuff == 2) {
        zero = averageRaw(); //Hold device in the air with tip pointed towards the ground, make sure tip is not touching anything
        Serial.println(zero);
      linebuff = 0; //reset buffer
    }
    

}
