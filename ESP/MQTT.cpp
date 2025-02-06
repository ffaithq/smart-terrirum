#include "MQTT.h"

MQTT::MQTT(int id) : client(espClient), deviceID(id) {
  sprintf(name, NAME_MQTT_CLIENT);
  client.setServer(IP_MQTT_SERVER, 1883);
}

int MQTT::setup_WIFI(){
    delay(50);

    if (DEBUG){
      Serial.print("\n[WiFi] Connecting to ");
      Serial.println(WIFI_SSID);
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int c = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
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
  
  while (!client.connected()) {
    // First ensure Wi-Fi is still up
    if (WiFi.status() != WL_CONNECTED) {
      if(DEBUG){
        Serial.println("[MQTT setup][ERROR] WIFI is not connected");
      }
      return MQTT_WIFI_NOT_CONNECTED;
    }
    
    if(DEBUG){
      Serial.print("[MQTT setup][INFO] Attempting MQTT connection with client name: ");
      Serial.println(NAME_MQTT_CLIENT);
    }


    // Attempt to connect
    if (client.connect(name)) {
      if (DEBUG){
        Serial.println("[MQTT setup] Connected to broker");
      }

      if (deviceID < 0) {
        if (DEBUG){
          Serial.println("[MQTT setup] Subscribing to 'rpi/list_ids' to obtain ID");
        }

        client.subscribe(TOPIC_FOR_ID);
        client.setCallback(callback);

        const String valueNames[] = {"IP", "Description"};
        const String values[] = {IP, DESCRIPTION};

        send(
          TOPOIC_TO_CONNECT,valueNames,
          sizeof(valueNames)/sizeof(valueNames[0]),
          values,sizeof(values)/sizeof(values[0])
          );
      } 
      else {
        if (DEBUG){
          Serial.print("[MQTT setup] Already have ID: ");
          Serial.println(deviceID);
          Serial.print("[MQTT setup] Will publish to topic: ");
          Serial.println(topic);
        }

        client.subscribe("rpi/setpoints");
        client.setCallback(callback);
      }

    } else {

      if(DEBUG){
        Serial.print("[MQTT setup] Connection failed, rc=");
        Serial.print(client.state());
        Serial.println(" - retry in 2 seconds");
      }

      delay(2000);
    }
  }
  return MQTT_CONNECTED;
}

int MQTT::disconect_MQTT(){

  try
  {
    client.disconnect();
  }
  catch (const std::exception& e)
  {
      // Handle the exception
      if (DEBUG){
        Serial.println("Error: ");
        Serial.println(e.what());  
      }
      return MQTT_FAIL_DISCONNECT;
  }
  return OK;
}

int MQTT::assign_ID(int id){ 
  if (id < 0){
      if (DEBUG){
        Serial.print("[MQTT assign ID]Try to assign id which is less that zero");
      }
      return FAIL_TO_ASSIGN_ID;
  }
  deviceID = id;
  return OK;
}

int MQTT::assign_name(char *n){
    try
    {
      strncpy(name, n, sizeof(name) - 1);
      name[sizeof(name) - 1] = '\0';
    }
    catch (const std::exception& e)
    {
        if (DEBUG){
          Serial.println("Error: ");
          Serial.println(e.what());  
        }
        return FAIL_TO_ASSIGN_TOPIC;
    }
    return OK;
}

int MQTT::assign_topic(char *t){
    try
    {
      strncpy(topic, t, sizeof(topic) - 1);
      topic[sizeof(topic) - 1] = '\0';
    }
    catch (const std::exception& e)
    {
        if (DEBUG){
          Serial.println("Error: ");
          Serial.println(e.what());  
        }
        return FAIL_TO_ASSIGN_TOPIC;
    }
    return OK;
}

int MQTT::begin(){
  try
  {
      client.loop();
  }
  catch (const std::exception& e)
  {
      if (DEBUG){
        Serial.println("Error: ");
        Serial.println(e.what());  
      }
      return MQTT_FAIL_BEGIN_LOOP;
  }
  return OK;
}
          
int MQTT::get_IP(String *pIP){
  if (IP.isEmpty()){
    return IP_ISNOT_ASSIGNED;
  }

  *pIP = IP;
  return OK;
}

int MQTT::get_ID(int *pID){
  if (deviceID < 0){
    return ID_ISNOT_ASSIGNED;
  }
  *pID = deviceID;

  return OK;
}

int MQTT::send(char* topic2Send, const String* valuesName, int nameCount, const String* values, int valueCount) {
    char buffer[256];
    if (strcmp(topic2Send, Own_Topic) == 0){
      topic2Send = topic; 
    }
    int err = bundleJsonIntoBuffer(
        valuesName, nameCount, 
        values, valueCount, 
        buffer, sizeof(buffer),
        200,deviceID
    );

    if (err != OK){
        if (DEBUG){
          Serial.println("[MQTT SEND][ERROR] Error from bundleJson");
          Serial.print("ERROR CODE: "); Serial.print(err);
        }
        return err;
    }

    // Debug Output
    if(DEBUG){
      Serial.println("[MQTT SEND][OK] JSON created successfully. Output:");
      Serial.println(buffer);
    }


    // Publish to MQTT
    bool pubSuccess = client.publish(topic2Send, buffer);
    if (!pubSuccess) {
        if(DEBUG){
          Serial.println("[MQTT SEND][ERROR]MQTT publish failed!");
        }
        
        return MQTT_FAIL_PUBLISH;
    }
    if(DEBUG){
      Serial.println("[MQTT SEND][OK] MQTT message sent successfully!");
    }
   
    return OK;  // Success
}


int MQTT::send(char* topic2Send, const String* valuesName, int nameCount, const float* values, int valueCount) {
    char buffer[256];
    if (strcmp(topic2Send, Own_Topic) == 0){
      topic2Send = topic; 
    }
    int err = bundleJsonIntoBuffer(
        valuesName, nameCount, 
        values, valueCount, 
        buffer, sizeof(buffer),
        200,deviceID
    );

    if (err != OK){
        if (DEBUG){
          Serial.println("[MQTT SEND][ERROR] Error from bundleJson");
          Serial.print("ERROR CODE: "); Serial.print(err);
        }
        return err;
    }

    // Debug Output
    if(DEBUG){
      Serial.println("[MQTT SEND][OK] JSON created successfully. Output:");
      Serial.println(buffer);
    }


    // Publish to MQTT
    bool pubSuccess = client.publish(topic2Send, buffer);
    if (!pubSuccess) {
        if(DEBUG){
          Serial.println("[MQTT SEND][ERROR]MQTT publish failed!");
        }
        
        return MQTT_FAIL_PUBLISH;
    }
    if(DEBUG){
      Serial.println("[MQTT SEND][OK] MQTT message sent successfully!");
    }
   
    return OK;  // Success
}
