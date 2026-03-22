#ifndef CONFIG_API_H
#define CONFIG_API_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "nvs_manager.h"

static volatile unsigned long restartTime = 0;
static volatile bool bootComplete = false;
static String lastValidatedIP = "";
static bool ipValidationPassed = false;

class ConfigAPI
{
private:
    AsyncWebServer *server;
    ConfigManager *configMgr;

public:
    ConfigAPI(AsyncWebServer *srv, ConfigManager *cfg) : server(srv), configMgr(cfg) {}

    void registerEndpoints()
    {
        // GET /api/validate-ip - Validate if IP is available
        server->on("/api/validate-ip", HTTP_GET, [this](AsyncWebServerRequest *request)
        {
            if (!request->hasParam("ip")) {
                request->send(400, "application/json", "{\"error\":\"Missing ip parameter\"}");
                return;
            }
            
            String ipStr = request->getParam("ip")->value();
            uint8_t ip[4];
            
            // Parse IP
            if (sscanf(ipStr.c_str(), "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]) != 4) {
                request->send(400, "application/json", "{\"error\":\"Invalid IP format\"}");
                return;
            }
            
            // Validate IP range (not 0.x, 255.x)
            if (ip[0] == 0 || ip[0] == 255 || ip[3] == 0 || ip[3] == 255) {
                request->send(400, "application/json", "{\"error\":\"Invalid IP range (avoid 0.x, 255.x)\"}");
                return;
            }
            
            IPAddress testIP(ip[0], ip[1], ip[2], ip[3]);
            IPAddress currentIP = WiFi.localIP();
            
            // Check if IP is same as current device IP
            if (testIP == currentIP) {
                lastValidatedIP = ipStr;
                ipValidationPassed = true;
                request->send(200, "application/json", "{\"available\":true,\"message\":\"IP same as current (valid)\",\"current_ip\":true}");
                return;
            }
            
            // Validate subnet
            IPAddress gateway(DEFAULT_GATEWAY_1, DEFAULT_GATEWAY_2, DEFAULT_GATEWAY_3, DEFAULT_GATEWAY_4);
            bool sameSubnet = ((testIP[0] == currentIP[0]) && 
                              (testIP[1] == currentIP[1]) && 
                              (testIP[2] == currentIP[2]));
            
            if (!sameSubnet) {
                request->send(400, "application/json", "{\"error\":\"IP must be in same subnet: 10.225.180.x\"}");
                return;
            }
            
            // Check if is gateway
            if (testIP == gateway) {
                request->send(400, "application/json", "{\"error\":\"IP is gateway - cannot use\",\"gateway\":\"10.225.180.1\"}");
                return;
            }
            
            // Check if is broadcast
            if (testIP[3] == 255) {
                request->send(400, "application/json", "{\"error\":\"IP is broadcast - cannot use (x.x.x.255)\"}");
                return;
            }
            
            // Validation passed
            lastValidatedIP = ipStr;
            ipValidationPassed = true;
            request->send(200, "application/json", "{\"available\":true,\"message\":\"IP validation passed - ready to save\",\"valid\":true}");
        });

        // GET /api/config - Retrieve current config
        server->on("/api/config", HTTP_GET, [this](AsyncWebServerRequest *request)
        {
            String json = configMgr->toJSON();
            request->send(200, "application/json", json);
        });

        // POST /api/config - Update config
        server->on("/api/config", HTTP_POST, [this](AsyncWebServerRequest *request) {}, nullptr, 
        [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            try {
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, data);
                
                if (error) {
                    request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                    return;
                }

                String newIP = doc["static_ip"].is<String>() ? doc["static_ip"].as<String>() : "";
                
                // Require validation before save
                if (newIP.length() > 0 && !ipValidationPassed) {
                    request->send(403, "application/json", "{\"error\":\"IP not validated. Validate first.\",\"code\":\"VALIDATION_REQUIRED\"}");
                    return;
                }
                
                if (newIP.length() > 0 && newIP != lastValidatedIP) {
                    request->send(403, "application/json", "{\"error\":\"IP changed after validation. Re-validate.\",\"code\":\"MISMATCH\"}");
                    return;
                }

                // Update WiFi SSID
                if (doc["wifi_ssid"].is<String>()) {
                    configMgr->setWiFiSSID(doc["wifi_ssid"].as<String>());
                }

                // Update WiFi Password
                if (doc["wifi_pass"].is<String>()) {
                    configMgr->setWiFiPassword(doc["wifi_pass"].as<String>());
                }

                // Update Static IP
                if (doc["static_ip"].is<String>()) {
                    String ipStr = doc["static_ip"].as<String>();
                    uint8_t ip[4];
                    if (sscanf(ipStr.c_str(), "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]) == 4) {
                        configMgr->setStaticIP(ip[0], ip[1], ip[2], ip[3]);
                    }
                }

                // Clear validation after successful save
                ipValidationPassed = false;
                lastValidatedIP = "";
                
                request->send(200, "application/json", "{\"status\":\"OK\",\"message\":\"Config updated. Device will restart in 5 seconds...\"}");
                
                if (bootComplete) {
                    restartTime = millis() + 5000;
                    Serial.println("[API] Restart scheduled in 5 seconds");
                }

            } catch (...) {
                request->send(500, "application/json", "{\"error\":\"Server error\"}");
            }
        });

        // GET /api/system - System info
        server->on("/api/system", HTTP_GET, [this](AsyncWebServerRequest *request)
        {
            JsonDocument doc;
            doc["device"] = "ESP32 DOIT DevKit V1";
            doc["ip"] = WiFi.localIP().toString();
            doc["ssid"] = WiFi.SSID();
            doc["mac"] = WiFi.macAddress();
            doc["free_heap"] = ESP.getFreeHeap();
            doc["uptime"] = millis() / 1000;

            String json;
            serializeJson(doc, json);
            request->send(200, "application/json", json);
        });

        // POST /api/reset - Factory Reset
        server->on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
            // Require validation before factory reset
            if (!ipValidationPassed) {
                request->send(403, "application/json", "{\"error\":\"Validate IP first before factory reset\",\"code\":\"VALIDATION_REQUIRED\"}");
                return;
            }
            
            ipValidationPassed = false;
            lastValidatedIP = "";
            
            configMgr->resetToDefaults();
            request->send(200, "application/json", "{\"status\":\"OK\",\"message\":\"Factory reset. Device will restart...\"}");
            
            if (bootComplete) {
                restartTime = millis() + 2000;
                Serial.println("[API] Factory reset - restart in 2 seconds");
            }
        });

        Serial.println("[API] Config endpoints registered");
    }

    // Check scheduled restart (called from main loop)
    static void checkScheduledRestart()
    {
        if (restartTime > 0 && millis() >= restartTime)
        {
            restartTime = 0;
            Serial.println("[API] Executing restart");
            delay(100);
            ESP.restart();
        }
    }

    // Flag boot as complete
    static void bootCompleted()
    {
        bootComplete = true;
        Serial.println("[BOOT] Startup complete - restart enabled");
    }
};

#endif
