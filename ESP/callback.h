#ifndef CALLBACK_H
#define CALLBACK_H

#include <PubSubClient.h> // Library on GitHub 
#include "MQTT.h"
#include <string.h>


void callback(char* topic, byte* payload, unsigned int length); // MQTT callback

#endif