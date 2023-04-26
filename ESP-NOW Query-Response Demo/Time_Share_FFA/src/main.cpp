// Time Share/Split Testing
// Firefighter A:
// Node time splits its antenna usage between ESP-NOW, BTLE, and WiFi.
// Node must confirm successful query and receive data from sensors

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// MAC Addresses of L and R ESPs
// Use MAC Address of RECEIVER(s)
//uint8_t broadcastAddress_TL[] = {0x90,0x38,0x0C,0xED,0x67,0x70}; // Top Left - FFA
uint8_t broadcastAddress_TR[] = {0xC0,0x49,0xEF,0x44,0xDB,0x54}; // Top Right - 
uint8_t broadcastAddress_BL[] = {0xC8,0xF0,0x9E,0x2C,0x1B,0x48}; // Bottom Left - WiFi Node
uint8_t broadcastAddress_BR[] = {0x08,0xB6,0x1F,0x3D,0x3A,0x0 }; // Bottom Right - 

// Pins of debug LEDs
const int GreenLED = 25;
const int RedLED = 26;

//Variable of incoming temperature
float globalTemp;

//Variable to query temperature
bool request_temperature;

//String to store if sending data was successful
String success;

// *** MUST MATCH THE RECEIVER STRUCTURE ***
typedef struct struct_message {
    float temp;
    bool request;
} struct_message;

// Create a struct_message to hold incoming temp from other ESP
struct_message IncomingTemp;

// Create a struct_message to send to temp probe to request data
struct_message QueryTemp;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
    digitalWrite(RedLED, LOW);
  }
  else{
    success = "Delivery Fail :(";
    digitalWrite(RedLED, HIGH);
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&IncomingTemp, incomingData, sizeof(IncomingTemp));
  Serial.print("Bytes received: ");
  Serial.println(len);
  globalTemp = IncomingTemp.temp;
}

//Print the other ESP's temperature to serial monitor
void printTempReadings()
{
  Serial.print("Temperature: ");
  Serial.print(globalTemp);
  Serial.println();
}


void setup()
{
  Serial.begin(115200);
  //Set ESP as WiFi Station
  WiFi.mode(WIFI_STA);
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  //Register Peer
  memcpy(peerInfo.peer_addr, broadcastAddress_TR, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  //Add Peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Setup LED pins
  pinMode(GreenLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
}

void loop()
{
  IncomingTemp.request = 0;
  QueryTemp.temp = 0;
  QueryTemp.request = 1;
  digitalWrite(GreenLED, HIGH);
  esp_now_send(broadcastAddress_TR, (uint8_t *) &QueryTemp, sizeof(QueryTemp));
  delay(100);
  if(IncomingTemp.request = 1){
    printTempReadings();
    QueryTemp.request = 0;
    digitalWrite(GreenLED, LOW);
    esp_now_send(broadcastAddress_TR, (uint8_t *) &QueryTemp, sizeof(QueryTemp));
    delay(2000);
  }
}

