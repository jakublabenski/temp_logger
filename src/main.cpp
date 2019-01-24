
#include <Arduino.h>

#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <PubSubClient.h>


WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

const char* topic = "/temp_logger";

void setup()
{
    Serial.begin(9600);

    WiFiManager wifiManager;
    wifiManager.autoConnect("temp_logger");

    client.setServer("192.168.0.3", 1883);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool one_wire_loop(int64_t& ret_addr, float& celsius);

void reading()
{
    do {
        int64_t addr = 0;
        float celsius = 0;

        if (!one_wire_loop(addr, celsius)) {
            break;
        };

        snprintf(msg, 50, "{ \"id\": %d, \"temp\": %.2f }", (int)addr, celsius);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish(topic, msg);

    } while (true);
}

void loop()
{
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    reading();

    Serial.println("Done");
    delay(10000);
}