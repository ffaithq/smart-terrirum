#ifndef SENSORS_H
#define SENSORS_H

#include "BME680.h"
#include "catnip.h"
#include "LTR390.h"
#include "MHZ19B.h"


BME680 bme680;
Catnip catnip_1(0x20);
Catnip catnip_2(0x21);
LTR390 ltr390;
MHZ19B mhz19b(Serial2, 16, 17);

#endif // SENSORS_H
