#include <Arduino.h>
//Wifi
#include <ESP8266WiFi.h>          
//Wifi manager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//particle sensor
#include <Seeed_HM330X.h>
//Gas Sensor
#include <DFRobot_MICS.h>
//json
#include <ArduinoJson.h>
//MQtt
#include <PubSubClient.h>


#define CALIBRATION_TIME   1        // in minutes

// When using I2C communication, use the following program to construct an object by DFRobot_MICS_I2C
/**
 * select i2c device address 
 * MICS_ADDRESS_0               0x75
 * MICS_ADDRESS_1               0x76
 * MICS_ADDRESS_2               0x77
 * MICS_ADDRESS_3               0x78
 */
#define MICS_I2C_ADDRESS MICS_ADDRESS_0
DFRobot_MICS_I2C mics(&Wire, MICS_I2C_ADDRESS);

WiFiClient wlanClient;
PubSubClient client(wlanClient);
IPAddress server(192, 168, 178, 85); //mqtt server

HM330X sensor;
uint8_t buf[30];


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
    for (int i = 1; i < 8; i++) {
        value = (uint16_t) data[i * 2] << 8 | data[i * 2 + 1];
        print_result(str[i - 1], value);

    }

    return NO_ERROR;
}

HM330XErrorCode parse_result_value(uint8_t *data) {
    if (NULL == data) {
        return ERROR_PARAM;
    }
    for (int i = 0; i < 28; i++) {
        Serial.print(data[i], HEX);
        Serial.print("  ");
        if ((0 == (i) % 5) || (0 == i)) {
            Serial.println("");
        }
    }
    uint8_t sum = 0;
    for (int i = 0; i < 28; i++) {
        sum += data[i];
    }
    if (sum != data[28]) {
        Serial.println("wrong checkSum!!!!");
    }
    Serial.println("");
    return NO_ERROR;
}



void setup() {
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Wifi connected");

  client.setServer(server, 1883);
  //client.setCallback(callback);

  while(!Serial);
  while(!mics.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  } Serial.println("Device connected successfully !");

  /**
   * Gets the power mode of the sensor
   * The sensor is in sleep mode when power is on,so it needs to wake up the sensor. 
   * The data obtained in sleep mode is wrong
   */
  uint8_t mode = mics.getPowerState();
  if(mode == SLEEP_MODE){
    mics.wakeUpMode();
    Serial.println("wake up sensor success!");
  }else{
    Serial.println("The sensor is wake up mode");
  }

  /**
   * Do not touch the sensor probe when preheating the sensor.
   * Place the sensor in clean air.
   * The default calibration time is 3 minutes.
   */
  while(!mics.warmUpTime(CALIBRATION_TIME)){
    Serial.println("Please wait until the warm-up time is over!");
    delay(1000);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("AirQuality")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");

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

void loop() {
  if (!client.connected()) {
   //reconnect();
  }
  //client.loop();

    /**
   * Gas type:
   * MICS-4514 You can get all gas concentration
   * MICS-5524 You can get the concentration of CH4, C2H5OH, H2, NH3, CO
   * MICS-2714 You can get the concentration of NO2
   *   CO       = 0x01  (Carbon Monoxide)  (1    - 1000)PPM
   *   CH4      = 0x02  (Methane)          (1000 - 25000)PPM
   *   C2H5OH   = 0x03  (Ethanol)          (10   - 500)PPM
   *   H2       = 0x06  (Hydrogen)         (1    - 1000)PPM
   *   NH3      = 0x08  (Ammonia)          (1    - 500)PPM
   *   NO2      = 0x0A  (Nitrogen Dioxide) (0.1  - 10)PPM
   */
  float gasdata = mics.getGasData(CO);
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  //mics.sleepMode();

    if (sensor.read_sensor_value(buf, 29)) {
        Serial.println("HM330X read result failed!!!");
    }
    parse_result_value(buf);
    parse_result(buf);
    Serial.println("");

    delay(1000);
}