#include "MHZ19B.h"
#include "Arduino.h"

#define RXD2 16
#define TXD2 17

MHZ19B::MHZ19B(HardwareSerial &serial, int rxPin, int txPin)
    : mhzSerial(serial), rxPin(rxPin), txPin(txPin), mhz19b(&serial) {}

void MHZ19B::begin() {
    mhz19b.setRange5000ppm();
    Serial2.begin(9600, SERIAL_8N1, rxPin, txPin);
    mhz19b.setAutoCalibration(true);
    while (!mhz19b.detect()) {
        Serial.println(F("Detecting MH-Z19B sensor..."));
        delay(2000);
    }
    while (mhz19b.isWarmingUp()) {
        Serial.println(F("Warming up..."));
        delay(2000);
    }
}

int MHZ19B::readCO2(float &co2) {
    if (mhz19b.isReady()) {
        co2 = mhz19b.readCO2();
        return (co2 < 0) ? -1 : co2;
    }
    return -1;
}

bool MHZ19B::isReady() {
    return mhz19b.isReady();
}

void MHZ19B::calibrateZero() {
    mhz19b.startZeroCalibration();
    Serial.println("CO2 sensor zero calibration started.");
}
