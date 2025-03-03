#include "SSD1306.h"
#include "Arduino.h"

// Constructor: Initialize the display object
SSD1306::SSD1306() : ssd1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

// Method to initialize the display
void SSD1306::begin() {
  if (!ssd1306.begin(SSD1306_SWITCHCAPVCC,SSD1306_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Infinite loop if initialization fails
  }
}

// Method to clear the display
void SSD1306::clear() {
  ssd1306.clearDisplay();
}

// Method to display text on the screen
void SSD1306::printText(const char *text) {
  ssd1306.clearDisplay();
  ssd1306.setTextSize(1);      // Set text size to normal
  ssd1306.setTextColor(SSD1306_WHITE); // White text
  ssd1306.setCursor(0, 0);     // Set the cursor position
  ssd1306.print(text);
  ssd1306.display();
}

void SSD1306::printCountdown(const char *text, uint8_t count){
  ssd1306.clearDisplay();
  ssd1306.setTextSize(1);      // Set text size to normal
  ssd1306.setTextColor(SSD1306_WHITE); // White text
  ssd1306.setCursor(0, 0);     // Set the cursor position
  ssd1306.print(text); ssd1306.print("\n");
  ssd1306.print("Aprrox time remaining\n"); ssd1306.print(count); ssd1306.print("s\n");
  ssd1306.display();
}
void SSD1306::printMeasuredData(float &tempAir, float &humAir, float &tempSoil, float &humSoil, float &pressAir, float &UV, float &co2, float &gas){
  static uint8_t page = 0;
  ssd1306.clearDisplay();
  ssd1306.setTextSize(1);      // Set text size to normal
  ssd1306.setTextColor(SSD1306_WHITE); // White text
  ssd1306.setCursor(0, 0);     // Set the cursor position

  if(page == 0)
  {
    ssd1306.print("Temp air: ");  ssd1306.print(tempAir);  ssd1306.print("C\n");
    ssd1306.print("Hum air: ");  ssd1306.print(humAir);   ssd1306.print("%\n");
    ssd1306.print("CO2: ");   ssd1306.print(co2);      ssd1306.print("ppm\n");
    ssd1306.print("IAQ: ");   ssd1306.print(gas);      ssd1306.print("\n");
  }
  else if (page == 1)
  {
    ssd1306.print("Temp soil: "); ssd1306.print(tempSoil); ssd1306.print("C\n");
    ssd1306.print("Hum soil: "); ssd1306.print(humSoil);  ssd1306.print("%\n");
    ssd1306.print("Press: ");     ssd1306.print(pressAir); ssd1306.print("hPa\n");
    ssd1306.print("UV index: ");    ssd1306.print(UV);       ssd1306.print("\n");
  }
  ssd1306.display();
  page = (page + 1) % 2;
}
