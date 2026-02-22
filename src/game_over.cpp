#include "game_over.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>

#include "game.hpp"

using namespace std::chrono_literals;

int GameOverAnimator::utf8Len(unsigned char c) {
  if ((c & 0x80) == 0) return 1;
  if ((c & 0xE0) == 0xC0) return 2;
  if ((c & 0xF0) == 0xE0) return 3;
  if ((c & 0xF8) == 0xF0) return 4;
  return 1;
}

std::vector<std::string> GameOverAnimator::splitUTF8(const std::string &s) {
  std::vector<std::string> out;
  out.reserve(s.size());  // preallocate to reduce reallocations
  for (size_t i = 0; i < s.size();) {
    int len = utf8Len((unsigned char)s[i]);
    out.push_back(s.substr(i, len));
    i += len;
  }
  return out;
}

void GameOverAnimator::gotoXY(int y, int x) {
  std::cout << "\x1b[" << y << ";" << x << "H";
}

GameOverAnimator::GameOverAnimator(ArtStyle style, Game *g)
    : style_(style), game_(g) {
  if (style_ == ArtStyle::UNICODE)
    loadUnicodeArt();
  else
    loadAsciiArt();

  rows_ = static_cast<int>(g1_.size());
  cols_ = 0;

  for (auto &r : g1_) cols_ = std::max(cols_, static_cast<int>(r.size()));
  for (auto &r : g2_) cols_ = std::max(cols_, static_cast<int>(r.size()));

  for (auto &r : g1_)
    while ((int)r.size() < cols_) r.push_back(" ");
  for (auto &r : g2_)
    while ((int)r.size() < cols_) r.push_back(" ");

  asciiBlinkPoints_ = {{4, 8},  {4, 9},  {4, 10}, {4, 11}, {4, 12}, {4, 13},
                       {4, 14}, {4, 15}, {5, 8},  {5, 9},  {5, 10}, {5, 11},
                       {5, 12}, {5, 13}, {5, 15}, {5, 15}};

  unicodeBlinkPoints_ = {{6, 9}, {6, 10}, {6, 11}, {6, 12}, {6, 13}, {6, 14}};
}

void GameOverAnimator::loadUnicodeArt() {
  std::vector<std::string> art = {" ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁ ", "▛  ▅▅▅          ▜",
                                  "▏    ███        ▕", "▏      ███▄     ▕",
                                  "▏      ███▀     ▕", "▏    ███        ▕",
                                  "▙  ▀▀▀   ▀▀▀▀▀▀ ▟"};

  std::vector<std::string> art2 = {" ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁ ", "▛    ▙▖         ▜",
                                   "▏ ▄▄▄██▙▁  ▄▄▄▄ ▕", "▏██▀▀██▛▔  ▀▀▀██▕",
                                   "▏██  ▛▘       ██▕", "▏██▄▄▄▄▄▄▄▄▄▄▄██▕",
                                   "▙ ▀▀▀▀▀▀▀▀▀▀▀▀▀ ▟"};

#ifdef __clang__
  for (auto &r : art) g1_.push_back(splitUTF8(r));
  for (auto &r : art2) g2_.push_back(splitUTF8(r));
#elif defined(__GNUC__)
  for (auto &r : art) g1_.push_back(std::move(splitUTF8(r)));
  for (auto &r : art2) g2_.push_back(std::move(splitUTF8(r)));
#else
  for (auto &r : art) g1_.push_back(splitUTF8(r));
  for (auto &r : art2) g2_.push_back(splitUTF8(r));
#endif
}

void GameOverAnimator::loadAsciiArt() {
  std::vector<std::string> art = {
      ",_________________,",   "|  __             |", "|  \\ \\            |",
      "|   \\ \\           |", "|   / /  ______   |", "|  /_/  |______|  |",
      "|                 |",   "'-----------------'"};

  std::vector<std::string> art2 = {
      ",_________________,",  "|     __          |", "|  ___\\ \\   ,___  |",
      "| / ,__  |  |_, \\ |", "| | | /_/     | | |", "| | |_________| | |",
      "| \\_____________/ |", "'-----------------'"};

#ifdef __clang__
  for (auto &r : art) g1_.push_back(splitUTF8(r));
  for (auto &r : art2) g2_.push_back(splitUTF8(r));
#elif defined(__GNUC__)
  for (auto &r : art) g1_.push_back(std::move(splitUTF8(r)));
  for (auto &r : art2) g2_.push_back(std::move(splitUTF8(r)));
#else
  for (auto &r : art) g1_.push_back(splitUTF8(r));
  for (auto &r : art2) g2_.push_back(splitUTF8(r));
#endif
}

void GameOverAnimator::drawFirstArt(int x, int y) {
  for (int r = 0; r < rows_; ++r) {
    gotoXY(y + r, x);
    if (style_ == ArtStyle::UNICODE) {
      std::cout << (r == 0 ? "\x1b[27m" : "\x1b[7m");
    } else {
      std::cout << "\x1b[1m";
    }
    for (auto &c : g1_[r]) std::cout << c;
  }
  std::cout << std::flush;
  std::this_thread::sleep_for(600ms);
}

