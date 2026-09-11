
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <WiFiMulti.h>
#include <Wire.h>
#include <EEPROM.h>

#define EEPROM_SIZE 100
#define LOCATION_CALIBRATION_FACTOR 0 //Float, requires 4 bytes of EEPROM
#define LOCATION_ZERO_OFFSET 10 //Must be more than 4 away from previous spot. int32_t, requires 4 bytes of EEPROM


bool settingsDetected = false; //Used to prompt user to calibrate their scale

//Create an array to take average of weights. This helps smooth out jitter.
#define AVG_SIZE 4
float avgWeights[AVG_SIZE];
byte avgWeightSpot = 0;
WiFiServer server(9999);   
String thing = "";
String thung = "";
const char* ssid = "Pixel";
const char* password = "abcviolin123";
String line = "";
int linebuff = 0;
String serverName = "http://172.16.143.133:5000/tile";
int payload = 0;
int httpResponseCode = 0;
NAU7802 myScale;
bool calibratescale;
HTTPClient http;
WiFiClient client;
float   calibrationFactor = -0.000929f;
float   cf = 0.04;
int32_t zeroOffset        = -4399203;
int32_t zero = 0;
int32_t averageRaw() {
  int32_t sum = 0;
int32_t getraw = 0;
  for (int i = 0; i < 32; i++) {
    while (!myScale.available()) delayMicroseconds(100);
    sum += myScale.getReading();
  }
  return sum / 32;
}


void setup(){
   EEPROM.begin(EEPROM_SIZE); 
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }
    Serial.println(WiFi.localIP());
    pinMode(1, INPUT);
server.begin();
    http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      Wire.begin();
      
  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Scale detected!");

     myScale.calculateZeroOffset(2);
       myScale.setSampleRate(NAU7802_SPS_80);
  myScale.setGain(NAU7802_GAIN_128);
  myScale.calibrateAFE();          
} 


void calibrateScale(void)
{
  Serial.println();
  Serial.println();
  Serial.println(F("Scale calibration"));

  // Serial.println(F("Setup scale with no weight on it. Press a key when ready."));
  // while (Serial.available()) Serial.read(); //Clear anything in RX buffer
  // while (Serial.available() == 0) delay(10); //Wait for user to press key
  // myScale.calculateZeroOffset(64); //Zero or Tare the scale. Average over 64 readings.
  // Serial.print(F("New zero offset: "));
  // Serial.println(myScale.getZeroOffset());

  // Serial.println(F("Place known weight on scale. Press a key when weight is in place and stable."));
  // while (Serial.available()) Serial.read(); //Clear anything in RX buffer
  // while (Serial.available() == 0) delay(10); //Wait for user to press key

  // Serial.print(F("Please enter the weight, without units, currently sitting on the scale (for example '4.25'): "));
  // while (Serial.available()) Serial.read(); //Clear anything in RX buffer
  // while (Serial.available() == 0) delay(10); //Wait for user to press key

  //Read user input
  //float weightOnScale = Serial.parseFloat();
float weightOnScale = 350.00;
  Serial.println();

  myScale.calculateCalibrationFactor(weightOnScale, 64); //Tell the library how much weight is currently on it 
  // Serial.print(F("New cal factor: "));
  // Serial.println(myScale.getCalibrationFactor(), 2);

  // Serial.print(F("New Scale Reading: "));
  // Serial.println(myScale.getWeight(), 2);


  
}


int32_t sum;

void getdata() {
  client = server.available();
thing = "";
         float tt = 0.0;
      //       for (int i = 0; i<20; i++) {
      //       while (!myScale.available()) delayMicroseconds(100);
      //       sum += myScale.getReading();
      //         if (i ==499) {
      //           //thing = thing + String(tt, 1) + "," + String(myScale.getWeight());
      //         } else {
      //       //thing = thing + String(tt, 1) + "," + String(myScale.getWeight()) + ",";
      //         }
      //       tt = tt + 0.01;
      //       delay(10);
            

      //  }
     // while(client.available() ==0) {
      averageRaw();
       //Serial.println(zero);
       int32_t raw = averageRaw();
       //Serial.println(raw);
       raw = (raw - zero)*cf;
       //thing = String(raw);
    Serial.println(String(raw));
      //  thing = thing + String(tt, 2) + "," + String(myScale.getWeight());
      //  tt = tt + 0.01;
      //  delay(10);
//}
}
void loop(){

  Serial.println(F("zero or start recording"));
  while (Serial.available()) Serial.read(); //Clear anything in RX buffer
  while (Serial.available() == 0) delay(10); //Wait for user to press key
  char p = Serial.read();
  if (p == 'z') {
linebuff = 4;

  }
  else if (p == 's') {
    linebuff = 3;
  }
    if (linebuff == 1){
      linebuff = 0;
            float kg  = 350.00;
      int32_t rawW = averageRaw();
      int32_t net  = rawW - zero;
      cf  = kg / (float)net;   // kg/count — Python applies this directly
      thing = "success";
      thung = cf;
      Serial.println(cf);

      http.POST("{\"uu\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");


    }
    else if (linebuff == 2) {
      Serial.println("get");
      thung = "thung";

      http.POST("{\"gg\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");
      Serial.println(thing);
      Serial.println("success?");
      linebuff = 0;
    }
    else if (linebuff == 3) {//stlp
      delay(100);
      thing = "";
      float tt = 0.0;
      float start = millis();
      //readSystemSettings();
      int32_t rawW;
      Serial.println("re");
      thung = "thung";
      while(1)
      {
        
      rawW=(averageRaw() - zero)*cf;
      tt = (millis()-start)/1000;
      thing = thing + String(tt, 1) + "," + String(rawW)+ ",";
      //         } else {

      delay(10);
      Serial.println("reading");
      if (Serial.available()){
        break;
      }
      }
      //Serial.println(myScale.getCalibrationFactor());
      //       float kg  = readFloat();
      // int32_t rawW = averageRaw();
      // int32_t net  = rawW - zero;
      // cf  = kg / (float)net;   // kg/count — Python applies this directly
      // thing = "success";
     // thung = cf;
   
      http.POST("{\"gg\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");
        Serial.println(thing);
      // linebuff = 0;
      linebuff =0;
    }
        else if (linebuff == 4) {
  //     Serial.println(myScale.getZeroOffset());
  // myScale.calculateZeroOffset(64);
        zero = averageRaw();
        Serial.println(zero);
      //myScale.setZeroOffset(zero);
  //thing = "zero";
  //thung = myScale.getZeroOffset();
        //http.POST("{\"uu\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");
      linebuff = 0;
    }
    

}
