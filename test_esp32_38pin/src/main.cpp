#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Preferences.h>
#include "credentials.h"
#include "nvs_manager.h"
#include "config_api.h"

AsyncWebServer server(80);
ConfigManager configMgr;
ConfigAPI configAPI(&server, &configMgr);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Initialize LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS OK");

  // Initialize NVS (Preferences)
  if (!configMgr.begin())
  {
    Serial.println("NVS initialization failed!");
    return;
  }

  // Read config from NVS
  String wifiSSID = configMgr.getWiFiSSID();
  String wifiPass = configMgr.getWiFiPassword();
  uint8_t staticIP1, staticIP2, staticIP3, staticIP4;
  configMgr.getStaticIP(staticIP1, staticIP2, staticIP3, staticIP4);

  Serial.printf("Loaded Config - SSID: %s\n", wifiSSID.c_str());
  Serial.printf("Static IP: %d.%d.%d.%d\n", staticIP1, staticIP2, staticIP3, staticIP4);

  // WiFi Setup
  WiFi.mode(WIFI_STA);

  // Set static IP before connect
  IPAddress staticIP(staticIP1, staticIP2, staticIP3, staticIP4);
  IPAddress gateway(DEFAULT_GATEWAY_1, DEFAULT_GATEWAY_2, DEFAULT_GATEWAY_3, DEFAULT_GATEWAY_4);
  IPAddress subnet(DEFAULT_SUBNET_1, DEFAULT_SUBNET_2, DEFAULT_SUBNET_3, DEFAULT_SUBNET_4);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("Static IP config failed!");
  }

  WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    Serial.print(".");
    delay(500);
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("IP Address (Fixed): ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("WiFi connection failed!");
  }

  // Register Web Server Routes
  // Register Config API FIRST (before static file serving)
  configAPI.registerEndpoints();

  // Then register static file serving
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Not Found"); });

  server.begin();
  Serial.println("Web server started!");
  Serial.println("========================================");
  Serial.println("API Endpoints:");
  Serial.println("  GET  /api/config    - Get current config");
  Serial.println("  POST /api/config    - Update config");
  Serial.println("  GET  /api/system    - System info");
  Serial.println("  POST /api/reset     - Factory reset");
  Serial.println("========================================");

  // Signal boot completion (enables restart feature)
  ConfigAPI::bootCompleted();
}

void loop()
{
  // Check for scheduled restart (non-blocking)
  ConfigAPI::checkScheduledRestart();
  delay(100);
}