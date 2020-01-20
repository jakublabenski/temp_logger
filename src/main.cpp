
#include <Arduino.h>

#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal

#include <PubSubClient.h>

#include <IotWebConf.h>

WiFiClient espClient;
PubSubClient client(espClient);

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf("temp_sensor", &dnsServer, &server, "password");

char msg[50];

const char* topic = "/temp_logger";

void setup()
{
    Serial.begin(9600);

    iotWebConf.init();

    // -- Set up required URL handlers on the web server.
//    server.on("/", handleRoot);
    server.on("/", []{ iotWebConf.handleConfig(); });
    server.onNotFound([](){ iotWebConf.handleNotFound(); });

    client.setServer("192.168.0.3", 1883);
    auto a = WiFi.localIP();
    a.toString().toCharArray(msg, 50);
    Serial.printf("IP: [%s]\n", msg);
    Serial.printf("IP: [%s]\n", msg);
    WiFi.SSID().toCharArray(msg, 50);
    Serial.printf("SSID: [%s]\n", msg);

}

void reconnect() {
    // Loop until we're reconnected
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
    iotWebConf.doLoop();

    if (WiFi.isConnected()) {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
        reading();
    }

    Serial.println("Done");
    delay(10000);
}