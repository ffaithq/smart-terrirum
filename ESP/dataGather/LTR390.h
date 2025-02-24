#ifndef LTR390_H
#define LTR390_H

#include "Adafruit_LTR390.h"

class LTR390 {
public:
    LTR390();
    void begin();
    float readUV(float &UV);
    void configureSensor();
private:
    Adafruit_LTR390 ltr;
};

#endif // LTR390_H
