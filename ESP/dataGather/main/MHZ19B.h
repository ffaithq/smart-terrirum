#ifndef MHZ19B_H
#define MHZ19B_H

#include <Arduino.h>
#include <ErriezMHZ19B.h>
#include "SSD1306.h"

#define WARMUP_TIME 170

class MHZ19B {
public:
    MHZ19B(HardwareSerial &serial, int rxPin, int txPin, SSD1306 &ssd1306);
    void begin();
    int readCO2(float &co2);
    bool isReady();
    void calibrateZero();

private:
    HardwareSerial &mhzSerial;
    ErriezMHZ19B mhz19b;
    SSD1306 &ssd1306; 
    int rxPin;
    int txPin;
};

#endif //MHZ19B_H