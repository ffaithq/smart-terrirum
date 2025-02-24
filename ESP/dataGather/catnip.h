#ifndef CATNIP_H
#define CATNIP_H

#include "I2CSoilMoistureSensor.h"

class Catnip {
public:
    Catnip(uint8_t address);
    void begin();
    void readData(float &humSoil, float &tempSoil);
private:
    I2CSoilMoistureSensor sensor;
};

#endif // CATNIP_H
