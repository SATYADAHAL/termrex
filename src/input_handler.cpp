#include "input_handler.hpp"

#include <stdlib.h>

int InputHandler::parseKeyCode(const char* seq) {
  if (!seq) return -1;
  if (seq[0] != 27 || seq[1] != '[') return -1;
  const char* p = seq + 2;
  char* end;
  long code = strtol(p, &end, 10);
  return static_cast<int>(code);
}

void InputHandler::readInput() {
  keypress_ = KeyPress::OTHER;
  ssize_t bytesRead = read(STDIN_FILENO, inp_, BUFSIZE);
  if (bytesRead <= 0) {
    return;
  }

  if (bytesRead == 1) {
    if (inp_[0] == ' ')
      keypress_ = KeyPress::SPACE;
    else if (inp_[0] == 'q' || inp_[0] == '\x1b')
      keypress_ = KeyPress::ESC;
  } else if (bytesRead == 3) {
    if (inp_[2] == 'B') {
      keypress_ = KeyPress::DOWN;
    }
  } else if (bytesRead >= 6) {
    /*Kitty Keyboard Protoocol
      https://sw.kovidgoyal.net/kitty/keyboard-protocol/#event-types
    */
    char lastkey = inp_[bytesRead - 1];
    char hasEvent = (inp_[bytesRead - 3] == ':') ? true : false;
    char eventType = (hasEvent) ? inp_[bytesRead - 2] : -1;
    switch (lastkey) {
      case 'B':
        if (hasEvent) {
          if (eventType == '1') {
            keypress_ = KeyPress::DOWN;
            break;
          }
          if (eventType == '2') {
            keypress_ = KeyPress::DOWN;
            break;
          }
          if (eventType == '3') {
            keypress_ = KeyPress::DOWN_REL;
            break;
          }
          break;
        } else {
          keypress_ = KeyPress::DOWN;
          break;
        }
      case 'u': {
        int keycode = parseKeyCode(inp_);
        if (keycode == 32) {
          keypress_ = KeyPress::SPACE;
          break;
        } else if (keycode == 113) {
          keypress_ = KeyPress::ESC;
          break;
        }
        break;
      }
      default:
        break;
    }
  }

  if (keypress_ == KeyPress::DOWN) {
    holdDownKey();
  } else if (keypress_ == KeyPress::DOWN_REL || keypress_ == KeyPress::SPACE ||
             keypress_ == KeyPress::OTHER) {
    releaseDownKey();
  }
}

bool InputHandler::hasEnoughTimepassed() {
  auto now = std::chrono::steady_clock::now();
  auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - downPressTime_)
                       .count();
  return elapsedMs >= keyRepeatDelay_;
}

void InputHandler::updateHoldTimer() {
  if (downHeld_ && hasEnoughTimepassed()) {
    releaseDownKey();
  }
}

void InputHandler::holdDownKey() {
  downHeld_ = true;
  downPressTime_ = std::chrono::steady_clock::now();
}

void InputHandler::releaseDownKey() { downHeld_ = false; }

bool InputHandler::isDownActive() const { return downHeld_; }

void InputHandler::reset() {
  keypress_ = KeyPress::OTHER;
  downHeld_ = false;
}
