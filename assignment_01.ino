// IoT Workshop
// Send temperature and humidity data to MQTT
//
// WiFiNINA https://www.arduino.cc/en/Reference/WiFiNINA (MKR WiFi 1010)
// Arduino MKR ENV https://www.arduino.cc/en/Reference/ArduinoMKRENV
// Arduino MQTT Client  https://github.com/arduino-libraries/ArduinoMqttClient

#include <WiFiNINA.h>
#include <Arduino_MKRENV.h>
#include <ArduinoMqttClient.h>

#include "config.h"

WiFiSSLClient net;
MqttClient mqtt(net);

String temperatureTopic = "itp/" + DEVICE_ID + "/temperature";
String humidityTopic = "itp/" + DEVICE_ID + "/humidity";
String pressureTopic = "itp/" + DEVICE_ID + "/pressure";
String illuminanceTopic = "itp/" + DEVICE_ID + "/illuminance";
String uvaTopic = "itp/" + DEVICE_ID + "/uva";
String uvbTopic = "itp/" + DEVICE_ID + "/uvb";
String soilTopic = "itp/" + DEVICE_ID + "/soil";

// Publish every 10 seconds for the workshop. Real world apps need this data every 5 or 10 minutes.
unsigned long publishInterval = 1800 * 1000; //setting 30 minutes interval
//unsigned long publishInterval = 10 * 1000; //setting 30 minutes interval

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);

  // Wait for a serial connection
  // while (!Serial) { }

  // initialize the shield
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }

  Serial.println("Connecting WiFi");
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqtt.poll();

  if (millis() - lastMillis > publishInterval) {
    lastMillis = millis();

    float temperature = ENV.readTemperature(FAHRENHEIT);
    float humidity    = ENV.readHumidity();
    float pressure    = ENV.readPressure();                // kPa
    float illuminance = ENV.readIlluminance();             // lux
    float uva         = ENV.readUVA();                     // μW/cm2
    float uvb         = ENV.readUVB();                     // μW/cm2
    float uvindex     = ENV.readUVIndex();                 // 0 - 11
    float moisture    = analogRead(0);                     //connect sensor to Analog 0

    Serial.print(temperature);
    Serial.print("°F ");
    Serial.print(humidity);
    Serial.print("% RH ");
    Serial.print(pressure);
    Serial.print("kPa ");
    Serial.print(illuminance);
    Serial.print("lux ");
    Serial.print(uva);
    Serial.print("μW/cm2 ");
    Serial.print(uvb);
    Serial.print("μW/cm2 ");
    Serial.print(moisture);
    Serial.println("% ");

    mqtt.beginMessage(temperatureTopic);
    mqtt.print(temperature);
    mqtt.endMessage();

    mqtt.beginMessage(humidityTopic);
    mqtt.print(humidity);
    mqtt.endMessage();

    mqtt.beginMessage(pressureTopic);
    mqtt.print(pressure);
    mqtt.endMessage();

    mqtt.beginMessage(illuminanceTopic);
    mqtt.print(illuminance);
    mqtt.endMessage();

    mqtt.beginMessage(uvaTopic);
    mqtt.print(uva);
    mqtt.endMessage();

    mqtt.beginMessage(uvbTopic);
    mqtt.print(uvb);
    mqtt.endMessage();

    mqtt.beginMessage(soilTopic);
    mqtt.print(moisture);
    mqtt.endMessage();
  }
}

void connectWiFi() {
  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  Serial.print("WiFi firmware version ");
  Serial.println(WiFi.firmwareVersion());

  Serial.print("Attempting to connect to SSID: ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");

  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(3000);
  }

  Serial.println("Connected to WiFi");
  printWiFiStatus();

}

void connectMQTT() {
  Serial.print("Connecting MQTT...");
  mqtt.setId(DEVICE_ID);
  mqtt.setUsernamePassword(MQTT_USER, MQTT_PASSWORD);

  while (!mqtt.connect(MQTT_BROKER, MQTT_PORT)) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("connected.");
}

void printWiFiStatus() {
  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
