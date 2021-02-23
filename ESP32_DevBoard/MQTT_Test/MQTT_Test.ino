#include <WiFi.h>
#include <PubSubClient.h>
#include "accessData.h"           // make sure u have an file called accessData.h 
                                  // including MQTT and WiFi configutation 

#define MQTT_PUBLISH_TOPIC "/iot/test"
#define MQTT_SUBSCRIBE_TOPIC "/iot/test"

const String DEVICE_NAME = "mqttTester";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi(){
  /** 
   *  Setting up the WiFi connection to defined network. 
   */
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // connection to WIFi Network 
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

void setup_mqtt(){
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(DEVICE_NAME.c_str())) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish(MQTT_PUBLISH_TOPIC, "new device connected");
      // ... and resubscribe
      client.subscribe(MQTT_SUBSCRIBE_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  /**
   * Define a function which is called when a new message recieved from the broker. 
   */
    Serial.println("--- new message from broker ---");
    Serial.print("Topic:    ");
    Serial.println(topic);
    Serial.print("Message:  ");
    Serial.write(payload, length);
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  setup_wifi();
  setup_mqtt();
}

void loop() {
  client.loop();              // start mqtt client loop
  client.publish(MQTT_PUBLISH_TOPIC, "test message");
  delay(5000);
}
