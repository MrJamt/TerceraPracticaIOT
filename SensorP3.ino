#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

//DHT
#define DHTPIN 32          //Digital Pin
#define DHTTYPE DHT11     //DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

//HC-SR04
int triggerPin = 14;
int echoPin = 12;

const char *WIFI_SSID = "TECHLAB";
const char *WIFI_PASS = "catolica11";

const char * MQTT_BROKER = "broker.hivemq.com";
const int MQTT_BROKER_PORT = 1883;

const char * MQTT_CLIENT_ID = "jorge.montano@ucb.edu.bo";

const char * SUBSCRIBE_TOPIC_DHT_T = "jamt/output/dht/t";
const char * SUBSCRIBE_TOPIC_DHT_H = "jamt/output/dht/h";
const char * SUBSCRIBE_TOPIC_HC_D = "jamt/output/hc/d";

WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);

boolean mqttClientConnect() {
  Serial.print("Connecting to " + String(MQTT_BROKER));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.print("Connected to " + String(MQTT_BROKER));
  } else {
    Serial.println("Can't connect to " + String(MQTT_BROKER));
  }
  return mqttClient.connected();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println(" DONE!");
  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  dht.begin();
}

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float cm = 0.01723 * readUltrasonicDistance(14, 12);

  Serial.print("Humedad Relativa: ");
  Serial.print(h);
  Serial.print("% \t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" *C \t");
  Serial.print("Distancia: ");
  Serial.print(cm);
  Serial.println("cm");

  if (!mqttClient.connected()) {
    mqttClientConnect();
  }
  else {
    Serial.println("Published in " + String(SUBSCRIBE_TOPIC_DHT_T));
    mqttClient.publish(SUBSCRIBE_TOPIC_DHT_T, String(t).c_str());
    Serial.println("Published in " + String(SUBSCRIBE_TOPIC_DHT_H));
    mqttClient.publish(SUBSCRIBE_TOPIC_DHT_H, String(h).c_str());
    Serial.println("Published in " + String(SUBSCRIBE_TOPIC_HC_D));
    mqttClient.publish(SUBSCRIBE_TOPIC_HC_D,  String(cm).c_str());
  }
}