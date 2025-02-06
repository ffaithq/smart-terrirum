#ifndef MQTT_h
#define MQTT_h

#include <ArduinoJson.h> // Library From Arduino manager
#include <WiFi.h>
#include <PubSubClient.h> // Library on GitHub 
#include "Config.h"



class MQTT {
private:
    WiFiClient espClient;
    PubSubClient client;  // Declare without initialization
    int deviceID;
    int status;
    char topic[32]; // Topic to send measurement data - automaticly assign 
    char name[32];
    String IP;

    void callback(char* topic, byte* payload, unsigned int length); // MQTT callback
    void callback_startup(char* topic, byte* payload, unsigned int length);
public:
    MQTT(int id);  // Constructor
    int setup_MQTT();
    int setup_WIFI();
    int send(char* topic, const String* valuesName, int nameCount, const String* values, int valueCount);
    int begin();
    int bundleJsonIntoBufferFloat(
        const String* valueNames, size_t valueNamesCount,
        const float*  values,     size_t valuesCount,
        char* outBuffer, size_t bufferSize,
        const int capacityJson
    );
    int bundleJsonIntoBufferString(
        const String* valueNames, size_t valueNamesCount,
        const String* values, size_t valuesCount,
        char* outBuffer, size_t bufferSize,
        int capacityJson // Pass deviceId explicitly
    );
    String get_IP();
    int get_ID();
    void loop();
};
#endif