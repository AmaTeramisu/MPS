#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include "DHT.h"

#define DHTPIN 4     
#define DHTTYPE DHT22   
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6


DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);


const int relay = 5;
float humidity;
float temperature;


const float maxTemperature = 35;
const float maxHumidity = 80;


void handleStatus() {
    String state = digitalRead(5) == HIGH ? "Ventilation is working" : "Ventilation isn't working";
    String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Web Server Demo</title>
        <meta http-equiv="refresh" content="2">
      </head>
      <body> 
    )";
    String resp = R"(
        </body>
      </html>
    )";
    String message = "<h1>Temperature: " + String(temperature) + "</h1>";
    message += "<h1>Humidity: " + String(humidity) + "</h1>";
    message += "<h1>" + state + "</h1>";
    String out = response + message + resp;
    server.send(200, "text/html", out);

}

void setup(void) {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleStatus);
  server.begin();
  Serial.println("HTTP server started (http://localhost:8180)");
}

void loop(void) {

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.println(String(temperature));
  Serial.println(String(humidity));
  if(humidity >= maxHumidity || temperature >= maxTemperature)
  { 
    digitalWrite(relay, HIGH);
    Serial.println("Ventilation is working");
  } 
  else
  {
    digitalWrite(relay, LOW);
    Serial.println("Ventilation isn't working");
  }
  Serial.println("");
  Serial.println("-----------------------");
  Serial.println("");
  server.handleClient();
  delay(1000);
  
}
