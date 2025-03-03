#ifndef SSD1306_H
#define SSD1306_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Set the display size (128x64 for most SSD1306 displays)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SSD1306_I2C_ADDRESS 0X3C

class SSD1306{
  private:
    Adafruit_SSD1306 ssd1306;

  public:
    // Constructor to initialize the display
    SSD1306();

    // Method to initialize the display
    void begin();

    // Method to clear the display
    void clear();

    // Method to display text
    void printText(const char *text);
    void printCountdown(const char *text, uint8_t count);

    void printMeasuredData(float &tempAir, float &humAir, float &tempSoil, float &humSoil, float &pressAir, float &UV, float &co2, float &gas);
};

#endif //SSD1306_H
