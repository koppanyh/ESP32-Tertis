#ifndef ESP_TERTIS_DISPLAY_H_
#define ESP_TERTIS_DISPLAY_H_

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

#endif  // ESP_TERTIS_DISPLAY_H_
