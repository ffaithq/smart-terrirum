#ifndef Config_h
#define Config_h



#define DESCRIPTION "Internal ESP" // Description which describe something about ESP (where,what)
#define BoardPIN 2
#define PERIOD_MEASUREMENT 25000 //300000  //  In millisecodnds. 5 minutes in milliseconds (300,000 ms = 5 minutes)
#define ID 1

#define WIFI_SSID "apres-ski" // TODO: Replace for SSID which will be used
#define WIFI_PASSWORD "penske963_vamos" // TODO: Replace password which will be used
#define IP_MQTT_SERVER "192.168.0.44"// TODO: Replace  MQTT Broker IP address 
#define NAME_MQTT_CLIENT "ESP32_Internal"

#define TOPIC_TO_SEND "esp/internal"
#define TOPIC_FOR_ID "rpi/list_ids"
#define TOPIC_FOR_SETPOINTS "rpi/setpoints"
#define TOPOIC_TO_CONNECT "rpi/conn_esp"

#define LEDpin 23
#define PUMP 18
#define UVpin 19
#define BUTTON 33
#define LEVEL_SENSOR 32

#define SCREEN_WIDTH 128  // OLED width
#define SCREEN_HEIGHT 64  // OLED height
#define OLED_RESET    -1  // Reset pin (not used)

#define WARMUP_TIME 170

#define DEBUG true

#define OK                          0

#define RESTART_ESP                 1

// For Catnip
#define HUMIDITY_SOIL_LOW_C 246
#define HUMIDITY_SOIL_HIGH_C 500
#define HUMIDITY_SOIL_FILTER_SIZE 50

#define RXD2 16
#define TXD2 17

#define TEMP_AIR         0
#define HUM_AIR          1
#define TEMP_SOIL        2
#define HUM_SOIL         3
#define UV_ID            4
#define PRESS_AIR        5
#define CO2_ID           6
#define IAQ              7


#define SATURATE(value, min_value, max_value) ((value) < (min_value) ? (min_value) : ((value) > (max_value) ? (max_value) : (value)))

extern float waterLevel;
extern float uvLevel;


#endif