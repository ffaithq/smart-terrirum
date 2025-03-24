#include "BME680.h"
#include "Arduino.h"
#include "math.h"

#define GAS_LOWER_LIMIT 13200   // Bad air quality limit
#define GAS_UPPER_LIMIT 290000  // Good air quality limit
#define IAQ_LOWER_LIMIT 0   // Bad air quality limit
#define IAQ_UPPER_LIMIT 500  // Good air quality limit 
#define HUMIDITY_REFERENCE 40
#define HUM_WEIGHT 0.25
#define GAS_WEIGHT 0.75
#define GAS_SCALING_FACTOR 60
#define IAQ_SCALING_FACTOR 5 //increase or decrease based on sensitivity
#define TEMPERATURE_OFFSET 1.7
//#define GAS_RESISTANCE_BASE_LINE 294797 //low iaq
#define NOT_FOUND 6000


BME680::BME680() {}

int BME680::begin() 
{
    if (!bme.begin()) {
        Serial.println(F("BME680 not found!"));
        return NOT_FOUND;
    }

    configureSensor();
    return OK;
}

void BME680::readData(float *tempAir, float *humAir, float *pressAir, float *gas) {
    if (!bme.performReading()) {
        Serial.println(F("Failed to complete reading :("));
        return;
    }
    *tempAir = bme.temperature - TEMPERATURE_OFFSET;
    *humAir = bme.humidity;
    *pressAir = bme.pressure / 100.0; // Pa -> hPa
    *gas = bme.readGas();
    *gas = SATURATE(*gas, GAS_LOWER_LIMIT, GAS_UPPER_LIMIT);
    calculateIAQ(humAir, gas);
    
}

void BME680::calculateIAQ(float *humAir, float *gas)
{
  //https://github.com/G6EJD/BME680-Example/blob/master/ESP32_bme680_CC_demo_03.ino
    float hum_score, gas_score, IAQ_score;

    // Improved Humidity Score Calculation
    if (*humAir >= 38 && *humAir <= 42)
        hum_score = HUM_WEIGHT * 100;
    else if (*humAir < 38)
        hum_score = (HUM_WEIGHT / HUMIDITY_REFERENCE) * (*humAir) * 100;
    else
        hum_score = ((-HUM_WEIGHT / (100 - HUMIDITY_REFERENCE) * (*humAir)) + 0.416666) * 100;

    gas_score = (GAS_WEIGHT / (GAS_UPPER_LIMIT - GAS_LOWER_LIMIT)* (*gas) - (GAS_LOWER_LIMIT * (GAS_WEIGHT / (GAS_UPPER_LIMIT - GAS_LOWER_LIMIT)))) * 100;
    gas_score = SATURATE(gas_score, 0, 75); 

    IAQ_score = hum_score + gas_score;
    *gas = (100 - IAQ_score) * 5;
    // Final IAQ Score Scaling (Higher = Worse Air)
    *gas = SATURATE(*gas, IAQ_LOWER_LIMIT, IAQ_UPPER_LIMIT);

}

void BME680::configureSensor()
{
    bme.setTemperatureOversampling(BME680_OS_2X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_1X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_0);
    bme.setGasHeater(320, 150);//default values 320 degrees for 150 ms
 
}