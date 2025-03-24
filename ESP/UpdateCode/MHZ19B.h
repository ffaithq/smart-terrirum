#ifndef MHZ19B_H
#define MHZ19B_H

#include <Arduino.h>
#include <ErriezMHZ19B.h>
#include "esp32-hal-gpio.h"
#include "HardwareSerial.h"

#include "Config.h"
#include "ERROR.h"

class MHZ19B {
public:
    MHZ19B(HardwareSerial &serial, int rxPin, int txPin);
    int begin(bool firstCALL);
    int readCO2(float *co2);
    bool isReady();

    // IS IT NEEDED ?? NOT USED ANYWHERE
    int calibrateZero();

private:
    HardwareSerial &mhzSerial;
    ErriezMHZ19B mhz19b;
    int rxPin;
    int txPin;
};

#endif //MHZ19B_H