void GameOverAnimator::drawSecondArt(int x, int y) {
  for (int r = 0; r < rows_; ++r) {
    gotoXY(y + r, x);
    if (style_ == ArtStyle::UNICODE) {
      std::cout << (r == 0 ? "\x1b[27m" : "\x1b[7m");
    } else {
      std::cout << "\x1b[1m";
    }
    for (auto &c : g2_[r]) std::cout << c;
  }
  std::cout << "\033[0m";
  std::cout << std::flush;
}

void GameOverAnimator::blinkPoints(
    int x, int y, const std::vector<std::pair<int, int> > &points) {
  for (int i = 0; i < 12; i++) {
    while (!game_->isTerminalBigEnough());
    bool on = (i % 2 == 0);

    for (auto &p : points) {
      int r = p.first;
      int c = p.second;
      if (r < 0 || r >= rows_ || c < 0 || c >= cols_) continue;

      gotoXY(y + r, x + c);
      std::cout << (on ? g1_[r][c] : " ");
    }
    std::cout << std::flush;
    std::this_thread::sleep_for(50ms);
  }
}

void GameOverAnimator::radialSwipe(int x, int y) {
  double cy = (rows_ - 1) / 2.0;
  double cx = (cols_ - 1) / 2.0;

  for (int f = 0; f <= 90; ++f) {
    while (!game_->isTerminalBigEnough());
    double t = (double)f / 90;
    double theta = t * 2 * M_PI;

    for (int r = 0; r < rows_; ++r) {
      gotoXY(y + r, x);
      if (style_ == ArtStyle::UNICODE) {
        std::cout << (r == 0 ? "\x1b[27m" : "\x1b[7m");
      } else {
        std::cout << "\x1b[1m";
      }

      for (int c = 0; c < cols_; ++c) {
        double dx = c - cx;
        double dy = r - cy;
        double ang = std::atan2(dx, -dy);
        if (ang < 0) ang += 2 * M_PI;

        std::cout << (ang <= theta ? g2_[r][c] : g1_[r][c]);
      }
    }
    std::cout << "\033[39m";
    std::cout << std::flush;
    std::this_thread::sleep_for(8ms);
  }

  std::cout << "\x1b[27m";
  std::this_thread::sleep_for(300ms);
}

void GameOverAnimator::run(int x, int y) {
  drawFirstArt(x, y);
  if (style_ == ArtStyle::UNICODE)
    blinkPoints(x, y, unicodeBlinkPoints_);
  else
    blinkPoints(x, y, asciiBlinkPoints_);
  radialSwipe(x, y);
}

void GameOverAnimator::runStatic(int x, int y) {
  game_->tty_.mvXY(game_->topLeft_.x_, game_->topLeft_.y_);
  game_->drawBoxNoFill();

  game_->tty_.mvXY(game_->topLeft_.x_ + 1, game_->groundLevel_ - 2);
  std::cout << "\033[2m" << game_->terrain_.getVisible();

  game_->tty_.mvXY(game_->topLeft_.x_ + 1, game_->groundLevel_ - 1);
  std::cout << game_->terrain_.getSubterrain() << "\033[22m";

  game_->spawner_.render();

  std::cout << "\033[1m";
  game_->trex_.print();
  game_->trex_.changeEye();
  game_->trex_.changeMouth();
  std::cout << "\033[0m";

  game_->score_.draw(game_->topLeft_.y_, game_->bottomRight_.x_);

  drawSecondArt(x, y);

  game_->tty_.mvXY(x - 10, y + 8);
  printf("\033[2mHit Space to try again — Q or Esc to exit\033[22m");
  fflush(stdout);
  std::cout << std::flush;
}

// GameOver =================================================================
GameOver::GameOver(ArtStyle style, Game *g) : game_(g), animator_(style, g) {}

void GameOver::animate(int x, int y) { animator_.run(x, y); }

void GameOver::reset() { animatedOnce_ = false; }

void GameOver::drawBanner(int x, int y) {
  game_->tty_.mvXY(x, y);
  for (auto &line : game_->GAME_OVER_BANNER.ANSI_ART) {
    std::cout << line;
    printf("\033[%dD\033[1B", game_->GAME_OVER_BANNER.COLS);
  }
  if (game_->type_ == GameType::ASCII) {
    printf("\033[0m");
  }
}

void GameOver::drawEnvironmentBeforeArt() {
  game_->tty_.mvXY(game_->topLeft_.x_, game_->topLeft_.y_);
  game_->drawBoxNoFill();

  game_->tty_.mvXY(game_->topLeft_.x_ + 1, game_->groundLevel_ - 2);
  std::cout << "\033[2m" << game_->terrain_.getVisible();

  game_->tty_.mvXY(game_->topLeft_.x_ + 1, game_->groundLevel_ - 1);
  std::cout << game_->terrain_.getSubterrain() << "\033[22m";

  game_->spawner_.render();

  std::cout << "\033[1m";
  game_->trex_.print();
  game_->trex_.changeEye();
  game_->trex_.changeMouth();
  std::cout << "\033[0m";

  game_->score_.draw(game_->topLeft_.y_, game_->bottomRight_.x_);
}

void GameOver::showStatic(int x, int y) {
  drawEnvironmentBeforeArt();
  drawBanner(x - 10, y + 8);
  animator_.runStatic(x, y);
  game_->tty_.mvXY(x - 10, y + 10);
  printf("\033[2mHit Space to try again — Q or Esc to exit\033[22m");
}
