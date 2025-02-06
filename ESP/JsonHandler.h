#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <ArduinoJson.h> // Library From Arduino manager

#include "Config.h"

//Function to bundle data into JSON for sending via MQTT

int bundleJsonIntoBuffer(
    const String* valueNames, size_t valueNamesCount,
    const float*  values,     size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    const int capacityJson, int deviceID
);
int bundleJsonIntoBuffer(
    const String* valueNames, size_t valueNamesCount,
    const String* values, size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    int capacityJson,int deviceID // Pass deviceId explicitly
);




#endif