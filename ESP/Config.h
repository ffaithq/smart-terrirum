#ifndef Config_h
#define Config_h



#define DESCRIPTION "tst ESP" // Description which describe something about ESP (where,what)
#define BoardPIN 2
#define Own_Topic   "Own"
// CONSTANT PART - Setting
#define WIFI_SSID "apres-ski" // TODO: Replace for SSID which will be used
#define WIFI_PASSWORD "penske963_vamos" // TODO: Replace password which will be used
#define IP_MQTT_SERVER "192.168.0.175"// TODO: Replace  MQTT Broker IP address 

#define TOPIC_FOR_ID "rpi/list_ids"
#define TOPIC_FOR_SETPOINTS "rpi/setpoints"
#define TOPOIC_TO_CONNECT "rpi/conn_esp"

#define NAME_MQTT_CLIENT "ESP_MQTT"

#define INIT_DEVICE_ID -1
#define DEBUG true

#define OK                          0x0

#define RESTART_ESP                 -0x0001


#define SET_SERVER_SUCCES           -0x0100
#define SET_SERVER_FAIL             -0x0101


#define WIFI_CONNECTING_BEGIN       -0x1000
#define WIFI_CONNECTING_WAIT        -0x1001
#define WIFI_CONNECTED              -0x1002
#define WIFI_FAIL_CONNECTION        -0x1003


#define MQTT_WIFI_NOT_CONNECTED     -0x2001
#define MQTT_CONNECTED              -0x2002
#define MQTT_FAIL_BEGIN_LOOP        -0x2003
#define MQTT_FAIL_DISCONNECT        -0x2004
#define MQTT_FAIL_PUBLISH           -0x2005


#define JH_NOT_ENOUGH_VALUES        -0x2101
#define JH_NOT_ENOUGH_NAMES         -0X2102
#define JH_OVERFLOW_JSON            -0x2103
#define JH_TRUNCUT_BUFFER           -0x2104
#define JH_ID_NEGATIVE              -0x2105


#define IP_ISNOT_ASSIGNED           -0x3000
#define ID_ISNOT_ASSIGNED           -0x3001
#define FAIL_TO_ASSIGN_TOPIC        -0x3002
#define FAIL_TO_ASSIGN_NAME         -0x3003
#define FAIL_TO_ASSIGN_ID           -0x3004

extern float waterLevel;
extern float uvLevel;


#endif