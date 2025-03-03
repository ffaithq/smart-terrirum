#include "BME680.h"
#include "tools.h"
#include "Arduino.h"

#define GAS_LOWER_LIMIT 2000   // Bad air quality limit
#define GAS_UPPER_LIMIT 50000  // Good air quality limit 
#define HUMIDITY_REFERENCE 40
#define HUM_WEIGHT 0.25
#define GAS_WEIGHT 0.75
#define TEMPERATURE_OFFSET 1.7


BME680::BME680() : gas_reference(250000) {}

void BME680::begin() 
{
    if (!bme.begin()) {
        Serial.println(F("BME680 not found!"));
        while (1);
    }

    configureSensor();
}

void BME680::readData(float &tempAir, float &humAir, float &pressAir, float &gas) {
    if (!bme.performReading()) {
        Serial.println(F("Failed to complete reading :("));
        return;
    }
    tempAir = bme.temperature - TEMPERATURE_OFFSET;
    humAir = bme.humidity;
    pressAir = bme.pressure / 100.0; // Pa -> hPa
    gas = bme.readGas();
    calculateIAQ(humAir, gas);
    
}

void BME680::calculateIAQ(float &humAir, float &gas)
{
  ////https://github.com/G6EJD/BME680-Example GAS IAQ
  float hum_score, gas_score;

  hum_score = (humAir >= 38 && humAir <= 42) ? 25.0 : (humAir < 38 ? (0.25 / HUMIDITY_REFERENCE * humAir * 100) : (-0.25 / (100 - HUMIDITY_REFERENCE) * humAir + 0.416666) * 100);

  gas_score = (0.75 / (GAS_UPPER_LIMIT - GAS_LOWER_LIMIT) * gas_reference - (2000 * 0.75 / (GAS_UPPER_LIMIT - GAS_LOWER_LIMIT))) * 100;
  gas = (100 - (hum_score + gas_score)) * 5;
  if ((getgasreference_count++)%50==0) getGasReference(); 
}

void BME680::configureSensor()
{
    bme.setTemperatureOversampling(BME680_OS_2X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_1X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_0);
    bme.setGasHeater(320, 150);//default values 320 degrees for 150 ms
    getGasReference();

    

}

void BME680::getGasReference() {
    //Serial.println("Getting a new gas reference value");
    float totalGas = 0;
    for (int i = 0; i < 10; i++) 
        totalGas += bme.readGas();
        delay(100);
    gas_reference = SATURATE(totalGas / 10.0, GAS_LOWER_LIMIT, GAS_UPPER_LIMIT);
}