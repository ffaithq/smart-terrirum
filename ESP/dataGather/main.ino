#include <Wire.h>
#include <SPI.h>
#include "sensors.h"

//dara arrays definitions

void printValues();

const char* KEYS[] = {"tempAir", "humAir", "tempSoil", "humSoil", "UV", "pressAir", "co2", "IAQ"};
float tempAir, humAir, tempSoil, humSoil, UV, pressAir, co2, gas;
float* values[] = {&tempAir, &humAir, &tempSoil, &humSoil, &UV, &pressAir, &co2, &gas};

void setup() {
    Serial.begin(115200);
    
    bme680.begin();
    catnip_1.begin();
    catnip_2.begin();
    ltr390.begin();
    mhz19b.begin();
}

void loop() {
    bme680.readData(tempAir, humAir, pressAir, gas);
    
    float humSoil1, humSoil2, tempSoil1, tempSoil2;
    catnip_1.readData(humSoil1, tempSoil1);
    catnip_2.readData(humSoil2, tempSoil2);
    
    humSoil = (humSoil1 + humSoil2) / 2.0;
    tempSoil = (tempSoil1 + tempSoil2) / 2.0;
    
    ltr390.readUV(UV);
    
    mhz19b.readCO2(co2);

    printValues();

    delay(5000);
}

void printValues()
{
 for(int i = 0; i <8; i++)
  {
    Serial.print(KEYS[i]);
    Serial.print(": ");
    Serial.print(String(*values[i]));
    Serial.print("  \t");
  }

  Serial.println();
}
