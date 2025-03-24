#include "LTR390.h"
#include <Arduino.h>

#define UV_SENSITIVITY 2300
#define WFAC 1.2

#define LTR390_NOT_FOUND 7000
LTR390::LTR390() {}

int LTR390::begin() {
    if (!ltr.begin()) {
        Serial.println("LTR390 not found!");
        return LTR390_NOT_FOUND;
    }
    configureSensor();
    return OK;
}

void LTR390::configureSensor()
{
    ltr.setMode(LTR390_MODE_UVS);
    ltr.setResolution(LTR390_RESOLUTION_20BIT);
    ltr.setGain(LTR390_GAIN_3);
}


void LTR390::readUV(float *UV) {
    if (ltr.newDataAvailable())
    {
      
      *UV = ltr.readUVS();
      *UV = (*UV / UV_SENSITIVITY) * WFAC;
    }
}
