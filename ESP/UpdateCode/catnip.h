#ifndef CATNIP_H
#define CATNIP_H

#include "I2CSoilMoistureSensor.h"
#include "Config.h"
#include "ERROR.h"
#include <Arduino.h>

class Catnip {
public:
    Catnip(uint8_t address);
    int begin();
    int readData(float *humSoil, float *tempSoil);
private:
    I2CSoilMoistureSensor sensor;
    uint8_t _address;
};

#endif // CATNIP_H
