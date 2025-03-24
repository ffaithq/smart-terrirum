#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <ArduinoJson.h> // Library From Arduino manager
#include "Config.h"
#include "ERROR.h"

//Function to bundle data into JSON for sending via MQTT
int bundleJsonIntoBuffer(
    const char* const* valueNames, size_t valueNamesCount,
    const float* values, size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    const int capacityJson, int deviceID
);




#endif