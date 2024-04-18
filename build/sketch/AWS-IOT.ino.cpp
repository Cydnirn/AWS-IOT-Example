#include <Arduino.h>
#line 1 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
float h ;
float t;
 
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
 
#line 16 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
void connectAWS();
#line 60 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
void publishMessage();
#line 70 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
void messageHandler(char* topic, byte* payload, unsigned int length);
#line 81 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
void setup();
#line 87 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
void loop();
#line 16 "/home/rei/Programming/Arduino/AWS-IOT/AWS-IOT.ino"
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["message"] = "Hello, World!";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}
 
void setup()
{
  Serial.begin(115200);
  connectAWS();
}
 
void loop()
{
  publishMessage();
  client.loop();
  delay(1000);
}
