#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// MAC Addresses of L and R ESPs
// Use MAC Address of RECEIVER
uint8_t broadcastAddress[] = {0x90,0x38,0x0C,0xED,0x67,0x70}; // L
//uint8_t broadcastAddress[] = {0xC0,0x49,0xEF,0x44,0xDB,0x54}; // R

const int GreenLED = 25;
const int RedLED = 26;

//Variables of local temperature and query boolean
float outgoingTemp = 0.0;
bool incomingRequest = 0;

//String to store if sending data was successful
String success;

//Must match the receiver structure
typedef struct struct_message {
    float temp;
    bool request;
} struct_message;

// Create a struct_message called LocalTemp to hold local ESP temperature
struct_message LocalTemp;

//Create Struct_message called IncomingQuery to hold query boolean
struct_message IncomingQuery;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&IncomingQuery, incomingData, sizeof(IncomingQuery));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingRequest = IncomingQuery.request;
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
  esp_now_register_recv_cb(OnDataRecv);
  
  //Register Peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  //Add Peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Setup LED pins
  pinMode(GreenLED, OUTPUT);
  pinMode(RedLED, OUTPUT);

}

void loop()
{
  //Set local temp to broadcast
  if (outgoingTemp < 40)
  {
    outgoingTemp += 0.1;
  } else 
  {
    outgoingTemp = 0;
  }

  LocalTemp.temp = outgoingTemp;
  incomingRequest = IncomingQuery.request;
  Serial.println(incomingRequest);

  // Send message via ESP-NOW if FF node has requested
  if (IncomingQuery.request == 1){
    digitalWrite(GreenLED, HIGH);
    digitalWrite(RedLED, LOW);
    LocalTemp.request = 1;
    esp_now_send(broadcastAddress, (uint8_t *) &LocalTemp, sizeof(LocalTemp));
  } else {
    LocalTemp.request = 0;
    digitalWrite(GreenLED, LOW);
    digitalWrite(RedLED, HIGH);
  }

  delay(500);
  Serial.println(outgoingTemp);
}

