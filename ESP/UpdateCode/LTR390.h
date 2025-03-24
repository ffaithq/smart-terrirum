#ifndef LTR390_H
#define LTR390_H

#include "Adafruit_LTR390.h"
#include "Config.h"
#include "ERROR.h"

class LTR390 {
public:
    LTR390();
    int begin();
    void readUV(float *UV);
    void configureSensor();
private:
    Adafruit_LTR390 ltr;
};

#endif
