#ifndef ESP_TERTIS_DISPLAY_H_
#define ESP_TERTIS_DISPLAY_H_

#include <Adafruit_SSD1306.h>
#include <Wire.h>

class Display {
  // Define a basic display interface that works for anything
 public:
  virtual void Init() {
    // Setup display hardware here
    // TODO Write this for your display
  }
  virtual void Update() {
    // Push the new pixel values to the display
    // (0, 0) is defined as the top left corner
    // TODO Write this for your display
  }
  void Clear() {
    for (int x = 0; x < width_; x++) {
      for (int y = 0; y < height_; y++) {
        pixels_[x][y] = false;
      }
    }
  }
  void SetAt(int x, int y, bool color) {
    // Set pixel at specified coordinates
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
      return;
    pixels_[x][y] = color;
  }
  bool GetAt(int x, int y) {
    // Get pixel at specified coordinates
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
      return false;
    return pixels_[x][y];
  }
  const int GetWidth() {
    // Get the width in a pretty way
    return width_;
  }
  const int GetHeight() {
    // Get the height in a pretty way
    return height_;
  }

 protected:
  static const int width_ = 10;
  static const int height_ = 20;
  bool pixels_[width_][height_] = {0};
};

class SSD1306Display : public Display {
  // This is designed to work with my test setup
  // TODO Just delete this class and update Display to work on your display
 public:
  void Init() override {
    const int screen_width = 128;  // OLED display width, in pixels
    const int screen_height = 64;  // OLED display height, in pixels
    const int oled_reset = 4;  // Reset pin # (or -1 if sharing Arduino reset pin)
    const int screen_address = 0x3C;  // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
    display_ = Adafruit_SSD1306(screen_width, screen_height, &Wire, oled_reset);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display_.begin(SSD1306_SWITCHCAPVCC, screen_address)) {
      Serial.println(F("SSD1306 allocation failed!"));
      while (true); // Don't proceed, loop forever
    }
    display_.clearDisplay();
    display_.display();
  }
  void Update() override {
    display_.clearDisplay();
    for (int x = 0; x < width_; x++) {
      for (int y = 0; y < height_; y++) {
        if (pixels_[x][y])
          display_.fillRect(y * 6 + 2, 56 - x * 6, 5, 5, SSD1306_WHITE);
      }
    }
    display_.drawRect(0, 0, 123, 63, SSD1306_WHITE);
    display_.display();
  }

 private:
  Adafruit_SSD1306 display_;
};

#endif  // ESP_TERTIS_DISPLAY_H_
