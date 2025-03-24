#include "JsonHandler.h"

int bundleJsonIntoBuffer(
    const char* const* valueNames, size_t valueNamesCount,
    const float* values, size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    const int capacityJson, int deviceID
) {
  // check if connection is establish. Negative ID means connection isn't establish
  if (deviceID < 0){
    return JH_ID_NEGATIVE;
  }
  // Check if the number of names and values are mismatched
  if (valueNamesCount > valuesCount) {
    if (DEBUG){
      Serial.println("[JSON_HADLER][ERROR] Not enough values!");
    }

    return JH_NOT_ENOUGH_VALUES;
  }


  if (valueNamesCount < valuesCount) {
    if (DEBUG){
      Serial.println("[JSON_HADLER][ERROR] Not enough names!");
    }

    return JH_NOT_ENOUGH_NAMES;
  }

  // Create a DynamicJsonDocument with runtime-defined capacity
  DynamicJsonDocument json(capacityJson);

  // Add deviceId to JSON
  json["ID"] = deviceID;

  // Add (name -> value) pairs
  for (size_t i = 0; i < valueNamesCount; i++) {
    json[valueNames[i]] = values[i];
    
    if (json.memoryUsage() > json.capacity()) {
      if(DEBUG){
        Serial.println("[JSON_HADLER][ERROR] Not enough memory to add new items!");
      }

      return JH_OVERFLOW_JSON;
    }
  }

  // Serialize JSON into outBuffer
  size_t bytesWritten = serializeJson(json, outBuffer, bufferSize);
  if (bytesWritten + 1 > bufferSize) { // Ensure null terminator fits
    if (DEBUG){
      Serial.println("[JSON_HADLER][ERROR] JSON output truncated!");
    }

    return JH_TRUNCUT_BUFFER;
  }

  return OK; // Success
}