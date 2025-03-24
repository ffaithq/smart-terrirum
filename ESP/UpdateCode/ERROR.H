#ifndef ERROR_h
#define ERROR_h

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

#define VALUE_IS_SATURATED 4000

#define MHZ19B_ISNOT_DETECTED 5000
#define MHZ19B_ISNOT_READY 5001
#define MHZ19B_WARMING_UP 5002
#define MHZ19B_CO2_LESS_ZERO 5003


#endif