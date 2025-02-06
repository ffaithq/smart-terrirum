/* 
  Arduino project for ESP32
  Project: Smart Terrarium
  Subject: Master Project 2
  Author: Vojtech Micek, Stepan Fatov, Roman Mikulin, Jakub Madzia
*/

#include "Config.h"
#include <LiquidCrystal_I2C.h>
#include "MQTT.h"

int lcdColumns = 16;
int lcdRows = 2;
float waterLevel = 0;
float uvLevel = 0;
int status;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
MQTT mqtt(INIT_DEVICE_ID);


void setup() {
  Serial.begin(115200);
  status = mqtt.setup_WIFI();
  if (status != WIFI_CONNECTED){
    ESP.restart();
  }
  mqtt.setup_MQTT();

  lcd.init();                   
  lcd.backlight();
}

void loop() { 
    int c = 0;
    while (mqtt.begin() != OK) {
        delay(1000);
        if (DEBUG){
          Serial.println("\n[MQTT BEGIN] Wait till mqtt client is started");
          Serial.print("Try: ");
          Serial.print(c);
          Serial.print("/10\n");
        }

        c++;

        if (c > 10) {
            if (DEBUG){
              Serial.println("\n[MQTT BEGIN] Fail to start loop. Wait too long");
            }
            ESP.restart();
        }
    }

    lcd.setCursor(0, 0);
    lcd.print("IP address:");
    lcd.setCursor(0, 1);
    String IP;
    status = mqtt.get_IP(&IP);
    lcd.print(IP);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("device ID:");
    lcd.setCursor(0, 1);
    int ID;
    status = mqtt.get_ID(&ID);
    lcd.print(ID);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Water Level:");
    lcd.setCursor(0, 1);
    lcd.print(waterLevel);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("UV level:");
    lcd.setCursor(0, 1);
    lcd.print(uvLevel);
    delay(1000);
    lcd.clear();

    String names[] = {"temperature"};
    float  vals[]  = {23.4};

    mqtt.send(Own_Topic, names, 1, vals, 1);
}
