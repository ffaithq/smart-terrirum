
#include "MHZ19B.h"



MHZ19B::MHZ19B(HardwareSerial &serial, int rxPin, int txPin)
    : mhzSerial(serial), rxPin(rxPin), txPin(txPin), mhz19b(&serial) {}

int MHZ19B::begin(bool firstCALL) {
    if (firstCALL){
      mhz19b.setRange5000ppm();
      Serial2.begin(9600, SERIAL_8N1, rxPin, txPin);

      mhz19b.setAutoCalibration(true);
    }

    if (!mhz19b.detect()){
        return MHZ19B_ISNOT_READY;
    }
    
    if (mhz19b.isWarmingUp()){
      return MHZ19B_WARMING_UP;
    }
    return OK;


    /*
    while (!mhz19b.detect()) {
        Serial.println(F("Detecting MH-Z19B sensor..."));
        delay(2000);
    }
        uint8_t count = WARMUP_TIME;

          while (mhz19b.isWarmingUp()) {

        Serial.println(F("Warming up..."));
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for 1000 ms
        count--;
    }

    */
}

int MHZ19B::readCO2(float *co2) {
    if (mhz19b.isReady()) {

        *co2 = mhz19b.readCO2();
        return (*co2 < 0) ? MHZ19B_CO2_LESS_ZERO : OK;
    }
    return MHZ19B_ISNOT_READY;
}

bool MHZ19B::isReady() {
    return mhz19b.isReady();
}

int MHZ19B::calibrateZero() {
    mhz19b.startZeroCalibration();
    Serial.println("CO2 sensor zero calibration started.");

    return OK;
}
