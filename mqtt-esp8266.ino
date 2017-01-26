/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid =  ""
const char* password = ""
const char* mqtt_server = ""
const char* topicPot = "dev/node1/potentiometer";
const char* topicTemp = "dev/node1/temperature";
const char* topicHum = "dev/node1/humidity";
const char* topicLed1 = "dev/node1/led1";


char AnalogValueChar[5];
char TempValueChar[5];
char HumValueChar[5];
char ButtonChar[5];

String AnalogValue;
String TempValue;
String HumValue;
String Button;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
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
    //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe(topicTemp);
      //client.subscribe(topicLed1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void establishContact() {
  while (Serial.available() <= 0) {
    delay(300);
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  Serial.begin(9600);
  establishContact();  // send a byte to establish contact until receiver responds
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

/*  long now = millis();

  if (now - lastMsg > 1000) {
    lastMsg = now;
   
    if (Serial.available() > 0) {
*/    
      // get incoming byte:
      AnalogValue = Serial.readStringUntil(',');
      Serial.read();
      TempValue = Serial.readStringUntil(',');
      Serial.read();
      HumValue = Serial.readStringUntil(',');
      Serial.read();
      Button = Serial.readStringUntil('\n');
  
      AnalogValue.toCharArray(AnalogValueChar, 5);
      TempValue.toCharArray(TempValueChar, 5);
      HumValue.toCharArray(HumValueChar, 5);
      Button.toCharArray(ButtonChar, 5);
/*  
      Serial.print("Publish message: Analog: ");
      Serial.print(AnalogValueChar);
      Serial.print("  --- Button : ");
      Serial.print(ButtonChar);
      Serial.print("\n");  
*/    
      client.publish(topicPot, AnalogValueChar);
      client.publish(topicTemp, TempValueChar);
      client.publish(topicHum, HumValueChar);
      client.publish(topicLed1, ButtonChar);
    
    //}
  //}
}



