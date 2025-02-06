
#include "JsonHandler.h"

int bundleJsonIntoBuffer(
    const String* valueNames, size_t valueNamesCount,
    const String* values, size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    int capacityJson, int deviceID
) {

  // Check if the number of names and values match
  if (valueNamesCount > valuesCount) {
    if(DEBUG){
      Serial.println("[JSON_HADLER STRING][ERROR] Not enough values!");
    }

    return JH_NOT_ENOUGH_VALUES;
  }
  if (valueNamesCount < valuesCount) {
    if(DEBUG){
      Serial.println("[JSON_HADLER][ERROR] Not enough names!");
    }

    return JH_NOT_ENOUGH_NAMES;
  }

  // Allocate space for JSON
  DynamicJsonDocument json(capacityJson);

  // Add deviceId
  json["deviceId"] = deviceID;

  // Add (name -> value) pairs
  for (size_t i = 0; i < valueNamesCount; i++) {
    json[valueNames[i]] = values[i];

    // Check for memory overflow
    if (json.memoryUsage() > json.capacity()) {
      if(DEBUG){
        Serial.println("[JSON_HADLER][ERROR] Not enough memory to add new items!");
      }

      return JH_OVERFLOW_JSON;
    }
  }

  // Serialize JSON into outBuffer
  size_t bytesWritten = serializeJson(json, outBuffer, bufferSize);

  // Ensure space for null terminator
  if (bytesWritten + 1 >= bufferSize) { 
    if(DEBUG){
      Serial.println("[JSON_HADLER][ERROR] JSON output truncated!");
    }

    return JH_TRUNCUT_BUFFER;
  }

  return OK; // Success
}

// TODO: Add error if device number is -1. Trying send data before connection was establish

int bundleJsonIntoBuffer(
    const String* valueNames, size_t valueNamesCount,
    const float*  values,     size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    const int capacityJson,int deviceID
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
  json["deviceId"] = deviceID;

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