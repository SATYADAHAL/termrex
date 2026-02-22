#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <unistd.h>

#include <chrono>

enum class KeyPress { SPACE, DOWN, ESC, OTHER, DOWN_REL };

class InputHandler {
 private:
  static constexpr int BUFSIZE = 64;
  char inp_[BUFSIZE] = {0};

 public:
  int keyRepeatDelay_ = 200;
  std::chrono::steady_clock::time_point downPressTime_;
  bool downHeld_ = false;
  KeyPress keypress_ = KeyPress::OTHER;
  KeyPress lastpress_ = KeyPress::OTHER;

  InputHandler() = default;

  void readInput();

  void updateHoldTimer();

  void holdDownKey();
  void releaseDownKey();
  bool hasEnoughTimepassed();
  bool isDownActive() const;
  int parseKeyCode(const char*);

  void reset();
};

#endif
