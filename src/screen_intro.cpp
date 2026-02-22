#include <unistd.h>

#include <chrono>
#include <iostream>
#include <map>
#include <string>

#include "game.hpp"
#include "screen_intro.hpp"
#include "sprite_asset.hpp"
#include "trex.hpp"
#include "version.hpp"

ScreenIntro::ScreenIntro(Game* g) : game_(g) {
  oldTime_ = std::chrono::steady_clock::now();
  currentTime_ = oldTime_;
}

void ScreenIntro::drawBanner(const SpriteAsset& banner) const {
  for (const auto& line : banner.ANSI_ART) {
    printf("\033[1m%s\033[0m", line.c_str());
    printf("\033[%dD\033[1B", banner.COLS);
  }
}

void ScreenIntro::drawHelpMenu(bool blink) const {
  printf(" ___________________________________________   \033[47D\033[1B");
  printf(
      "/\\                 \033[1mHow to Play?\033[0m             \\  "
      "\033[47D\033[1B");
  printf("\\_|------------------------------------------| \033[47D\033[1B");
  printf("  |                                          | \033[47D\033[1B");
  printf("  | SPACE / UP  :     JUMP                   | \033[47D\033[1B");
  printf("  | DOWN        :     DUCK / FAST DROP       | \033[47D\033[1B");
  printf("  | Q / ESC     :     QUIT GAME              | \033[47D\033[1B");
  printf("  |                                          | \033[47D\033[1B");
  printf("  |   _______________________________________|_\033[47D\033[1B");
  printf("   \\_/________________________________________/\033[47D\033[2B");
  printf(blink ? "\033[1m" : "\033[0m");
  printf("               \033[33m[ PRESS SPACE TO START ]\033[0m        ");
};

void ScreenIntro::drawFooter() const {
  std::cout << "\033[2m" << "v" << TERMREX_VERSION_STRING << " - "
            << "By Satya Pr. Dahal\033[0m";
}

void ScreenIntro::run() {
  auto& tty = game_->tty_;
  auto& trex = game_->trex_;
  auto& banner = game_->INTROBANNER;
  int width = game_->WIDTH;
  int bannerX = game_->topLeft_.x_ + (width - banner.COLS) / 2;
  int bannerY = game_->topLeft_.y_ + 1;
  int menuX =
      game_->topLeft_.x_ + trex.getWidth() + (width - trex.getWidth() - 46) / 2;
  int menuY =
      game_->topLeft_.y_ + banner.ROWS + 3 + (game_->WIDTH - banner.ROWS) / 2;
  bool blink = true;
  ZAnimation zanim(game_->topLeft_.x_ + trex.getWidth() - 3,
                   game_->groundLevel_ - trex.getHeight() - 1, 0.5);
  while (true) {
    if (!game_->isTerminalBigEnough()) {
      continue;
    };
    zanim.updatePoint(game_->topLeft_.x_ + trex.getWidth() - 3,
                      game_->groundLevel_ - trex.getHeight() - 1);
    bannerX = game_->topLeft_.x_ + (width - banner.COLS) / 2;
    bannerY = game_->topLeft_.y_ + 1;
    menuX = game_->topLeft_.x_ + trex.getWidth() +
            (width - trex.getWidth() - 46) / 2;
    menuY = game_->topLeft_.y_ + banner.ROWS - 3 +
            (game_->HEIGHT - banner.ROWS) / 2;
    // Drawing
    game_->drawBoxNoFill();
    tty.mvXY(bannerX, bannerY);
    drawBanner(banner);
    drawIntroGround();
    zanim.drawAndAdvance();
    game_->status_ = GameStatus::INTRO_SCREEN;
    game_->updatePosition();
    trex.print();

    // controls menu
    tty.mvXY(menuX - 5, menuY - 3);
    currentTime_ = std::chrono::steady_clock::now();
    drawHelpMenu(blink);
    float elapsed =
        std::chrono::duration<float>(currentTime_ - oldTime_).count();
    if (elapsed > BLINK_TIME) {
      blink = !blink;
      oldTime_ = currentTime_;
    }
    // footer
    tty.mvXY(menuX + 7, game_->bottomRight_.y_ - 2);
    drawFooter();
    game_->takeInput();
    if (game_->input_.keypress_ == KeyPress::SPACE) {
      trex.changeAsset(TrexAssetName::JUMP);
      trex.print();
      fflush(stdout);
      usleep(200000);
      playIntroJump();
      game_->updatePosition();
      return;
    }
    if (game_->input_.keypress_ == KeyPress::ESC) {
      exit(0);
    }
    fflush(stdout);
    usleep(10000);
  }
}

void ScreenIntro::drawIntroGround() {
  auto& tty = game_->tty_;
  auto topLeft = game_->topLeft_;
  auto bottomRight = game_->bottomRight_;

  tty.mvXY(topLeft.x_ + 1, bottomRight.y_ - 3);
  printf("\033[2m_____,-.______");
  tty.mvXY(topLeft.x_ + 1, bottomRight.y_ - 2);
  printf("_   . -  -    .\033[0m");
}

