#include <Arduino.h>

//#ifdef feather32u4 
//#ifdef ESP32
#include "WiFi.h"
//#else
//  #include <ESP8266WiFi.h>
//#endif
//#include <AdafruitIO_Wifi.h>

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){
  Serial.println(WiFi.macAddress());
  delay(1000);
}