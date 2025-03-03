#ifndef CATNIP_H
#define CATNIP_H

#include "I2CSoilMoistureSensor.h"

#define HUMIDITY_SOIL_LOW_C 246
#define HUMIDITY_SOIL_HIGH_C 500
#define HUMIDITY_SOIL_FILTER_SIZE 50

class Catnip {
public:
    Catnip(uint8_t address);
    void begin();
    void readData(float &humSoil, float &tempSoil);
private:
    I2CSoilMoistureSensor sensor;
    uint8_t _address;
};

#endif // CATNIP_H