void ScreenIntro::playIntroJump() {
  auto& trex = game_->trex_;
  auto& tty = game_->tty_;

  int trexH = trex.getHeight();
  int ground = game_->groundLevel_ - trexH;
  int peak = ground - (game_->HEIGHT - 2 - trexH);

  int y = ground;
  float vy = -3.8f;
  float gUp = 0.15f;
  float gDown = 0.45f;
  bool goingUp = true;

  while (true) {
    if (goingUp) {
      y += (int)vy;
      vy += gUp;

      if (y <= peak || vy >= 0.0f) {
        y = peak;
        goingUp = false;
        vy = 0.0f;
      }
    } else {
      vy += gDown;
      y += (int)vy;

      if (y >= ground) {
        y = ground;
        trex.setTop(y);
        trex.render(game_->topLeft_.x_, game_->bottomRight_.x_);
        break;
      }
    }
    //    tty.clear();
    game_->drawBox();
    drawIntroGround();
    trex.setTop(y);
    trex.render(game_->topLeft_.x_, game_->bottomRight_.x_);
    fflush(stdout);
    usleep(45000);
  }
  usleep(55000);
  bool assetChange = true;
  for (int i = 0; i < trex.paddingLeft_; ++i) {
    trex.moveBy(0, 1);
    if (assetChange) {
      trex.changeAsset(TrexAssetName::RUN1);
    } else {
      trex.changeAsset(TrexAssetName::RUN2);
    }
    assetChange = !assetChange;
    tty.clear();
    game_->drawBox();
    drawIntroGround();
    trex.render(game_->topLeft_.x_, game_->bottomRight_.x_);
    fflush(stdout);
    usleep(100000);
  }
}
ZAnimation::ZAnimation(int start_x, int start_y, double frame_delay)
    : start_x_(start_x),
      start_y_(start_y),
      FRAME_DELAY_MS(static_cast<long long>(frame_delay * 1000)) {
  last_update_ = std::chrono::steady_clock::now();

  // Initialize frames
  FRAMES_DATA_ = {
      {{0, 0, 'z', "BOLD"}},
      {{1, 0, 'z', "BOLD"}},
      {{2, -1, 'Z', "NORMAL"}},
      {{3, -1, 'Z', "NORMAL"}, {0, 0, 'z', "BOLD"}},
      {{4, -2, 'Z', "NORMAL"}, {1, 0, 'z', "BOLD"}},
      {{5, -2, 'Z', "DIM"}, {2, -1, 'Z', "NORMAL"}},
      {{6, -3, 'Z', "DIM"}, {3, -1, 'Z', "NORMAL"}, {0, 0, 'z', "BOLD"}},
      {{7, -3, 'Z', "DIM"}, {4, -2, 'Z', "NORMAL"}, {1, 0, 'z', "BOLD"}},
      {{8, -4, 'Z', "DIM"}, {5, -2, 'Z', "DIM"}, {2, -1, 'Z', "NORMAL"}},
      {{6, -3, 'Z', "DIM"}, {3, -1, 'Z', "NORMAL"}},
      {{7, -3, 'Z', "DIM"}, {4, -2, 'Z', "NORMAL"}},
      {{8, -4, 'Z', "DIM"}, {5, -2, 'Z', "DIM"}},
      {{6, -3, 'Z', "DIM"}},
      {{7, -3, 'Z', "DIM"}},
      {{8, -4, 'Z', "DIM"}},
      {}};

  // Initialize style codes
  STYLE_MAP_ = {{"BOLD", "\033[1m"}, {"NORMAL", "\033[0m"}, {"DIM", "\033[2m"}};
}

void ZAnimation::updatePoint(int x, int y) {
  start_x_ = x;
  start_y_ = y;
}

void ZAnimation::moveCursor(int row, int col) const {
  std::cout << "\033[" << row << ";" << col << "H";
}

const std::string& ZAnimation::getStyleCode(const std::string& style) const {
  static const std::string EMPTY_STRING = "";
  auto it = STYLE_MAP_.find(style);
  return (it != STYLE_MAP_.end()) ? it->second : EMPTY_STRING;
}

void ZAnimation::eraseFrame(const Frame& frame) {
  for (const auto& element : frame) {
    int row = start_y_ + element.y_;
    int col = start_x_ + element.x_;
    if (row > 0 && col > 0) {
      moveCursor(row, col);
      std::cout << ' ';
    }
  }
}

bool ZAnimation::drawAndAdvance() {
  auto now = std::chrono::steady_clock::now();
  if (now - last_update_ < FRAME_DELAY_MS) return false;

  last_update_ = now;
  eraseFrame(previous_elements_);
  const Frame& currentElements = FRAMES_DATA_[current_frame_];
  for (const auto& element : currentElements) {
    int row = start_y_ + element.y_;
    int col = start_x_ + element.x_;
    if (row > 0 && col > 0) {
      moveCursor(row, col);
      std::cout << getStyleCode(element.style_) << element.character_
                << "\033[0m";
    }
  }
  std::cout << std::flush;
  previous_elements_ = currentElements;
  current_frame_ = (current_frame_ + 1) % FRAMES_DATA_.size();
  return true;
}
