#include <Arduino.h>
//Wifi
#include <ESP8266WiFi.h>          
//Wifi manager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//particle sensor
#include <Seeed_HM330X.h>
//json
#include <ArduinoJson.h>
//MQtt
#include <PubSubClient.h>
//sleep modes
#include "user_interface.h"







WiFiClient wifiClient;
PubSubClient client(wifiClient);


/*-----Change Mqtt values here-------*/
IPAddress server(192, 168, 178, 75);  //mqtt server
const char *user = "mqtt";            //mqtt username
const char *pass = "test123";         //password
/*----------------------------------*/

HM330X sensor;
uint8_t buf[30];

#define MQTTTopic "homeassistant/sensor/air"
#define MQTT_UNIQUE_IDENTIFIER WiFi.macAddress() 

WiFiManager wifiManager;

uint16_t pm1;
uint16_t pm25;
uint16_t pm10;


const char *str[] = {"sensor num: ", "PM1.0 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                     "PM2.5 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                     "PM10 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                     "PM1.0 concentration(Atmospheric environment,unit:ug/m3): ",
                     "PM2.5 concentration(Atmospheric environment,unit:ug/m3): ",
                     "PM10 concentration(Atmospheric environment,unit:ug/m3): ",
};


HM330XErrorCode print_result(const char *str, uint16_t value) {
    if (NULL == str) {
        return ERROR_PARAM;
    }
    Serial.print(str);
    Serial.println(value);
    return NO_ERROR;
}

/*parse buf with 29 uint8_t-data*/
HM330XErrorCode parse_result(uint8_t *data) {
    uint16_t value = 0;
    if (NULL == data)
        return ERROR_PARAM;
    for (int i = 2; i < 5; i++) {
        value = (uint16_t) data[i * 2] << 8 | data[i * 2 + 1];
        if(i==2){
           pm1 = value;
        }
        if(i==3){
          pm25 = value;
        }
        if(i==4){
           pm10 = value;
        }
        print_result(str[i - 1], value);

    }

    return NO_ERROR;
}


/*
  Sends the 3 sensor values as a json via mqtt
*/
void sendStatus(){
  StaticJsonDocument<128> JSONencoder;
  Serial.println("Sending Status");
  JSONencoder["pm1"] = (int)pm1,
  JSONencoder["pm25"] = (int)pm25,
  JSONencoder["pm10"] = (int)pm10;
  uint8_t JSONmessage[128];
  size_t n = serializeJson(JSONencoder, JSONmessage);
  client.publish("homeassistant/sensor/air/state", JSONmessage, n, true);
        
}
/*
  Message for Homeassistant autodiscovery 
  for PM1
*/
void send1Config(){
 DynamicJsonDocument JSONencoder(4096);
  JSONencoder["name"] = "air 1",
  JSONencoder["device_class"] = "pm1",
  JSONencoder["unit_of_measurement"] = "µg/m³",
  JSONencoder["dev"]["ids"] = MQTT_UNIQUE_IDENTIFIER,
  JSONencoder["dev"]["mf"] = "Robin47113",
  JSONencoder["dev"]["mdl"] = "1.0",
  JSONencoder["dev"]["name"] = "air",
  JSONencoder["state_topic"] = "homeassistant/sensor/air/state",
  JSONencoder["value_template"] = "{{ value_json.pm1}}" ;

  size_t n = measureJson(JSONencoder);
                  if (client.beginPublish("homeassistant/sensor/air1/config", n, true) == true) {
                   // SERIAL_DEBUG_LN("Configuration Publishing Begun")
                    if (serializeJson(JSONencoder, client) == n){
                     //   SERIAL_DEBUG_LN("Configuration Sent")
                    }
                    if (client.endPublish() == true) {
                       // SERIAL_DEBUG_LN("Configuration Publishing Finished")
                    }
                } else {
                  //  SERIAL_DEBUG_LN("Error sending Configuration")
                }
}

