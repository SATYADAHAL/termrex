#ifndef SCORE_HPP
#define SCORE_HPP

#include <sys/stat.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class HighScoreRW {
 private:
  std::string filePath_;
  bool fileExists() const {
    struct stat buffer;
    return (stat(filePath_.c_str(), &buffer) == 0);
  }

 public:
  HighScoreRW(const std::string& filename = ".termrex_highscore") {
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
      filePath_ = filename;
    } else {
      filePath_ = std::string(homeDir) + "/" + filename;
    }
  }

  int read() const {
    std::ifstream inFile(filePath_);
    int highScore = 0;
    if (inFile.is_open()) {
      inFile >> highScore;
      inFile.close();
    } else if (fileExists()) {
      std::cerr
          << "Warning: Cannot read high score file (permission issue?).\n";
    }
    return highScore;
  }

  bool store(int score) const {
    std::ofstream outFile(filePath_);
    if (!outFile.is_open()) {
      outFile.open("./.termrex_highscore");
      if (!outFile.is_open()) {
        std::cerr
            << "Error: Cannot write high score file. High score not saved.\n";
        return false;
      }
    }
    outFile << score;
    return true;
  }
};

struct DigitFont {
  std::vector<std::string> digits_;
  int digitWidth_;
  int digitHeight_;

  std::string hi_;
  int hiWidth_;
  int hiHeight_;
};

const DigitFont ASCII_DIGITS = {{{" ___  \033[6D\033[1B"
                                  "/   \\ \033[6D\033[1B"
                                  "| O | \033[6D\033[1B"
                                  "\\___/ \033[6D\033[1B"},
                                 {" __   \033[6D\033[1B"
                                  "/| |  \033[6D\033[1B"
                                  " | |  \033[6D\033[1B"
                                  "_|_|_ \033[6D\033[1B"},
                                 {" ___  \033[6D\033[1B"
                                  "|_  ) \033[6D\033[1B"
                                  " / /  \033[6D\033[1B"
                                  "/___| \033[6D\033[1B"},
                                 {" ____ \033[6D\033[1B"
                                  "|__ / \033[6D\033[1B"
                                  " |_ \\ \033[6D\033[1B"
                                  "|___/ \033[6D\033[1B"},
                                 {" _ _  \033[6D\033[1B"
                                  "| | | \033[6D\033[1B"
                                  "|_ _| \033[6D\033[1B"
                                  "  |_| \033[6D\033[1B"},
                                 {" ___  \033[6D\033[1B"
                                  "| __| \033[6D\033[1B"
                                  "|__ \\ \033[6D\033[1B"
                                  "|___/ \033[6D\033[1B"},
                                 {"  __  \033[6D\033[1B"
                                  " / /  \033[6D\033[1B"
                                  "/ _ \\ \033[6D\033[1B"
                                  "\\___/ \033[6D\033[1B"},
                                 {" ___  \033[6D\033[1B"
                                  "|__ \\ \033[6D\033[1B"
                                  "  / / \033[6D\033[1B"
                                  " /_/  \033[6D\033[1B"},
                                 {" ___  \033[6D\033[1B"
                                  "( _ ) \033[6D\033[1B"
                                  "/ _ \\ \033[6D\033[1B"
                                  "\\___/ \033[6D\033[1B"},
                                 {" ___  \033[6D\033[1B"
                                  "/ _ \\ \033[6D\033[1B"
                                  "\\_, / \033[6D\033[1B"
                                  " /_/  \033[6D\033[1B"}},
                                6,
                                4,
                                " _  _ ___ \033[10D\033[1B"
                                "| || |_ _|\033[10D\033[1B"
                                "| __ || | \033[10D\033[1B"
                                "|_||_|___|",
                                10,
                                4};

