#ifndef MQTTGuard_h
#define MQTTGuard_h

#include <ArduinoJson.h> // Library From Arduino manager
#include <WiFi.h>
#include <PubSubClient.h> // Library on GitHub 
#include <string.h>

#include "Config.h"
#include "callback.h"
#include "JsonHandler.h"

class MQTT {
private:
    WiFiClient espClient;
    PubSubClient client;  // Declare without initialization
    int deviceID;
    char topic[32]; // Topic to send measurement data - automaticly assign 
    char name[32];
    String IP;


public:
    MQTT(int id);  // Constructor
    int setup_MQTT();
    int setup_WIFI();
    int disconect_MQTT();
    int send(char* topic, const String* valuesName, int nameCount, const String* values, int valueCount);
    int send(char* topic, const String* valuesName, int nameCount, const float* values, int valueCount);
    int begin();
    int assign_ID(int id);
    int assign_name(char *n);
    int assign_topic(char *t);
    int get_IP(String *pIP);
    int get_ID(int *pID);
};

extern MQTT mqtt;

#endif