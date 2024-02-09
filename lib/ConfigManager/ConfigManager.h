#ifndef __CONFIG_CONFIG_MANAGER_H__
#define __CONFIG_CONFIG_MANAGER_H__

// constants (changes may lead to crashes)
#define MAX_WIFI_SSID_LENGTH 32            // maximum length of wifi ssid
#define MAX_WIFI_PASSWORD_LENGTH 64        // maximum length of wifi password
#define MAX_FILENAME_LENGTH 32             // maximum length of filename
#define JSON_BUFFER_SIZE 4096              // size of json buffer
#define DEFAULT_CONFIG_FILE "/config.json" // default config file

#include <Arduino.h>

#include <LittleFS.h>

#include <ArduinoJson.h>

// type definitions
typedef struct {
    char ssid[MAX_WIFI_SSID_LENGTH];
    char password[MAX_WIFI_PASSWORD_LENGTH];
} local_wifi_config_t;

typedef struct {
    local_wifi_config_t wifi;
} config_t;


class ConfigManager {
    private:
        const char* filename;

    public:
        ConfigManager(const char* filename);
        ConfigManager();
        ~ConfigManager();

        config_t config;

        void load_config();
        void save_config();

        const char* get_wifi_ssid();
        const char* get_wifi_password();
};

#endif