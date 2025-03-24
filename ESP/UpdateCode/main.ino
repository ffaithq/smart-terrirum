#include <Wire.h>
#include <SPI.h>
#include "Arduino.h"
#include "JsonHandler.h"
#include <WiFi.h> 
#include <PubSubClient.h>
#include "Config.h"
#include "ERROR.h"
#include "sensors.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

WiFiClient espClient;
PubSubClient client(espClient);

struct SharedData {
  int                  statusDAQ;
  int                  statusMQTT;
  unsigned short int   counter;
  float                measurements[8];  // ✅ Fixed syntax
  bool                 validData;
};


const char* keys_of_values[8] = {
  "Tair",
  "Hair",
  "Tsoil",
  "Hsoil",
  "UV",
  "Pair",
  "co2",
  "IAQ"
};

SharedData shared = {0,0, 0, {0}, false};  // Initialize with zeros
SemaphoreHandle_t xMutex;  // Protects sharedData1

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void DAQ_task(void *parameter) {

  SharedData* data = (SharedData*) parameter;

  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    data->statusDAQ = bme680.begin();
    Serial.print("BME680: ");
    Serial.println(data->statusDAQ);
    xSemaphoreGive(xMutex);
  }
  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    data->statusDAQ = ltr390.begin();

    Serial.print("ltr390: ");
    Serial.println(data->statusDAQ);
    xSemaphoreGive(xMutex);
  }
  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    data->statusDAQ = catnip_1.begin();

    Serial.print("catnip_1: ");
    Serial.println(data->statusDAQ);
    xSemaphoreGive(xMutex);
  }
  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    data->statusDAQ = catnip_2.begin();

    Serial.print("catnip_2: ");
    Serial.println(data->statusDAQ);
    xSemaphoreGive(xMutex);
  }

  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    data->statusDAQ = mhz19b.begin(true);
    xSemaphoreGive(xMutex);
  }

  while (data->statusDAQ != OK){
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      data->statusDAQ = mhz19b.begin(false);
      Serial.print("MHZ19B: ");
      Serial.println(data->statusDAQ);
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }

  float humSoil1, humSoil2, tempSoil1, tempSoil2;
  while (true) {

    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      bme680.readData(
          &data->measurements[TEMP_AIR],
          &data->measurements[HUM_AIR],
          &data->measurements[PRESS_AIR],
          &data->measurements[IAQ]
      );

      catnip_1.readData(&humSoil1, &tempSoil1);
      catnip_2.readData(&humSoil2, &tempSoil2);

      data->measurements[HUM_SOIL] = (humSoil1 + humSoil2) / 2.0;
      data->measurements[TEMP_SOIL] = (tempSoil1 + tempSoil2) / 2.0;


      mhz19b.readCO2(&data->measurements[CO2_ID]);
      ltr390.readUV(&data->measurements[UV_ID]);
      data->counter += 1;
      xSemaphoreGive(xMutex);  // Release the mutex
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void OLED_task(void *parameter) {
  SharedData* data = (SharedData*) parameter;

  // Initialize OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true){
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    } // Halt if OLED fails
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("Init message");
  oled.display();
  vTaskDelay(10000 / portTICK_PERIOD_MS);

  float copiedData[8];
  int status;
  

  while (true) {
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      for (int i = 0; i < 8; i++) {
        copiedData[i] = data->measurements[i];
      }
      status = data->statusDAQ;
      xSemaphoreGive(xMutex);
    }

    for (int i = 0; i < 8; i++) {
      oled.clearDisplay();
      oled.setCursor(0, 0);
      oled.print(keys_of_values[i]);  // e.g. "tempAir"
      oled.setCursor(0, 16);
      oled.print(copiedData[i], 2);   // Print value with 2 decimal places
      oled.display();

      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void MQTT_task(void *prm){
  SharedData* data = (SharedData*) prm;
  client.setCallback(callback);


  vTaskDelay(1000 / portTICK_PERIOD_MS);

  float copiedData[8];
  int status;
  unsigned short int   counter = 0;
  unsigned short int   previous_counter = 0;

  while (true) {
    while (!client.connected()) {
    if (client.connect(NAME_MQTT_CLIENT)) {
      client.subscribe("rpi/control");
      } else {
        Serial.println("Can not connect MQTT");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
      }
    }
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      for (int i = 0; i < 8; i++) {
        copiedData[i] = data->measurements[i];
      }
      status = data->statusDAQ;
      counter = data->counter;
      xSemaphoreGive(xMutex);
    }
    if (counter != previous_counter){
      char buffer[256];

      int err = bundleJsonIntoBuffer(
        keys_of_values, 8,
        copiedData, 8,
        buffer, sizeof(buffer),
        200, 1);

      client.publish(TOPIC_TO_SEND, buffer);  // Example temperature data

      previous_counter = counter;
    }
    digitalWrite(LEDpin,HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LEDpin,LOW);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

void Actuator_task(void *prm){
  while(1){
    if (!digitalRead(BUTTON)){
      digitalWrite(PUMP,HIGH);
      Serial.println("Actuator ON");
    }
    else {
      digitalWrite(PUMP,LOW);
      Serial.println("Actuator OFF");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

}

void setup() {
  
  Serial.begin(115200);
  xMutex = xSemaphoreCreateMutex();
  if (xMutex == NULL) {
    Serial.println("Mutex creation failed!");
    while (true);  // Stop everything
  }
  pinMode(BUTTON, INPUT);        // Set the button pin as an input
  pinMode(LEVEL_SENSOR, INPUT);  // Set the button pin as an input

  pinMode(PUMP, OUTPUT);    // Set the button pin as an input
  pinMode(UVpin, OUTPUT);   // Set the button pin as an input
  pinMode(LEDpin, OUTPUT);  // Set the button pin as an input

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  client.setServer(IP_MQTT_SERVER, 1883);
  xTaskCreatePinnedToCore(DAQ_task, "DAQ", 4096, &shared, 1, NULL, 0);
  xTaskCreatePinnedToCore(OLED_task, "OLED", 4096, &shared, 1, NULL, 0);
  xTaskCreatePinnedToCore(MQTT_task, "MQTT", 4096, &shared, 1, NULL, 0);
  xTaskCreatePinnedToCore(Actuator_task, "Actuators_task", 1024, NULL, 1, NULL, 0);
}

void loop() {
  /*
  if (!client.connected()) {

    reconnect();
  }
  client.loop();


    send();
  }

  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(PUMP, HIGH);
  } else {
    digitalWrite(PUMP, LOW);
  }
  */
}

/*



  // Set MQTT server and callback
  client.setServer(IP_MQTT_SERVER, 1883);
  client.setCallback(callback);

void printValues() {
  for (int i = 0; i < 8; i++) {
    Serial.print(KEYS[i]);
    Serial.print(": ");
    Serial.print(String(*values[i]));
    Serial.print("  \t");
  }

  Serial.println();
}
*/

/*



int send(float[] measurement) {
  char buffer[256];

  float values[] = { tempAir, humAir, tempSoil, humSoil, UV, pressAir, co2, gas };
  int err = bundleJsonIntoBuffer(
    KEYS, 8,
    measurement, 8,
    buffer, sizeof(buffer),
    200, 1);

  client.publish(TOPIC_TO_SEND, buffer);  // Example temperature data
  Serial.println("Published temperature data!");

  return 0;
}



// Function to reconnect to MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESPClient")) {  // Change "ESPClient" for multiple devices
      Serial.println("connected!");
      client.subscribe("rpi/commnad");  // Subscribe to a topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}
*/
