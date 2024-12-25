#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// DEFINE PART
// TODO: Add error code - can be separated in other file
#define DESCRIPTION "tst ESP" // Description which describe something about ESP (where,what)
#define ledPin 2


// CONSTANT PART - Setting
const char* ssid = "apres-ski"; // TODO: Replace for SSID which will be used
const char* password = "penske963_vamos"; // TODO: Replace password which will be used
const char* mqtt_server = "192.168.0.175"; // TODO: Replace  MQTT Broker IP address 


// BUFFERS
char name[32];  // Name of MQTT device to connect - automaticly assign 
char topic[32]; // Topic to send measurement data - automaticly assign 
int deviceId = -1;    // ID of ESP which has to be the same as in devices database - automaticly assign. -1 means that ID isn't assigned yet

WiFiClient espClient;
PubSubClient client(espClient);


// Simple LED blink function - for debug purpose
// TODO: Make function to indicate error status - function will get (int error) and blink LED which has to be blink
void blink_led(unsigned int times, unsigned int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}


int bundleJsonIntoBuffer(
    const String* valueNames, size_t valueNamesCount,
    const String*  values,     size_t valuesCount,
    int deviceId,
    char* outBuffer, size_t bufferSize,
    int capacityJson)
{
  // 1) Compare array sizes
  if (valueNamesCount > valuesCount) {
    Serial.println("[ERROR] Not enough values!");
    return 1;
  }
  if (valueNamesCount < valuesCount) {
    Serial.println("[ERROR] Not enough names!");
    return 2;
  }

  // 2) Create a StaticJsonDocument of 'capacityJson' bytes
  StaticJsonDocument<200> json;  // Or: StaticJsonDocument<capacityJson> json;
  // (Note: 'capacityJson' must be a compile-time constant in template form:
  //  StaticJsonDocument<someInteger>.)

  // 3) Insert "deviceId"
  json["deviceId"] = deviceId;

  // 4) Insert each (name -> value) pair
  for (size_t i = 0; i < valuesCount; i++) {
    // Example: doc["temperature"] = 23.4
    json[valueNames[i]] = values[i];

    // Check memory usage after adding each item
    if (json.memoryUsage() > json.capacity()) {
      Serial.println("[ERROR] Not enough memory to add new items!");
      return 3;
    }
  }

  // 5) Serialize JSON into outBuffer (up to bufferSize)
  size_t bytesWritten = serializeJson(json, outBuffer, bufferSize);
  if (bytesWritten >= bufferSize) {
    Serial.println("[ERROR] JSON output truncated!");
    return 4;
  }

  // 6) Success
  return 0;
}


int bundleJsonIntoBuffer(
    const String* valueNames, size_t valueNamesCount,
    const float*  values,     size_t valuesCount,
    int deviceId,
    char* outBuffer, size_t bufferSize,
    int capacityJson)
{
  // 1) Compare array sizes
  if (valueNamesCount > valuesCount) {
    Serial.println("[ERROR] Not enough values!");
    return 1;
  }
  if (valueNamesCount < valuesCount) {
    Serial.println("[ERROR] Not enough names!");
    return 2;
  }

  // 2) Create a StaticJsonDocument of 'capacityJson' bytes
  StaticJsonDocument<200> json;  // Or: StaticJsonDocument<capacityJson> json;
  // (Note: 'capacityJson' must be a compile-time constant in template form:
  //  StaticJsonDocument<someInteger>.)

  // 3) Insert "deviceId"
  json["deviceId"] = deviceId;

  // 4) Insert each (name -> value) pair
  for (size_t i = 0; i < valuesCount; i++) {
    // Example: doc["temperature"] = 23.4
    json[valueNames[i]] = values[i];

    // Check memory usage after adding each item
    if (json.memoryUsage() > json.capacity()) {
      Serial.println("[ERROR] Not enough memory to add new items!");
      return 3;
    }
  }

  // 5) Serialize JSON into outBuffer (up to bufferSize)
  size_t bytesWritten = serializeJson(json, outBuffer, bufferSize);
  if (bytesWritten >= bufferSize) {
    Serial.println("[ERROR] JSON output truncated!");
    return 4;
  }

  // 6) Success
  return 0;
}

