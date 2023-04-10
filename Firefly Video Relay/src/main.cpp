#include <Arduino.h>
#include "WiFi.h"
#include "AsyncUDP.h"

const char * ssid = "Castillo";
const char * password = "yfkhf6rjpa";

#define MAX_PACKET_SIZE 1472

AsyncUDP udp;

const IPAddress remoteIP(192, 168, 1, 136);
uint8_t img_buff[80000];
uint8_t prev_packet = 0;
tcpip_adapter_if_t tcpip_if=TCPIP_ADAPTER_IF_MAX;

void send_image(const uint8_t * img_data, size_t len){
  uint8_t buffer[MAX_PACKET_SIZE];
  //Serial.println("1");
  size_t blen = sizeof(buffer);
  //Serial.println("2");
  size_t rest = len % blen;
  //Serial.println("attempting to send image");
  
  for (uint8_t i = 0; i < len / blen; ++i) {
    //Serial.println("3");
    memcpy(buffer, img_data + (i * blen), MAX_PACKET_SIZE);
    //Serial.println("attempting to send chunk");
    udp.writeTo((const uint8_t *) &buffer, (size_t) MAX_PACKET_SIZE, remoteIP, 5050, tcpip_if);
    //delayMicroseconds(100);
  }
  if (rest) {
    //Serial.println("4");
    uint8_t final_buff[rest];
    memcpy(final_buff, img_data + (len - rest), rest);
    //Serial.println("5");
    udp.writeTo((const uint8_t *) &final_buff, (size_t) MAX_PACKET_SIZE, remoteIP, 5050, tcpip_if);
    //Serial.println("6");

  }
  //udp.writeTo(img_data, 6, remoteIP, 5050);
}

void packet_cb(AsyncUDPPacket packet){
  //Serial.print("packet received");
  memcpy(img_buff + (prev_packet * MAX_PACKET_SIZE), packet.data(), packet.length());
  prev_packet++;
  if((packet.length() < MAX_PACKET_SIZE)){
    //Serial.println("Calling send_image function");
    send_image(img_buff, sizeof(img_buff));
    prev_packet = 0;
  }

}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println("WiFi Failed!");
    while(1) {
      delay(1000);
    }
  }

  Serial.println(WiFi.localIP());

  if(udp.listen(5005)) {
    Serial.println("UDP Connected!");
    udp.onPacket([](AsyncUDPPacket packet) {
      packet_cb(packet);
      /**Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      //Serial.write(packet.data(), packet.length());
      **/
      //img_data = packet.data();
      //Serial.print("Saved Image data");
      //Serial.println("Sending Image Data");
      //send_image((uint8_t *) img_data, packet.length());
      //char tst[] = "hello";
      //send_image((uint8_t *) tst, 5);
      //Serial.println();
      //reply to the client
      //packet.printf("Got %u bytes of data", packet.length());
    });
  }
}

void loop() {
  delay(1000);
  //udp.broadcastTo("Anyone here?", 1234);
}