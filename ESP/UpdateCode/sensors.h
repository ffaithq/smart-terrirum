#ifndef SENSORS_H
#define SENSORS_H

#include "BME680.h"
#include "catnip.h"
#include "LTR390.h"
#include "MHZ19B.h"
#include "ERROR.h"
#include "Config.h"
//#include "SSD1306.h"

// Compute saturation


// Temprature of air
BME680 bme680;

// Humidity of soild 
Catnip catnip_1(0x20);
Catnip catnip_2(0x21);

// UV sensor
LTR390 ltr390;
//SSD1306 ssd1306;

// CO2 sensor
MHZ19B mhz19b(Serial2, 16, 17);


#endif // SENSORS_H
