#ifndef MHZ19B_H
#define MHZ19B_H

#include <Arduino.h>
#include <ErriezMHZ19B.h>


class MHZ19B {
public:
    MHZ19B(HardwareSerial &serial, int rxPin, int txPin);
    void begin();
    int readCO2(float &co2);
    bool isReady();
    void calibrateZero();

private:
    HardwareSerial &mhzSerial;
    ErriezMHZ19B mhz19b;
    int rxPin;
    int txPin;
};

#endif //MHZ19B_H