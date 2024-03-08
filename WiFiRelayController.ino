#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "wifi.h"

ESP8266WebServer server(80);

uint8_t relayPin = LED_BUILTIN;
bool relayStatus = LOW;

void setup()
{
  Serial.begin(115200);
  delay(100);
  pinMode(relayPin, OUTPUT);

  // подключиться к вашей локальной wi-fi сети
  WiFi.setHostname("WiFiRelay");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // проверить, подключился ли wi-fi модуль к wi-fi сети
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/relay_on", handle_relay_on);
  server.on("/relay_off", handle_relay_off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();

  if (relayStatus)
    digitalWrite(relayPin, LOW);
  else
    digitalWrite(relayPin, HIGH);
}

void handle_OnConnect()
{
  Serial.print("Relay status: ");
  if (relayStatus)
    Serial.print("ON");
  else
    Serial.print("OFF");
  Serial.println("");
  server.send(200, "text/html", SendHTML(relayStatus));
}

void handle_relay_on()
{
  relayStatus = HIGH;
  Serial.println("Relay status: ON");
  server.send(200, "text/html", SendHTML(true));
}

void handle_relay_off()
{
  relayStatus = LOW;
  Serial.println("Relay status: OFF");
  server.send(200, "text/html", SendHTML(false));
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t relayStatus)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>WiFi relay</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>WiFiRelayController v1.0.0</h1>\n";
  ptr += "<h3>@rahmaevao</h3>\n";

  if (relayStatus)
    ptr += "<p>Relay status: ON</p><a class=\"button button-off\" href=\"/relay_off\">OFF</a>\n";
  else
    ptr += "<p>Relay status: OFF</p><a class=\"button button-on\" href=\"/relay_on\">ON</a>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}