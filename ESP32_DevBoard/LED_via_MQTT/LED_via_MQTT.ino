#include <WiFi.h>
#include <PubSubClient.h>         // more informations: https://pubsubclient.knolleary.net/api#connect
#include "accessData.h"           // make sure u have an file called accessData.h 
                                  // including MQTT and WiFi configutation 

#define MQTT_PUBLISH_TOPIC "/iot/actor"
#define MQTT_SUBSCRIBE_TOPIC "/iot/actor"

const String DEVICE_NAME = "espTester";
#define LED_PIN 16


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
  char* willTopic = MQTT_PUBLISH_TOPIC;
  byte willQoS = 0;
  boolean willRetain = true;
  String will_msg = "connection device=" + DEVICE_NAME + " connected=0";
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(DEVICE_NAME.c_str(), "/iot/test", willQoS, willRetain, will_msg.c_str())) {
      Serial.println("connected to mqtt Broker");
      //Once connected, publish an announcement...
      String con_msg = "connection device=" + DEVICE_NAME + " connected=1";
      send_string(con_msg);
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

void send_string(String msg){
  char buff[msg.length()];
  msg.toCharArray(buff, msg.length()+1);
  client.publish(MQTT_PUBLISH_TOPIC, buff);
}

void callback(char* topic, byte *payload, unsigned int length) {
  /**
   * Define a function which is called when a new message was recieved from the broker. 
   * While using MQTT to control the LED the message need to be decoded. 
   */
    // convert message to string 
    payload[length] = '\0';
    String msg = String((char *)payload);

    // decode the message
    if (msg.startsWith("control")){
      decodeControlMsg(msg);
    }
}

void decodeControlMsg(String msg){
  /**
   * Decode the message and if this device is referenced, call the requested function if exists. 
   */
  // decode message to measurement tag_set field_set
  int space_1_idx = msg.indexOf(" ");
  int space_2_idx = msg.indexOf(" ", space_1_idx + 1);
  
  String mesurement = msg.substring(0, space_1_idx);
  String tag_set = msg.substring(space_1_idx + 1, space_2_idx);
  String field_set = msg.substring(space_2_idx + 1, msg.length());
  
  // decoding tag_set
  String _device = "";
  String _function = "";

  while (tag_set.length() > 0){
    String tag = tag_set.substring(0, tag_set.indexOf(","));
    String key = tag.substring(0, tag_set.indexOf("="));
    String value = tag.substring(tag_set.indexOf("=") + 1, tag.length());

    // compare tag keys with possible keys
    if (key.equalsIgnoreCase("device")){ _device = value; }
    else if (key.equalsIgnoreCase("function")) { _function = value; }

    // remove current tag from string and continue
    if (tag_set.indexOf(",") == -1) { tag_set = ""; }
    else { tag_set = tag_set.substring(tag_set.indexOf(",") + 1, tag_set.length()); }
  }

  if (_device.equals(DEVICE_NAME)){
    
    if (_function.equals("print")) {
      Serial.print("Started function ");
      Serial.print(_function);
      Serial.print(" for device ");
      Serial.println(_device);
    }
    else if (_function.equals("turnOnLED")) {  turnOnLED();   }
    else if (_function.equals("turnOffLED")) {  turnOffLED();   }
  }
}

void turnOnLED(){
  digitalWrite(LED_PIN, HIGH);    
}

void turnOffLED(){
  digitalWrite(LED_PIN, LOW);  
}

void setup() {
  pinMode(LED_PIN, OUTPUT);     // Initialize the LED_PIN pin as an output
  Serial.begin(115200);
  delay(10);
  
  setup_wifi();
  setup_mqtt();
}

void loop() {
  client.loop();              // start mqtt client loop
}
