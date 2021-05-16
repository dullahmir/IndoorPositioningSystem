#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

char rssi[10];

// SSID/Password
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxxr";

// MQTT Broker IP address
const char* mqtt_server = "192.168.2.16";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
     // if (strcmp(advertisedDevice.getName().c_str(),"SoundCore mini") == 0) {
      if (strcmp(advertisedDevice.getAddress().toString().c_str(),"00:e0:4c:61:69:f8") == 0) {
      Serial.printf("\n \n");
      Serial.printf("Device found! \n");
      Serial.printf("Name: %s \n", advertisedDevice.getName().c_str());
      client.publish("esp32_living/soundcore", advertisedDevice.getName().c_str());
      Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
      Serial.printf("MAC: %s \n", advertisedDevice.getAddress().toString().c_str());
      //client.publish("esp32/soundcoreMAC", advertisedDevice.getAddress().toString().c_str());
     // Serial.printf("TX Power: %d \n", advertisedDevice.getTXPower());
      //Serial.printf("UUID: %s \n", advertisedDevice.getServiceUUID().toString().c_str());
      Serial.printf("\n \n \n");
      dtostrf(advertisedDevice.getRSSI(), 1, 2, rssi);
      client.publish("esp32_living/RSSI", rssi);
      }
    }
};

void setup() {
  Serial.begin(115200);

  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_Scanner_2")) {
      Serial.println("connected");
      // Subscribe
     // client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1.5 seconds");
      // Wait 5 seconds before retrying
      delay(1500);
    }
  }
}
void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory  
 
    
  }
}
