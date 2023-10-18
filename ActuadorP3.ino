#include <WiFi.h>
#include <PubSubClient.h>

const char *WIFI_SSID = "TECHLAB";
const char *WIFI_PASS = "catolica11";

const char * MQTT_BROKER = "broker.hivemq.com";
const int MQTT_BROKER_PORT = 1883;

const char * MQTT_CLIENT_ID = "adriel.mounzon@ucb.edu.bo";

const char * SUBSCRIBE_TOPIC_DHT_T = "jamt/output/dht/t";
const char * SUBSCRIBE_TOPIC_DHT_H = "jamt/output/dht/h";
const char * SUBSCRIBE_TOPIC_HC_D = "jamt/output/hc/d";

//Leds
int RED_LED_D = 33;
int YELLOW_LED_D = 25;

int RED_LED_H = 26;
int YELLOW_LED_H = 27;

int RED_LED_T = 14;
int YELLOW_LED_T = 12;

WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);

void callback(const char * topic, byte * payload, unsigned int lenght) {
  String message;
  for (int i = 0; i < lenght; i++) {
    message += String((char) payload[i]);
  }
    if (String(topic) == SUBSCRIBE_TOPIC_HC_D) { //its distance
  Serial.println("Message from topic " + String(topic) + "> " + message);

    if (message.toFloat() >= 5.0) {
      digitalWrite(RED_LED_D, HIGH);
      digitalWrite(YELLOW_LED_D, LOW);
    } else {
      digitalWrite(RED_LED_D, LOW);
      digitalWrite(YELLOW_LED_D, HIGH);
    }
  } else if (String(topic) == SUBSCRIBE_TOPIC_DHT_T) { //its temperature
    Serial.println("Message from topic " + String(topic) + "> " + message);

    if (message.toFloat() >= 28) {
      digitalWrite(RED_LED_T, HIGH);
      digitalWrite(YELLOW_LED_T, LOW);
    } else {
      digitalWrite(RED_LED_T, LOW);
      digitalWrite(YELLOW_LED_T, HIGH);
    }
  }
  else if (String(topic) == SUBSCRIBE_TOPIC_DHT_H) { //its humidity
  Serial.println("Message from topic " + String(topic) + "> " + message);

    if (message.toFloat() > 32.0) {
      digitalWrite(RED_LED_H, HIGH);
      digitalWrite(YELLOW_LED_H, LOW);
    } else {
      digitalWrite(RED_LED_H, LOW);
      digitalWrite(YELLOW_LED_H, HIGH);
    }
  }
}

boolean mqttClientConnect() {
  Serial.println("Connecting to " + String(MQTT_BROKER));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
        Serial.println("Connected to " + String(MQTT_BROKER));

    mqttClient.subscribe(SUBSCRIBE_TOPIC_DHT_T);
    Serial.println("Subscribed to " + String(SUBSCRIBE_TOPIC_DHT_T));

    mqttClient.subscribe(SUBSCRIBE_TOPIC_DHT_H);
    Serial.println("Subscribed to " + String(SUBSCRIBE_TOPIC_DHT_H));

    mqttClient.subscribe(SUBSCRIBE_TOPIC_HC_D);
    Serial.println("Subscribed to " + String(SUBSCRIBE_TOPIC_HC_D));
  } else {
    Serial.println("Can't connect to " + String(MQTT_BROKER));
  }
  return mqttClient.connected();
}

void setup() {
  pinMode(RED_LED_T, OUTPUT);
  pinMode(YELLOW_LED_T, OUTPUT);
  pinMode(RED_LED_H, OUTPUT);
  pinMode(YELLOW_LED_H, OUTPUT);
  pinMode(RED_LED_D, OUTPUT);
  pinMode(YELLOW_LED_D, OUTPUT);

  Serial.begin(115200);
  Serial.println("Connecting to " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connected to " + String(WIFI_SSID));
  
  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqttClient.setCallback(callback);
}

void loop() {
  
  if (!mqttClient.connected()) {
    mqttClientConnect();
  }
  else {
    mqttClient.loop();
  }
}