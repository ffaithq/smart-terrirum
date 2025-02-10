#ifndef Config_h
#define Config_h



#define DESCRIPTION "tst ESP" // Description which describe something about ESP (where,what)
#define BoardPIN 2
#define Own_Topic   "Own"
// CONSTANT PART - Setting
#define WIFI_SSID "apres-ski" // TODO: Replace for SSID which will be used
#define WIFI_PASSWORD "penske963_vamos" // TODO: Replace password which will be used
#define IP_MQTT_SERVER "192.168.0.44"// TODO: Replace  MQTT Broker IP address 

#define TOPIC_FOR_ID "rpi/list_ids"
#define TOPIC_FOR_SETPOINTS "rpi/setpoints"
#define TOPOIC_TO_CONNECT "rpi/conn_esp"

#define NAME_MQTT_CLIENT "ESP_MQTT"

#define INIT_DEVICE_ID              -1
#define DEBUG true

#define OK                          0

#define RESTART_ESP                 1


#define SET_SERVER_SUCCES           100
#define SET_SERVER_FAIL             101


#define WIFI_CONNECTING_BEGIN       1000
#define WIFI_CONNECTING_WAIT        1001
#define WIFI_CONNECTED              1002
#define WIFI_FAIL_CONNECTION        1003


#define MQTT_WIFI_NOT_CONNECTED     2001
#define MQTT_CONNECTED              2002
#define MQTT_FAIL_BEGIN_LOOP        2003
#define MQTT_FAIL_DISCONNECT        2004
#define MQTT_FAIL_PUBLISH           2005


#define JH_NOT_ENOUGH_VALUES        2101
#define JH_NOT_ENOUGH_NAMES         2102
#define JH_OVERFLOW_JSON            2103
#define JH_TRUNCUT_BUFFER           2104
#define JH_ID_NEGATIVE              2105


#define IP_ISNOT_ASSIGNED           3000
#define ID_ISNOT_ASSIGNED           3001
#define FAIL_TO_ASSIGN_TOPIC        3002
#define FAIL_TO_ASSIGN_NAME         3003
#define FAIL_TO_ASSIGN_ID           3004

extern float waterLevel;
extern float uvLevel;


#endif