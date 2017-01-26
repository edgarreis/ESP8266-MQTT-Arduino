/*

 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay
  - on 2 switches the state of the relay

  - sends 0 on off relay
  - sends 1 on on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include <Stream.h>
#include "string.h"

const char* ssid          = ""
const char* password      = ""
const char* mqtt_server   = ""
const char* topicPot      = "dev/node1/potentiometer";
const char* topicTemp     = "dev/node1/temperature";
const char* topicHum      = "dev/node1/humidity";
const char* topicLed1     = "dev/node1/led1";
const char* topicButton1  = "dev/node1/button1";
const char* topicButton2  = "dev/node1/button2";
const char* topicButton3  = "dev/node1/button3";
//const char* outTopic      = "clients/cmd";
const char* outTopic      = "clients";

char AnalogValueChar[5];
char TempValueChar[5];
char HumValueChar[5];
char ButtonChar[5];
char PayloadChar[5];
char clearChar[5];

String AnalogValue;
String TempValue;
String HumValue;
String Button;
String PayloadString;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
long now = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void callback(char* topic, byte* payload, unsigned int length) {

  // Clear PayloadChar
  //PayloadChar = ";
  memcpy(PayloadChar, clearChar, 5);
  
  // Convert payload to String
  PayloadString = String((char*)payload);
  // Convert String to CharArray
  PayloadString.toCharArray(PayloadChar, 5);

  // Send to Serial
  if (strcmp(topic,topicButton1) == 0) {
      //Serial.print("1");
      Serial.println(PayloadChar); 
  }
  else if (strcmp(topic,topicButton2) == 0) {
      
      Serial.println(PayloadChar);
  }
  else if (strcmp(topic,topicButton3) == 0) {
      
      Serial.println(PayloadChar);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Node1 Connected");
      // ... and resubscribe
      client.subscribe(topicButton1);
      client.subscribe(topicButton2);
      client.subscribe(topicButton3);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
     }
   }
 }


//void establishContact() {
//  while (Serial.available() <= 0) {
//    delay(300);
//  }
//}

void setup() {
  
  Serial.begin(9600);
  //establishContact();  // send a byte to establish contact until receiver responds
  setup_wifi();        // Connect to wifi 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  now = millis();

  if (now - lastMsg > 1000) {
    lastMsg = now;
   
    if (Serial.available() > 0) {
    
      // Get Serial Incoming Byte
      //AnalogValue = Serial.readStringUntil(',');
      //Serial.read();
      Button = Serial.readStringUntil(',');
      Serial.read();
      TempValue = Serial.readStringUntil(',');
      Serial.read();
      HumValue = Serial.readStringUntil('\n');

      // Convert to Char
      //AnalogValue.toCharArray(AnalogValueChar, 5);
      Button.toCharArray(ButtonChar, 5);
      HumValue.toCharArray(HumValueChar, 5);
      TempValue.toCharArray(TempValueChar, 5);

      // Publish on MQTT Topic
      //client.publish(topicPot, AnalogValueChar);
      client.publish(topicLed1, ButtonChar);
      client.publish(topicHum, HumValueChar);
      client.publish(topicTemp, TempValueChar);
      
    }
  }
}
