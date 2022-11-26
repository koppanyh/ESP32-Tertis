#ifndef ESP_TERTIS_TERTIS_H_
#define ESP_TERTIS_TERTIS_H_

#include "display.h"

class Tertis {
 public:
  Tertis(Display* display) {
    display_ = display;
    Spawn();
  }
  bool Translate(int x, int y) {
    // Move the tetromino
    Draw(false);
    RawTranslate(x, y);
    bool collided = Collision();
    if (collided)  // Undo the move if we collide
      RawTranslate(-x, -y);
    Draw(true);
    return collided;
  }
  bool Rotate() {
    // Shortcut for rotation
    return Rotate(false);
  }
  bool Rotate(bool direction) {
    // Rotate the tetromino
    Draw(false);
    RawRotate(direction);
    bool collided = Collision();
    if (collided)  // Undo the rotate if we collide
      RawRotate(!direction);
    Draw(true);
    return collided;
  }
  void Step() {
    // The main component of the Tertis game loop
    if (!running_)
      return;
    bool collided = Translate(0, 1);
    if (collided) {  // Can't move down more, spawn a new tetromino
      Cleanup();
      collided = Spawn();
      if (collided)  // Pile reached the top, can't go forward. Game Over.
        Reset();
    }
    display_->Update();
  }
  void Reset() {
    // Restart the game
    display_->Clear();
    score_ = 0;
    Spawn();
  }
  void Start() {
    // Let the game progress
    running_ = true;
  }
  void Stop() {
    // Prevent the game from running
    running_ = false;
  }
  int GetScore() {
    // Get the score to display it
    return score_;
  }
  void Demo() {
    // Randomly play the game (e.g. crappy demo mode)
    Start();
    Translate(random(5) - 2, 0);
    if (!random(3))
      Rotate();
  }

 private:
  bool Collision() {
    // Check for wall and tetromino collisions
    for (int i = 0; i < 4; i++) {
      int x = coords_[i][0];
      int y = coords_[i][1];
      int w = display_->GetWidth();
      int h = display_->GetHeight();
      if (x < 0 || x >= w || y < 0 || y >= h)  // Walls collision
        return true;
      if (display_->GetAt(x, y))  // Tetromino collision
        return true;
    }
    return false;
  }
  void Draw(bool color) {
    // Update the display
    for (int i = 0; i < 4; i++)
      display_->SetAt(coords_[i][0], coords_[i][1], color);
  }
  void RawTranslate(int x, int y) {
    // Actually moves the tetromino
    for (int i = 0; i < 4; i++) {
      coords_[i][0] += x;
      coords_[i][1] += y;
    }
  }
  void RawRotate(bool direction) {
    // Actually rotates the tetromino
    int offset_x = coords_[0][0];
    int offset_y = coords_[0][1];
    for (int i = 1; i < 4; i++) {
      int x = coords_[i][0] - offset_x;
      int y = coords_[i][1] - offset_y;
      if (direction) {  // CCW
        x = -x;
      } else {  // CW
        y = -y;
      }
      coords_[i][0] = y + offset_x;
      coords_[i][1] = x + offset_y;
    }
  }
  bool Spawn() {
    // Spawn a new random tetromino
    int tetromino = random(7);
    int offset_x = display_->GetWidth() / 2;
    for (int i = 0; i < 4; i++) {
      coords_[i][0] = tetrominoes_[tetromino][i][0] + offset_x;
      coords_[i][1] = tetrominoes_[tetromino][i][1];
    }
    bool collided = Collision();
    Draw(true);
    return collided;
  }
  void Cleanup() {
    // Clear full rows and shift the remaining rows down
    int width = display_->GetWidth();
    int height = display_->GetHeight();
    for (int row = height - 1; row >= 0; row--) {
      while (true) {
        bool is_full = true;
        for (int x = 0; x < width; x++) {
          is_full = is_full && display_->GetAt(x, row);
        }
        if (!is_full)
          break;
        score_++;
        for (int row2 = row; row2 >= 1; row2--) {
          for (int x = 0; x < width; x++) {
            display_->SetAt(x, row2, display_->GetAt(x, row2 - 1));
          }
        }
        for (int x = 0; x < width; x++) {
          display_->SetAt(x, 0, false);
        }
      }
    }
  }
  
  Display* display_;
  int coords_[4][2];
  bool running_ = true;
  int score_ = 0;
  const int tetrominoes_[7][4][2] = {
    {{0, 0}, {-1, 0}, {-1, 1}, {1, 0}},  // L
    {{0, 0}, {-1, 0}, { 1, 0}, {1, 1}},  // J
    {{0, 0}, {-1, 0}, { 0, 1}, {1, 1}},  // Z
    {{0, 0}, {-1, 1}, { 0, 1}, {1, 0}},  // S
    {{0, 0}, {-1, 0}, { 0, 1}, {1, 0}},  // T
    {{0, 0}, { 0, 1}, { 1, 0}, {1, 1}},  // O
    {{0, 0}, {-1, 0}, { 1, 0}, {2, 0}}   // I
  };
};

#endif  // ESP_TERTIS_TERTIS_H_
