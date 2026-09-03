
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
               

} 

void recordSystemSettings(void)
{
  //Get various values from the library and commit them to NVM
  EEPROM.put(LOCATION_CALIBRATION_FACTOR, myScale.getCalibrationFactor());
  EEPROM.put(LOCATION_ZERO_OFFSET, myScale.getZeroOffset());

  EEPROM.commit(); //Some platforms need this. Comment this line if needed
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
float weightOnScale = 70.00;
  Serial.println();

  myScale.calculateCalibrationFactor(weightOnScale, 64); //Tell the library how much weight is currently on it 
  // Serial.print(F("New cal factor: "));
  // Serial.println(myScale.getCalibrationFactor(), 2);

  // Serial.print(F("New Scale Reading: "));
  // Serial.println(myScale.getWeight(), 2);
recordSystemSettings();

  
}

void readSystemSettings(void)
{
  float settingCalibrationFactor; //Value used to convert the load cell reading to lbs or kg
  int32_t settingZeroOffset; //Zero value that is found when scale is tared

  //Look up the calibration factor
  EEPROM.get(LOCATION_CALIBRATION_FACTOR, settingCalibrationFactor);
  if (settingCalibrationFactor == 0xFFFFFFFF)
  {
    settingCalibrationFactor = 1.0; //Default to 1.0
    EEPROM.put(LOCATION_CALIBRATION_FACTOR, settingCalibrationFactor);
  }

  //Look up the zero tare point
  EEPROM.get(LOCATION_ZERO_OFFSET, settingZeroOffset);
  if (settingZeroOffset == 0xFFFFFFFF)
  {
    settingZeroOffset = 0; //Default to 0 - i.e. no offset
    EEPROM.put(LOCATION_ZERO_OFFSET, settingZeroOffset);
  }

  //Pass these values to the library
  myScale.setCalibrationFactor(settingCalibrationFactor);
  myScale.setZeroOffset(settingZeroOffset);

  // settingsDetected = true; //Assume for the moment that there are good cal values
  // if (settingCalibrationFactor == 1.0 || settingZeroOffset == 0)
  //   settingsDetected = false; //Defaults detected. Prompt user to cal scale.
}

void getdata() {
         float tt = 0.0;
            for (int i = 0; i<20; i++) {
              if (i ==499) {
                thing = thing + String(tt, 1) + "," + String(myScale.getWeight());
              } else {
            thing = thing + String(tt, 1) + "," + String(myScale.getWeight()) + ",";
              }
            tt = tt + 0.01;
            delay(10);
            

       }
       Serial.println(thing);
}
void loop(){

WiFiClient client = server.available();
       //Serial.println(myScale.getReading());

// if (client) {
//   }
  //while (1) {
    while (client.available()) {
      //uint8_t data = client.read();
      // do something with char
      //Serial.println(client.read());
   line = client.readStringUntil('\r');
       Serial.println(line);

   if (line=="calibrate") {
    Serial.println("pppp");
    linebuff = 1;
    break;
   }
   else if(line == "getdata") {
    Serial.println("qqqq");
    linebuff = 2;
    break;
   }
      else if(line == "checkcal") {
    Serial.println("rrrr");
    linebuff = 3;
    break;
   }
         else if(line == "zero") {
    Serial.println("rrrr");
    linebuff = 4;
    break;
   }
else {
  Serial.println("error");
linebuff = 0;
break;
}
   
    }
    if (linebuff == 1){
      linebuff = 0;
Serial.println("post");
calibrateScale();
Serial.println(myScale.getCalibrationFactor());
//thing = myScale.getCalibrationFactor();
thing = "success";
thung = myScale.getCalibrationFactor();
http.POST("{\"uu\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");

    }
    else if (linebuff == 2) {
      Serial.println("get");
      thung = "thung";
      getdata();
      http.POST("{\"gg\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");
      Serial.println("success?");
      linebuff = 0;
    }
    else if (linebuff == 3) {
      
      readSystemSettings();
      Serial.println(myScale.getCalibrationFactor());
      thing = "success";
      thung = myScale.getCalibrationFactor();
      http.POST("{\"uu\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");
      linebuff = 0;
    }
        else if (linebuff == 4) {
      Serial.println(myScale.getZeroOffset());
  myScale.calculateZeroOffset(64);
  thing = "success";
  thung = myScale.getZeroOffset();
        http.POST("{\"uu\":\"" + thing + "\",\"ff\":\"" + thung + "\"}");
      linebuff = 0;
    }
    

//     if (server.hasClient()) {
//       return;
//     }
  //}


//             const uint16_t port = 5000;
//   const char *host = "172.16.143.149";  // ip or dns
//   WiFiClient client;

//     if (!client.connect(host, port)) {
//     Serial.println("Connection failed.");
//     Serial.println("Waiting 5 seconds before retrying...");
//     delay(5000);
//     return;
//   }

//        String thing = String(analogRead(1));
       
      
//          client.print("GET /index.html HTTP/1.1\n\n");

//   int maxloops = 0;

//   //wait for the server's reply to become available
//   while (!client.available() && maxloops < 1000) {
//     maxloops++;
//     delay(1);  //delay 1 msec
//   }
//   if (client.available() > 0) {
//     //read back one line from the server
//     String line = client.readStringUntil('\r');
//     Serial.println(line);
//   } else {
//     Serial.println("client.available() timed out ");
//   }

//   Serial.println("Closing connection.");
//   client.stop();

//   Serial.println("Waiting 5 seconds before restarting...");
//   delay(5000);

//       delay(100);
}
