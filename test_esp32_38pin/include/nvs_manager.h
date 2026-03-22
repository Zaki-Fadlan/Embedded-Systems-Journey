#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <Preferences.h>
#include <Preferences.h>
#include "credentials.h"

class ConfigManager
{
private:
    Preferences preferences;
    bool initialized = false;

public:
    bool begin()
    {
        if (!preferences.begin(NVS_NAMESPACE, false))
        {
            Serial.println("[NVS] Failed to initialize Preferences!");
            return false;
        }
        initialized = true;
        Serial.println("[NVS] Preferences initialized");
        return true;
    }

    void end()
    {
        preferences.end();
        initialized = false;
    }

    // WiFi SSID
    void setWiFiSSID(const String &ssid)
    {
        if (initialized)
        {
            preferences.putString(KEY_WIFI_SSID, ssid);
            Serial.printf("[NVS] WiFi SSID saved: %s\n", ssid.c_str());
        }
    }

    String getWiFiSSID()
    {
        if (initialized)
        {
            return preferences.getString(KEY_WIFI_SSID, DEFAULT_WIFI_SSID);
        }
        return DEFAULT_WIFI_SSID;
    }

    // WiFi Password
    void setWiFiPassword(const String &pass)
    {
        if (initialized)
        {
            preferences.putString(KEY_WIFI_PASS, pass);
            Serial.println("[NVS] WiFi Password saved");
        }
    }

    String getWiFiPassword()
    {
        if (initialized)
        {
            return preferences.getString(KEY_WIFI_PASS, DEFAULT_WIFI_PASS);
        }
        return DEFAULT_WIFI_PASS;
    }

    // Static IP
    void setStaticIP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4)
    {
        if (initialized)
        {
            String ipStr = String(ip1) + "." + String(ip2) + "." + String(ip3) + "." + String(ip4);
            preferences.putString(KEY_STATIC_IP, ipStr);
            Serial.printf("[NVS] Static IP saved: %s\n", ipStr.c_str());
        }
    }

    bool getStaticIP(uint8_t &ip1, uint8_t &ip2, uint8_t &ip3, uint8_t &ip4)
    {
        if (!initialized)
            return false;

        String ipStr = preferences.getString(KEY_STATIC_IP,
                                             String(DEFAULT_STATIC_IP_1) + "." + String(DEFAULT_STATIC_IP_2) + "." +
                                                 String(DEFAULT_STATIC_IP_3) + "." + String(DEFAULT_STATIC_IP_4));

        int parts[4];
        if (sscanf(ipStr.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]) == 4)
        {
            ip1 = parts[0];
            ip2 = parts[1];
            ip3 = parts[2];
            ip4 = parts[3];
            return true;
        }
        return false;
    }

    // Reset to Defaults
    void resetToDefaults()
    {
        if (initialized)
        {
            preferences.clear();
            Serial.println("[NVS] All settings reset to defaults");
        }
    }

    // Get JSON representation
    String toJSON()
    {
        String json = "{";
        json += "\"wifi_ssid\":\"" + getWiFiSSID() + "\",";
        json += "\"static_ip\":\"";

        uint8_t ip1, ip2, ip3, ip4;
        if (getStaticIP(ip1, ip2, ip3, ip4))
        {
            json += String(ip1) + "." + String(ip2) + "." + String(ip3) + "." + String(ip4);
        }
        json += "\"";
        json += "}";
        return json;
    }
};

#endif
