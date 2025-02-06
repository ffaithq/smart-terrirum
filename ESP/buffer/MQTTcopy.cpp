#include "MQTT.h"

MQTT::MQTT(int id) : client(espClient), deviceID(id) {
  sprintf(name, NAME_MQTT_CLIENT);
  client.setServer(IP_MQTT_SERVER, 1883);
}

int MQTT::setup_WIFI(){
    delay(50);
    status = WIFI_CONNECTING_BEGIN;
    if (DEBUG){
      Serial.print("\n[WiFi] Connecting to ");
      Serial.println(WIFI_SSID);
    }


    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int c = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        status = WIFI_CONNECTING_WAIT;
        if (DEBUG){
          Serial.println("\n[WIFI] Wait till WiFi is connected");
          Serial.print("Try: ");
          Serial.print(c);
          Serial.print("/10\n");
        }

        c++;

        if (c > 10) {
            if (DEBUG){
              Serial.println("\n[WIFI] Fail to connect. Wait too long");
            }
            return WIFI_FAIL_CONNECTION;
        }
  }
  IP = WiFi.localIP().toString();
  if (DEBUG){
    Serial.println("\n[WiFi] Connected");
    Serial.print("\n[WiFi] IP address: ");
    Serial.println(WiFi.localIP());
  }
  return WIFI_CONNECTED;
}

int MQTT::setup_MQTT(){
  // Keep trying until we're connected
  while (!client.connected()) {
    // First ensure Wi-Fi is still up
    if (WiFi.status() != WL_CONNECTED) {
      if(DEBUG){
        Serial.println("WIFI is not connected");
      }
      return MQTT_WIFI_NOT_CONNECTED;
    }
    
    if(DEBUG){
      Serial.print("[MQTT] Attempting MQTT connection with client name: ");
      Serial.println(NAME_MQTT_CLIENT);
    }


    // Attempt to connect
    if (client.connect(name)) {
      if (DEBUG){
        Serial.println("[MQTT] Connected to broker");
      }
      

      // If we don't have an ID yet, subscribe to get it
      if (deviceID < 0) {
        if (DEBUG){
          Serial.println("[MQTT] Subscribing to 'rpi/list_ids' to obtain ID");
        }

        client.subscribe("rpi/list_ids");
        client.setCallback([](char* topic, byte* payload, unsigned int length) {
          // Do something without instance data.
          Serial.print("Message received on topic: ");
          Serial.println(topic);
        });
        client.loop();
        const String valueNames[] = {"IP", "Description"};
        const String values[] = {IP, DESCRIPTION};
        send("rpi/conn_esp",valueNames,sizeof(valueNames)/sizeof(valueNames[0]),values,sizeof(values)/sizeof(values[0]));
      } 
      else {
        // Already have an ID, so we presumably want to publish on topic = "esp/id_<id>"
        Serial.print("[MQTT] Already have ID: ");
        Serial.println(deviceID);
        Serial.print("[MQTT] Will publish to topic: ");
        Serial.println(topic);

        client.subscribe("rpi/setpoints");
        client.setCallback([](char* topic, byte* payload, unsigned int length) {
          // Do something without instance data.
          Serial.print("Message received on topic: ");
          Serial.println(topic);
        });
      }

    } else {

      if(DEBUG){
        Serial.print("[MQTT] Connection failed, rc=");
        Serial.print(client.state());
        Serial.println(" - retry in 2 seconds");
      }

      //blink_led(3, 200);
      delay(2000);
    }
  }
  return MQTT_CONNECTED;
}
          