/*
  Message for Homeassistant autodiscovery 
  for PM2.5
*/
void send2Config(){
 DynamicJsonDocument JSONencoder(4096);
  JSONencoder["name"] = "air 2.5",
  JSONencoder["device_class"] = "pm25",
  JSONencoder["unit_of_measurement"] = "µg/m³",
  JSONencoder["dev"]["ids"] = MQTT_UNIQUE_IDENTIFIER,
  JSONencoder["dev"]["mf"] = "Robin47113",
  JSONencoder["dev"]["mdl"] = "1.0",
  JSONencoder["dev"]["name"] = "air",
  JSONencoder["state_topic"] = "homeassistant/sensor/air/state",
  JSONencoder["value_template"] = "{{ value_json.pm25}}" ;

  size_t n = measureJson(JSONencoder);
                  if (client.beginPublish("homeassistant/sensor/air2/config", n, true) == true) {
                   // SERIAL_DEBUG_LN("Configuration Publishing Begun")
                    if (serializeJson(JSONencoder, client) == n){
                     //   SERIAL_DEBUG_LN("Configuration Sent")
                    }
                    if (client.endPublish() == true) {
                       // SERIAL_DEBUG_LN("Configuration Publishing Finished")
                    }
                } else {
                  //  SERIAL_DEBUG_LN("Error sending Configuration")
                }
}

/*
  Message for Homeassistant autodiscovery 
  for PM10
*/
void send10Config(){
 DynamicJsonDocument JSONencoder(4096);
  JSONencoder["name"] = "air 10",
  JSONencoder["device_class"] = "pm10",
  JSONencoder["unit_of_measurement"] = "µg/m³",
  JSONencoder["dev"]["ids"] = MQTT_UNIQUE_IDENTIFIER,
  JSONencoder["dev"]["mf"] = "Robin47113",
  JSONencoder["dev"]["mdl"] = "1.0",
  JSONencoder["dev"]["name"] = "air",
  JSONencoder["state_topic"] = "homeassistant/sensor/air/state",
  JSONencoder["value_template"] = "{{ value_json.pm10}}" ;

  size_t n = measureJson(JSONencoder);
                  if (client.beginPublish("homeassistant/sensor/air10/config", n, true) == true) {
                   // SERIAL_DEBUG_LN("Configuration Publishing Begun")
                    if (serializeJson(JSONencoder, client) == n){
                     //   SERIAL_DEBUG_LN("Configuration Sent")
                    }
                    if (client.endPublish() == true) {
                       // SERIAL_DEBUG_LN("Configuration Publishing Finished")
                    }
                } else {
                  //  SERIAL_DEBUG_LN("Error sending Configuration")
                }
}

/*
  Connects to the Mqtt broker 
  send all the configs and a status
*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Air",user,pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      send1Config();
      send2Config();
      send10Config();

      //sendStatus();
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void updateValues(){
    if (sensor.read_sensor_value(buf, 29)) {
    Serial.println("HM330X read result failed!!!");
  }
  parse_result(buf);
  Serial.println("");
}

/*
  Light sleep callback
*/
void callback() {
  Serial1.println("Callback");
  Serial.flush();
}

void sensorInit(){
  digitalWrite(D6,LOW);
    //HM3301 init
  if (sensor.init()) {
        Serial.println("HM330X init failed!!!");
        while (1);
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.println("Start device in normal mode!");

  pinMode(D6,OUTPUT);
  digitalWrite(D6,LOW);

  //mqtt config
  client.setServer(server, 1883);

  //HM3301 init
  if (sensor.init()) {
        Serial.println("HM330X init failed!!!");
        while (1);
  }

  //connect to WIFI
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Wifi connected");

}


void loop() {

    Serial.println("Enter light sleep mode");
 
      uint32_t sleep_time_in_ms = 0xFFFFFFE;
      wifi_set_opmode(NULL_MODE);
      wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
      wifi_fpm_open();
      wifi_fpm_set_wakeup_cb(callback);
      wifi_fpm_do_sleep(sleep_time_in_ms *1000 );
      delay(sleep_time_in_ms + 1);
 
      Serial.println("Exit light sleep mode");
      //current time in millis
      int mil = millis();
      //start sensor
      sensorInit();
      //connect to wifi
      wifiManager.autoConnect("AutoConnectAP");
      Serial.println("Wifi connected");
      //connect to mqtt server
      if (!client.connected()) {
        reconnect();
      }
      //wait until timer is at 30s
      while(mil+30000<millis()){

      }
      //read values from sensor
      updateValues();
      //put sensor in sleepmode
      digitalWrite(D6,HIGH);
      //send values to server
      sendStatus();
       
      wifi_set_sleep_type(NONE_SLEEP_T);


//Code for sending values w/o sleep
/*
  if (!client.connected()) {
   reconnect();
  }else{
    sendStatus();
  }
  client.loop();
*/
}