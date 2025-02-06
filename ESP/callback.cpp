#include "callback.h"
#include "Config.h"


void callback(char* topicReceived, byte* message, unsigned int length) {
    int status;
    if(DEBUG){
      Serial.print("\n[Callback Start up][INFO] Message arrived on topic: ");
      Serial.println(topicReceived);
    }

    String messageTemp;
    for (int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    if (DEBUG){
        Serial.print("[Callback Start up][INFO] Payload: ");
        Serial.println(messageTemp);
    }


    // We only care about messages from "rpi/list_ids"
    if (strcmp(topicReceived, TOPIC_FOR_ID) == 0) {
        // Parse JSON to get our ID
        StaticJsonDocument<200> doc;

        DeserializationError error = deserializeJson(doc, messageTemp);
        if (error) {
            if(DEBUG){
                Serial.print("[Callback] JSON parse failed: ");
                Serial.println(error.c_str());
            }

            return;
        }
        String IP;
        status = mqtt.get_IP(&IP);
        if (status != OK){
            if (IP.isEmpty()){
                if(DEBUG){
                    Serial.println("[MQTT CALLBACK STARTUP][ERROR] Status OK, but IP is still empty");
                    Serial.print("Error code:"); Serial.print(status);
                }
            }
            if(DEBUG){
                Serial.println("[MQTT CALLBACK STARTUP][ERROR] Error while getting IP");
                Serial.print("Error code:"); Serial.print(status);
            }
        }
        int id = doc[IP].as<int>(); 
        if(DEBUG){
            Serial.print("[Callback] Received id = ");
            Serial.println(id);
        }

        status = mqtt.assign_ID(id);
        if (status != OK){
            if(DEBUG){
                Serial.println("[MQTT CALLBACK STARTUP][ERROR] Error while assinging id");
                Serial.print("Error code:"); Serial.print(status);
            }
        }
        char name[32];
        char topic[32];
        sprintf(name, "esp_%d", id);
        sprintf(topic, "esp/id_%d", id);

        status = mqtt.assign_name(name);
        if (status != OK){
            if(DEBUG){
                Serial.println("[MQTT CALLBACK STARTUP][ERROR] Error while assinging name");
                Serial.print("Error code:"); Serial.print(status);
            }
        }
        status = mqtt.assign_topic(topic);
        if (status != OK){
            if(DEBUG){
                Serial.println("[MQTT CALLBACK STARTUP][ERROR] Error while assinging topic");
                Serial.print("Error code:"); Serial.print(status);
            }
        }
        if(DEBUG){
            Serial.print("[Callback] New client name: ");
            Serial.println(name);
            Serial.print("[Callback] New topic: ");
            Serial.println(topic);
        }

        status = mqtt.disconect_MQTT();
        if (status != OK){
            if(DEBUG){
                Serial.println("[MQTT CALLBACK STARTUP][ERROR] Error while disconnect from server");
                Serial.print("Error code:"); Serial.print(status);
            }
        }
        status = mqtt.setup_MQTT();
        if (status != OK){
            if(DEBUG){
                Serial.println("[MQTT CALLBACK STARTUP][ERROR] Error while reconnect with new name");
                Serial.print("Error code:"); Serial.print(status);
            }
        }
    }
    else if (strcmp(topicReceived, TOPIC_FOR_SETPOINTS) == 0) {
        // Parse JSON to get our ID
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, messageTemp);
        if (error) {
            if (DEBUG){
                Serial.print("[Callback] JSON parse failed: ");
                Serial.println(error.c_str());
            }

            return;
        }
        waterLevel = doc["waterLevel"].as<int>(); 
        uvLevel = doc["uvLevel"].as<int>();
    }

}