void MQTT::callback_startup(char* topicReceived, byte* message, unsigned int length) {
    if(DEBUG){
      Serial.print("\n[Callback] Message arrived on topic: ");
      Serial.println(topicReceived);
    }


    // Build a temporary String from the message bytes
    String messageTemp;
    for (int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    Serial.print("[Callback] Payload: ");
    Serial.println(messageTemp);

    // We only care about messages from "rpi/list_ids"
    if (String(topicReceived) == "rpi/list_ids") {
    // Parse JSON to get our ID
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, messageTemp);
    if (error) {
        Serial.print("[Callback] JSON parse failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Assign the extracted ID to the global 'id' (do NOT re-declare int here!)
    deviceID = doc[IP].as<int>(); 
    Serial.print("[Callback] Received id = ");
    Serial.println(deviceID);

    // Blink the LED 'id' times
    //blink_led(deviceId, 1000);

    // We no longer need "rpi/list_ids"
    //client.unsubscribe("rpi/list_ids");

    // Optionally disconnect, so we can reconnect with a new client name
    //client.disconnect();

    // Create new client name and publish topic
    // e.g., name = "esp_42" and topic = "esp/id_42"
    sprintf(name, "esp_%d", deviceID);
    sprintf(topic, "esp/id_%d", deviceID);

    Serial.print("[Callback] New client name: ");
    Serial.println(name);
    Serial.print("[Callback] New topic: ");
    Serial.println(topic);

    // Reconnect with the new name (see connect_mqttServer below)
    }
}

// Define the callback function
void MQTT::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received message on topic: ");
    Serial.println(topic);

    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}


String MQTT::get_IP(){
  return IP;
}
int MQTT::get_ID(){
  return deviceID;
}
int MQTT::send(char* topic, const String* valuesName, int nameCount, const String* values, int valueCount) {
    char buffer[256];

    int err = bundleJsonIntoBufferString(
        valuesName, nameCount, 
        values, valueCount, 
        buffer, sizeof(buffer),
        200
    );

    // Check if JSON was created successfully
    if (err != 0) {
        Serial.print("Error in JSON creation: ");
        Serial.println(err);
        return err;  // Return error code
    }

    // Debug Output
    Serial.println("✅ JSON created successfully. Output:");
    Serial.println(buffer);

    // Publish to MQTT
    bool pubSuccess = client.publish(topic, buffer);
    if (!pubSuccess) {
        Serial.println("❌ MQTT publish failed!");
        return -2;  // Return error if MQTT publish fails
    }

    Serial.println("✅ MQTT message sent successfully!");
    return 0;  // Success
}


int MQTT::bundleJsonIntoBufferString(
    const String* valueNames, size_t valueNamesCount,
    const String* values, size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    int capacityJson // Pass deviceId explicitly
) {
  /*
    Function to bundle data into JSON for sending via MQTT
    Return Codes:
    0 - OK
    1 - Not enough values (valueNamesCount > valuesCount)
    2 - Not enough names (valueNamesCount < valuesCount)
    3 - Overflow of JSON file (too small capacityJson)
    4 - Output buffer too small (truncated JSON)
  */
 Serial.print("Number of values");
 Serial.println(valueNamesCount);
  // Check if the number of names and values match
  if (valueNamesCount > valuesCount) {
    Serial.println("[ERROR] Not enough values!");
    return 1;
  }
  if (valueNamesCount < valuesCount) {
    Serial.println("[ERROR] Not enough names!");
    return 2;
  }

  // Allocate space for JSON
  DynamicJsonDocument json(capacityJson); // Use DynamicJsonDocument for runtime capacity

  // Add deviceId
  json["deviceId"] = deviceID;

  // Add (name -> value) pairs
  for (size_t i = 0; i < valueNamesCount; i++) {
    json[valueNames[i]] = values[i];

    // Check for memory overflow
    if (json.memoryUsage() > json.capacity()) {
      Serial.println("[ERROR] Not enough memory to add new items!");
      return 3;
    }
  }

  // Serialize JSON into outBuffer
  size_t bytesWritten = serializeJson(json, outBuffer, bufferSize);
  if (bytesWritten + 1 >= bufferSize) { // Ensure space for null terminator
    Serial.println("[ERROR] JSON output truncated!");
    return 4;
  }

  return 0; // Success
}

// TODO: Add error if device number is -1. Trying send data before connection was establish

int MQTT::bundleJsonIntoBufferFloat(
    const String* valueNames, size_t valueNamesCount,
    const float*  values,     size_t valuesCount,
    char* outBuffer, size_t bufferSize,
    const int capacityJson
) {
  /*
  Function to bundle data into JSON for sending via MQTT
  Return Codes:
    0 - OK
    1 - Not enough values (valueNamesCount > valuesCount)
    2 - Not enough names (valueNamesCount < valuesCount)
    3 - Overflow of JSON document (too small capacityJson)
    4 - Output buffer too small (truncated JSON)
  */

  // Check if the number of names and values are mismatched
  if (valueNamesCount > valuesCount) {
    Serial.println("[ERROR] Not enough values!");
    return 1;
  }
  if (valueNamesCount < valuesCount) {
    Serial.println("[ERROR] Not enough names!");
    return 2;
  }

  // Create a DynamicJsonDocument with runtime-defined capacity
  DynamicJsonDocument json(capacityJson);

  // Add deviceId to JSON
  json["deviceId"] = deviceID;

  // Add name-value pairs to JSON
  for (size_t i = 0; i < valueNamesCount; i++) {
    json[valueNames[i]] = values[i];
  }

  // Serialize JSON into outBuffer
  size_t bytesWritten = serializeJson(json, outBuffer, bufferSize);
  if (bytesWritten + 1 > bufferSize) { // Ensure null terminator fits
    Serial.println("[ERROR] JSON output truncated!");
    return 4;
  }

  return 0; // Success
}

int MQTT::begin(){
  client.loop();
  return 0;
}