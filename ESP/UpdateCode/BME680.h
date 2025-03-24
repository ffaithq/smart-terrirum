#ifndef BME680_H
#define BME680_H

#include <Adafruit_BME680.h>
#include "Config.h"
#include "ERROR.h"

class BME680 {
public:
    BME680();
    int begin();
    void readData(float *tempAir, float *humAir, float *pressAir, float *gas);
    void configureSensor();
    void calculateIAQ(float *humAir, float *gas);
private:
    Adafruit_BME680 bme;
};

#endif // BME680_H
