
#include <Arduino.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

void setup()
{
    Serial.begin(9600);

    WiFiManager wifiManager;
    wifiManager.autoConnect("test_sensor");
}

void one_wire_loop();

void loop()
{
    one_wire_loop();
}