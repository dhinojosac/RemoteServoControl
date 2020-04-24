#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "credentials.h"

#define ID "RSC0000" // id to indetify device

Servo servo;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//mqtt topics
char subs_topic[25] = "";
char pub_topic[25] = "";



void setup() {
  sprintf(subs_topic,"%s/servo",ID);
  sprintf(pub_topic,"%s/stats",ID);

  
  servo.attach(5);                // Attach servo to D1 or pin 5
  pinMode(BUILTIN_LED, OUTPUT);   // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  servo.write(0);  //set servo to 0

  Serial.println(subs_topic);
  Serial.println(pub_topic);
}
 
void setup_wifi() {
  Serial.print("MAC:");
  Serial.println(WiFi.macAddress()); // print MAC 
 
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
  
  servo.write(10); //set servo to 10
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  char command[20] = "";
  memcpy(command, payload, length);

  
  //servo.write(atoi((char*)command));
  servo.writeMicroseconds(atoi((char*)command));

  Serial.print(">");
  Serial.print(command);
  Serial.print("<\n");
  Serial.print("Length:");
  Serial.println(length);
  
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      
      // Once connected, publish an announcement...
      client.publish(pub_topic,"Connected");
      
      // ... and resubscribe      
      client.subscribe(subs_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
  }
}
 
void loop() {
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  
  if (now - lastMsg > 60000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "stat #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);   

    client.publish(pub_topic, msg);
  }
  
}
