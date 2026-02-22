#ifndef TERMINAL_H
#define TERMINAL_H
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>
#include <functional>
#include <string>

#define DEBUG true

class Terminal {
 private:
  struct winsize dim_;
  struct termios original_termios_;
  int x_, y_;

  int minRows_ = -1;
  int minCols_ = -1;
  bool paused_ = false;

  Terminal();

  Terminal(const Terminal &) = delete;
  Terminal &operator=(const Terminal &) = delete;

  void drawResizeWarning();
  bool isKittyKeyboardSupported(double timeoutSec = 0.1);

 public:
  static Terminal &getInstance();
  ~Terminal();
  static void signalHandler(int);
  int getWidth();
  int getHeight();

  static std::function<void()> onSizeChange;

  bool kittyKeyboardSupported_ = false;

  void enableRawMode();
  void disableRawMode();
  void echo(bool enable);
  void canonical(bool enable);

  void mvHome();
  void mvRight(int);
  void mvLeft(int);
  void mvDown(int);
  void mvUp(int);
  void mvXY(int, int);
  void mvNextLine();
  void mvPreviousLine();
  void mvToColumn(int);
  std::string mvXYs(int, int);

  void showCursor();
  void hideCursor();

  void enableAltBuffer();
  void disableAltBuffer();

  void clear();

  void pauseIfTooSmall();
  bool isPaused() const;
  bool isBigEnough() const;
  void setMinSize(int minCols, int minRows);
};

/* Note: Top Left Conrer has position of (1,1) not (0,0) */
inline void Terminal::mvHome() { printf("\033[H"); }
inline void Terminal::mvXY(int x, int y) { printf("\033[%d;%dH", y, x); }

inline void Terminal::mvUp(int x) { printf("\033[%dA", x); }
inline void Terminal::mvDown(int x) { printf("\033[%dB", x); }
inline void Terminal::mvRight(int x) { printf("\033[%dC", x); }
inline void Terminal::mvLeft(int x) { printf("\033[%dD", x); }
inline void Terminal::mvNextLine() { printf("\033[E"); }
inline void Terminal::mvPreviousLine() { printf("\033[F"); }
inline void Terminal::mvToColumn(int x) { printf("\033[%dG", x); }
inline void Terminal::showCursor() { printf("\033[?25h"); }
inline void Terminal::hideCursor() { printf("\033[?25l"); }
inline void Terminal::enableAltBuffer() {
  printf("\033[?1049h");
  printf("\x1b[>2u");
};
inline void Terminal::disableAltBuffer() {
  printf("\x1b[<2u");
  printf("\033[?1049l");
};

#endif
