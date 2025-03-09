#include "LTR390.h"
#include <Arduino.h>
#define UV_SENSITIVITY 2300
LTR390::LTR390() {}

void LTR390::begin() {
    if (!ltr.begin()) {
        Serial.println("LTR390 not found!");
        while (1);
    }
    configureSensor();
}

void LTR390::configureSensor()
{
    ltr.setMode(LTR390_MODE_UVS);
    ltr.setResolution(LTR390_RESOLUTION_20BIT);
    ltr.setGain(LTR390_GAIN_3);
}


void LTR390::readUV(float &UV) {
    if (ltr.newDataAvailable())
    {
      UV = ltr.readUVS();
      UV = UV / UV_SENSITIVITY;
       //UV = ltr.readUVS();
    }
}