const DigitFont UNICODE_DIGIT = {{{"▄▖\033[2D\033[1B"
                                   "▌▌\033[2D\033[1B"
                                   "▙▌\033[2D\033[1B"},
                                  {"▗ \033[2D\033[1B"
                                   "▜ \033[2D\033[1B"
                                   "▟▖\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   "▄▌\033[2D\033[1B"
                                   "▙▖\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   "▄▌\033[2D\033[1B"
                                   "▄▌\033[2D\033[1B"},
                                  {"▖▖\033[2D\033[1B"
                                   "▙▌\033[2D\033[1B"
                                   " ▌\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   "▙▖\033[2D\033[1B"
                                   "▄▌\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   "▙▖\033[2D\033[1B"
                                   "▙▌\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   " ▌\033[2D\033[1B"
                                   " ▌\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   "▙▌\033[2D\033[1B"
                                   "▙▌\033[2D\033[1B"},
                                  {"▄▖\033[2D\033[1B"
                                   "▙▌\033[2D\033[1B"
                                   "▄▌\033[2D\033[1B"}},
                                 2,
                                 3,
                                 "▄ ▄▗▄▖\033[6D\033[1B"
                                 "█▄█ █ \033[6D\033[1B"
                                 "█ █▗█▖",
                                 6,
                                 3};
enum class ScoreStyle { UNICODE, ASCII };

class Score {
 public:
  Score(ScoreStyle style) {
    if (style == ScoreStyle::UNICODE)
      font_ = &UNICODE_DIGIT;
    else
      font_ = &ASCII_DIGITS;
    high_ = (float)hs_.read();
  }
  HighScoreRW hs_;
  float current_ = 0.0f;
  float high_ = 0.0f;
  const DigitFont* font_;

  int maxDigits_ = 5;
  int numberGap_ = 3;
  int msgNumGap_ = 3;
  int paddingEnd_ = 4;
  int totalGap_ = numberGap_ + msgNumGap_ + paddingEnd_;

  void update(float dt, float gameSpeed) {
    const float SCORE_MULTIPLIER = 0.08f;
    current_ += gameSpeed * dt * SCORE_MULTIPLIER;
    if (current_ > 999999.0) current_ = 999999.0;
  }

  void reset() {
    high_ = static_cast<float>(hs_.read());
    current_ = 0.0f;
  }

  void saveScore(float score) {
    if (score >= high_) {
      if (score >= 999999) {
        score = 999999;
      }
      hs_.store(int(score));
      high_ = score;
      return;
    }
  }

  void drawSimple(int gameTop, int gameRight) {
    printf("\x1b[%d;%dH", gameTop + 1,
           gameRight - 1 - maxDigits_ * 2 - 2 - totalGap_);
    printf("\033[2mHI");
    printf("%*c", msgNumGap_, ' ');
    printf("%0*d\033[22m", maxDigits_, (int)high_);
    printf("\033[1m%*c", numberGap_, ' ');
    printf("%0*d\033[0m", maxDigits_, (int)current_);
  }

  void draw(int gameTop, int gameRight) {
    printf("\x1b[%d;%dH", gameTop + 1,
           gameRight - 1 - maxDigits_ * font_->digitWidth_ * 2 -
               font_->hiWidth_ - totalGap_);

    printf("\033[2m%s", font_->hi_.c_str());
    printf("\033[%dA\033[%dC", font_->hiHeight_ - 1,
           msgNumGap_ + maxDigits_ * font_->digitWidth_ - font_->digitWidth_);

    {
      int tmp = static_cast<int>(high_ < 999999.0 ? high_ : 999999.0);
      int lastDigit = 0;
      int digitCount = 0;
      while (tmp > 0) {
        lastDigit = tmp % 10;
        tmp = tmp / 10;
        printf("%s", font_->digits_[lastDigit].c_str());
        printf("\033[%dD\033[%dA", font_->digitWidth_, font_->digitHeight_);
        digitCount++;
      }
      for (int i = 0; i < maxDigits_ - digitCount; ++i) {
        printf("%s", font_->digits_[0].c_str());
        printf("\033[%dD\033[%dA", font_->digitWidth_, font_->digitHeight_);
      }
    }
    printf("\033[0m\033[1m\033[%dC",
           maxDigits_ * font_->digitWidth_ * 2 + numberGap_);

    // Display current score
    int tmp = static_cast<int>(current_);

    int lastDigit = 0;
    int digitCount = 0;
    while (tmp > 0) {
      lastDigit = tmp % 10;
      tmp = tmp / 10;
      printf("%s", font_->digits_[lastDigit].c_str());

      printf("\033[%dD\033[%dA", font_->digitWidth_, font_->digitHeight_);
      digitCount++;
    }
    for (int i = 0; i < maxDigits_ - digitCount; ++i) {
      printf("%s", font_->digits_[0].c_str());
      printf("\033[%dD\033[%dA", font_->digitWidth_, font_->digitHeight_);
    }
    printf("\033[0m");
  }
};

#endif
