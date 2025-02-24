#ifndef BME680_H
#define BME680_H

#include <Adafruit_BME680.h>

class BME680 {
public:
    BME680();
    void begin();
    void readData(float &tempAir, float &humAir, float &pressAir, float &gas);
    void configureSensor();
    void getGasReference();
    void calculateIAQ(float &humAir, float &gas);
private:
    Adafruit_BME680 bme;
    float gas_reference;
    uint8_t getgasreference_count = 0;
};

#endif // BME680_H
