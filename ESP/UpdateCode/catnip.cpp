#include "Catnip.h"


Catnip::Catnip(uint8_t address) : sensor(address), _address(address) {}

int Catnip::begin() {
    sensor.begin(_address);
    return OK;
}


int Catnip::readData(float *humSoil, float *tempSoil) {

    float humSum = 0;
    float tempHumiditySoil = 0;
    float tempSum = 0;

    for (int i = 0; i < HUMIDITY_SOIL_FILTER_SIZE; i++) {
        humSum += SATURATE(sensor.getCapacitance(), HUMIDITY_SOIL_LOW_C, HUMIDITY_SOIL_HIGH_C);
        tempSum += sensor.getTemperature() / 10.0;
    
    }

    tempHumiditySoil = ((humSum / HUMIDITY_SOIL_FILTER_SIZE) - HUMIDITY_SOIL_LOW_C) /
              (HUMIDITY_SOIL_HIGH_C - HUMIDITY_SOIL_LOW_C) * 100;
    *humSoil = SATURATE(tempHumiditySoil, 0, 100); //0-100%
  
    *tempSoil = tempSum / HUMIDITY_SOIL_FILTER_SIZE;

    if (tempHumiditySoil > *humSoil){
      return VALUE_IS_SATURATED;
    }
    return OK;
}