// Callback function for "rpi/list_ids"
void callback_startup(char* topicReceived, byte* message, unsigned int length) {
  Serial.print("\n[Callback] Message arrived on topic: ");
  Serial.println(topicReceived);

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

    // Convert IP to string to use as the key
    String ipStr = WiFi.localIP().toString();  // e.g., "192.168.0.164"

    // Assign the extracted ID to the global 'id' (do NOT re-declare int here!)
    deviceId = doc[ipStr].as<int>(); 
    Serial.print("[Callback] Received id = ");
    Serial.println(deviceId);

    // Blink the LED 'id' times
    blink_led(deviceId, 1000);

    // We no longer need "rpi/list_ids"
    client.unsubscribe("rpi/list_ids");

    // Optionally disconnect, so we can reconnect with a new client name
    client.disconnect();

    // Create new client name and publish topic
    // e.g., name = "esp_42" and topic = "esp/id_42"
    sprintf(name, "esp_%d", deviceId);
    sprintf(topic, "esp/id_%d", deviceId);

    Serial.print("[Callback] New client name: ");
    Serial.println(name);
    Serial.print("[Callback] New topic: ");
    Serial.println(topic);

    // Reconnect with the new name (see connect_mqttServer below)
  }
}

void setup_wifi() {
  delay(50);
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int c = 0;
  while (WiFi.status() != WL_CONNECTED) {
    blink_led(2, 200);  // Blink LED to indicate Wi-Fi not connected
    delay(1000);
    Serial.print(".");
    c++;
    if (c > 10) {
      // If it takes too long, reboot
      ESP.restart();
    }
  }

  Serial.println("\n[WiFi] Connected");
  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTTserver() {
  // Keep trying until we're connected
  while (!client.connected()) {
    // First ensure Wi-Fi is still up
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }

    Serial.print("[MQTT] Attempting MQTT connection with client name: ");
    Serial.println(name);

    // Attempt to connect
    if (client.connect(name)) {
      Serial.println("[MQTT] Connected to broker");

      // If we don't have an ID yet, subscribe to get it
      if (deviceId < 0) {
        Serial.println("[MQTT] Subscribing to 'rpi/list_ids' to obtain ID");
        client.subscribe("rpi/list_ids");
        client.setCallback(callback_startup);


        char buffer[256];
        String ipStr = WiFi.localIP().toString();
        String names[] = {"IP","Description"};
        String vals[]  = {ipStr,DESCRIPTION};

        // Call the function
        int err = bundleJsonIntoBuffer(
            names, sizeof(names) / sizeof(names[0]),
            vals,  sizeof(vals) / sizeof(vals[0]),
            deviceId,
            buffer, sizeof(buffer),
            200
        );

        if (err == 0) {
          Serial.println("JSON created successfully. Output:");

        } else {
          Serial.print("Error code: ");
          Serial.println(err);
        }

        client.publish("rpi/conn_esp", buffer);
      } 
      else {
        // Already have an ID, so we presumably want to publish on topic = "esp/id_<id>"
        Serial.print("[MQTT] Already have ID: ");
        Serial.println(deviceId);
        Serial.print("[MQTT] Will publish to topic: ");
        Serial.println(topic);
      }

    } else {
      Serial.print("[MQTT] Connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" - retry in 2 seconds");
      blink_led(3, 200);
      delay(2000);
    }
  }
}



void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  // Initial client name before we get an ID
  sprintf(name, "esp_start_up");
  Serial.print("[Setup] Initial client name: ");
  Serial.println(name);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

// TODO: implement function to send data to MQTT server
int SendMeasurement(){
  return 0;
}


void loop() {
  // Reconnect if needed
  if (!client.connected()) {
    connectMQTTserver();
  }

  // Process any incoming messages
  client.loop();

  // If we have an ID, publish to "esp/id_<id>"
  if (deviceId > 0) {
    char buffer[256];
    // Example arrays (3 sensors)
    String names[] = {"temperature"};
    float  vals[]  = {23.4};

    // Call the function
    int err = bundleJsonIntoBuffer(
        names, sizeof(names) / sizeof(names[0]),
        vals,  sizeof(vals) / sizeof(vals[0]),
        deviceId,
        buffer, sizeof(buffer),
        200
    );

    if (err == 0) {
      Serial.println("JSON created successfully. Output:");

    } else {
      Serial.print("Error code: ");
      Serial.println(err);
    }
    client.publish(topic, buffer);

    // Print for debugging
    Serial.print("[Publish] to ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(buffer);
  }

  // Just a delay for demonstration
  delay(1000);
}
