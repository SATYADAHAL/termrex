#include "terminal.hpp"

#include <sys/select.h>

#include <chrono>
#include <csignal>
#include <cstdlib>

#include "draw_warning.hpp"

std::function<void()> Terminal::onSizeChange = nullptr;

Terminal::Terminal() {
  tcgetattr(STDIN_FILENO, &original_termios_);
  enableAltBuffer();
  mvXY(0, 0);
  fflush(stdout);

  if (isKittyKeyboardSupported()) {
    kittyKeyboardSupported_ = true;
  }

  if (signal(SIGWINCH, signalHandler) == SIG_ERR) {
    return;
  }
  if (ioctl(0, TIOCGWINSZ, &dim_) == -1) {
    dim_.ws_col = -1;
    dim_.ws_row = -1;
  }
  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    return;
  }
  return;
}

Terminal& Terminal::getInstance() {
  static Terminal tty;
  return tty;
}

int Terminal::getWidth() { return getInstance().dim_.ws_col; }
int Terminal::getHeight() { return getInstance().dim_.ws_row; }

void Terminal::clear() { printf("\033[2J\033[H"); }

void Terminal::signalHandler(int signal) {
  Terminal& term = Terminal::getInstance();

  if (signal == SIGWINCH) {
    ioctl(STDIN_FILENO, TIOCGWINSZ, &term.dim_);
    term.pauseIfTooSmall();
    onSizeChange();
    return;
  }

  if (signal == SIGINT) {
    term.disableAltBuffer();
    term.showCursor();
    term.disableRawMode();
    exit(1);
    return;
  }
}

void Terminal::enableRawMode() {
  struct termios raw = original_termios_;
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_iflag &= ~(ICRNL | INLCR | IGNCR | IXON | IXOFF);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void Terminal::disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios_);
}

void Terminal::echo(bool enable) {
  struct termios attr;
  tcgetattr(STDIN_FILENO, &attr);

  if (enable) {
    attr.c_lflag |= ECHO;
  } else {
    attr.c_lflag &= ~ECHO;
  }
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr);
}

void Terminal::canonical(bool enable) {
  struct termios attr;
  tcgetattr(STDIN_FILENO, &attr);

  if (enable) {
    attr.c_lflag |= ICANON;
  } else {
    attr.c_lflag &= ~ICANON;
  }

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr);
}

void Terminal::setMinSize(int minCols, int minRows) {
  this->minRows_ = minRows;
  this->minCols_ = minCols;
  this->pauseIfTooSmall();
  return;
}

void Terminal::pauseIfTooSmall() {
  if (minRows_ < 0 || minCols_ < 0) {
    paused_ = false;
    return;
  }
  if (dim_.ws_row < minRows_ || dim_.ws_col < minCols_) {
    paused_ = true;
    clear();
    drawResizeWarning();
  } else {
    paused_ = false;
    clear();
  }
  fflush(stdout);
}

bool Terminal::isPaused() const { return paused_; }
bool Terminal::isBigEnough() const { return !paused_; }

Terminal::~Terminal() {
  Terminal::getInstance().showCursor();
  showCursor();
  disableRawMode();
  echo(true);
  disableAltBuffer();
  disableRawMode();
}

bool Terminal::isKittyKeyboardSupported(double timeoutSec) {
  enableRawMode();
  write(STDOUT_FILENO, "\x1b[?u\x1b[c", 7);

  auto start = std::chrono::steady_clock::now();
  char buf[256];
  bool supported = false;
  while (true) {
    auto elapsed =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - start)
            .count();
    if (elapsed > timeoutSec) {
      supported = false;
      break;
    }
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = (long)50 * 1000;
    if (select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv) > 0) {
      ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
      if (n <= 0) continue;
      for (ssize_t i = 0; i + 2 < n; ++i) {
        if (buf[i] == '\x1b' && buf[i + 1] == '[' && buf[i + 2] == '?') {
          ssize_t p = i + 3;
          bool digit = false;
          while (p < n && std::isdigit(buf[p])) {
            digit = true;
            ++p;
          }
          if (digit && p < n) {
            if (buf[p] == 'u') {
              supported = true;
              goto done;
            } else if (buf[p] == 'c') {
              supported = false;
              goto done;
            }
          }
        }
      }
    }
  }
done:
  disableRawMode();
  return supported;
}
