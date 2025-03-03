#include "Catnip.h"
#include "tools.h"
#include <Arduino.h>



Catnip::Catnip(uint8_t address) : sensor(address), _address(address) {}

void Catnip::begin() {
    sensor.begin(_address);
}


void Catnip::readData(float &humSoil, float &tempSoil) {

    float humSum = 0;
    float tempSum = 0;
    for (int i = 0; i < HUMIDITY_SOIL_FILTER_SIZE; i++) {
        humSum += SATURATE(sensor.getCapacitance(), HUMIDITY_SOIL_LOW_C, HUMIDITY_SOIL_HIGH_C);
        tempSum += sensor.getTemperature() / 10.0;
    
    }
    humSoil = ((humSum / HUMIDITY_SOIL_FILTER_SIZE) - HUMIDITY_SOIL_LOW_C) /
              (HUMIDITY_SOIL_HIGH_C - HUMIDITY_SOIL_LOW_C) * 100;
    humSoil = SATURATE(humSoil, 0, 100); //0-100%
    tempSoil = tempSum / HUMIDITY_SOIL_FILTER_SIZE;
}
