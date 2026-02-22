#include "game.hpp"

#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "game_over.hpp"
#include "input_handler.hpp"
#include "obstacles.hpp"
#include "score.hpp"
#include "screen_intro.hpp"
#include "sprite_asset.hpp"
#include "trex.hpp"

Game::Game(GameSettings& settings)
    : FPS(45.0f),
      settings_(settings),
      type_(settings.asciiOnly_ ? GameType::ASCII : GameType::UNICODE),
      theme_(type_ == GameType::ASCII ? ASCII_THEME : UNICODE_THEME),
      trex_(theme_.trexPack_, theme_.trexType_, -1, -1, this),
      terrain_(0, this),
      spawner_(this, 50.0f, 200.0f, 10),
      FRAME_TIME(1.0f / FPS),
      INTROBANNER(theme_.introBanner_),
      GAME_OVER_BANNER(theme_.gameOverBanner_),
      gameOver_(theme_.artStyle_, this),
      status_(GameStatus::INTRO_SCREEN),
      HEIGHT(trex_.getHeight() * 4),
      WIDTH(trex_.getHeight() * 20),
      score_(theme_.scoreStyle_),
      tty_(Terminal::getInstance()) {
  this->status_ = GameStatus::INITIALISED;
  initializeGameEnvironment();
  terrain_.setWidth(this->WIDTH - 2);
  Terminal::onSizeChange = [this]() { this->updatePosition(); };
}

void Game::initializeGameEnvironment() {
  now_ = last_ = std::chrono::steady_clock::now();
  tty_.hideCursor();
  tty_.setMinSize(WIDTH, HEIGHT);
  tty_.enableRawMode();
  input_.keyRepeatDelay_ = settings_.keyRepeat_;
  spawner_.registerFactory([this]() {
    std::uniform_int_distribution<> dist(0,
                                         (int)theme_.CACTUS_ASSETS.size() - 1);
    const SpriteAsset& chosen = *theme_.CACTUS_ASSETS[dist(spawner_.gen_)];
    return std::make_unique<Cactus>(chosen, this);
  });
  if (settings_.obstacleDino_) {
    spawner_.registerFactory([this]() {
      std::uniform_int_distribution<> dist(0,
                                           (int)theme_.PTERO_ASSETS.size() - 1);
      auto& pair = theme_.PTERO_ASSETS[dist(spawner_.gen_)];

      std::uniform_int_distribution<> heightDist(0, 1);
      PterodactylHeight mode = (heightDist(spawner_.gen_) == 0)
                                   ? PterodactylHeight::LOW
                                   : PterodactylHeight::MID;
      return std::make_unique<Pterodactyl>(*pair.first, *pair.second, this,
                                           mode);
    });
  }
  updatePosition();
}

void Game::showIntro() {
  ScreenIntro intro(this);
  intro.run();
}

void Game::takeInput() {
  input_.readInput();
  input_.updateHoldTimer();
}

void Game::updatePosition() {
  if (!tty_.isBigEnough()) return;
  int oldGround = this->groundLevel_;
  int newDx = (tty_.getWidth() - this->WIDTH + 1) / 2;
  int newDy = (tty_.getHeight() - this->HEIGHT + 1) / 2;
  int frameShiftX = newDx - this->topLeft_.x_;
  this->topLeft_ = {newDx, newDy};
  this->bottomRight_ = {this->topLeft_.x_ + this->WIDTH,
                        this->topLeft_.y_ + this->HEIGHT};
  this->groundLevel_ = this->bottomRight_.y_ - 1;
  int groundShift = this->groundLevel_ - oldGround;
  if (status_ != GameStatus::INTRO_SCREEN) {
    trex_.top_ += groundShift;
    trex_.left_ += frameShiftX;
  } else {
    trex_.setPos(this->HEIGHT - trex_.getHeight() + newDy - 1, newDx + 1);
  }
  for (auto& obs : spawner_.obstacles_) {
    obs->sprite_.top_ += groundShift;
    obs->sprite_.left_ += frameShiftX;
  }
}

bool Game::isRunning() { return status_ != GameStatus::GAME_OVER; }

void Game::draw() {
  tty_.mvXY(topLeft_.x_, topLeft_.y_);
  this->drawBox();
  tty_.mvXY(topLeft_.x_ + 1, groundLevel_ - 2);
  std::cout << "\033[2m";
  std::cout << terrain_.getVisible();
  tty_.mvXY(topLeft_.x_ + 1, groundLevel_ - 1);
  std::cout << terrain_.getSubterrain();
  printf("\033[22m");
  std::cout << std::flush;
  spawner_.render();
  std::cout << "\033[1m";
  trex_.print();
  std::cout << std::flush;
  score_.draw(topLeft_.y_, bottomRight_.x_);
  std::cout << std::flush;
}

void Game::update() {
  if (input_.keypress_ == KeyPress::SPACE) {
    trex_.pressJump();
  }
  if (input_.isDownActive()) {
    if (trex_.currentStatus_ == TrexStatus::JUMPING)
      trex_.fallFast();
    else if (trex_.currentStatus_ == TrexStatus::RUNNING)
      trex_.crouch();
  } else if (trex_.currentStatus_ == TrexStatus::DOWN) {
    trex_.dunktime_ += dt_;
    int trexDunktimems = (int)(trex_.dunktime_ * 1000);
    if (trexDunktimems > input_.keyRepeatDelay_ &&
        !tty_.kittyKeyboardSupported_) {
      trex_.standUP();
    }
    if (input_.keypress_ == KeyPress::DOWN_REL) {
      trex_.standUP();
    }
  }
  trex_.update(dt_);
  spawner_.update(dt_);
  score_.update(dt_, this->speed_.current_);
  terrain_.update(dt_);
  this->speed_.update(dt_);
}

bool Game::isTerminalBigEnough() {
  if (!tty_.isBigEnough()) {
    if (this->status_ == GameStatus::GAME_OVER) {
    } else {
      this->status_ = GameStatus::PAUSED;
    }
    usleep(4000 * 1000);
    tcflush(STDIN_FILENO, TCIFLUSH);
    this->last_ = std::chrono::steady_clock::now();
    return false;
  }
  this->status_ = GameStatus::RUNNING;
  return true;
}
void Game::reset() {
  input_.reset();
  trex_.reset();
  terrain_.reset();
  spawner_.reset();
  score_.reset();
  speed_.reset();
  gameOver_.reset();
}

void Game::loop() {
  bool playAgain = true;
  bool firstTime = true;

  while (playAgain) {
    if (!isTerminalBigEnough()) continue;

    input_.keypress_ = KeyPress::OTHER;

    if (firstTime && !settings_.skipIntro_) {
      trex_.changeAsset(TrexAssetName::IDLE);
      showIntro();
      tcflush(STDIN_FILENO, TCIFLUSH);
    }

    trex_.setPos(groundLevel_ - trex_.rows(),
                 topLeft_.x_ + trex_.paddingLeft_ + 1);
    status_ = GameStatus::RUNNING;

    now_ = last_ = std::chrono::steady_clock::now();

    while (status_ != GameStatus::GAME_OVER) {
      if (!isTerminalBigEnough()) continue;

      now_ = std::chrono::steady_clock::now();
      dt_ = std::chrono::duration<float>(now_ - last_).count();

      if (dt_ < FRAME_TIME) {
        std::this_thread::sleep_for(
            std::chrono::microseconds((long)((FRAME_TIME - dt_) * 1e6)));
        continue;
      }
      takeInput();
      update();
      if (spawner_.checkCollision(trex_)) {
        spawner_.freezePterodactyls();
        this->status_ = GameStatus::GAME_OVER;
        break;
      }
      draw();
      last_ = now_;
    }

    this->gameOver();
    firstTime = false;
    bool waiting = true;
    tcflush(STDIN_FILENO, TCIFLUSH);
    tty_.clear();
    while (waiting) {
      gameOverDraw();
      if (!isTerminalBigEnough()) continue;
      takeInput();
      if (input_.keypress_ == KeyPress::SPACE) {
        waiting = false;
        this->reset();
      } else if (input_.keypress_ == KeyPress::ESC) {
        waiting = false;
        playAgain = false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    input_.keypress_ = KeyPress::OTHER;
  }
}
void Game::drawBox() const {
  std::string top = "╭";
  std::string bottom = "╰";
  std::string horizontal = "─";
  int innerWidth = this->WIDTH - 2;
  for (int i = 0; i < innerWidth; ++i) {
    top += horizontal;
    bottom += horizontal;
  }
  top += "╮";
  bottom += "╯";

  std::string middle = "│" + std::string(innerWidth, ' ') + "│";
  tty_.mvXY(topLeft_.x_, topLeft_.y_);
  printf("%s", top.c_str());
  for (int j = 1; j < this->HEIGHT - 1; ++j) {
    tty_.mvXY(topLeft_.x_, topLeft_.y_ + j);
    printf("%s", middle.c_str());
  }
  tty_.mvXY(topLeft_.x_, topLeft_.y_ + this->HEIGHT - 1);
  printf("%s", bottom.c_str());
}

void Game::drawBoxNoFill() const {
  const int INNER_H = HEIGHT - 2;
  const int INNER_W = WIDTH - 2;
  tty_.mvXY(topLeft_.x_, topLeft_.y_);
  printf("╭");
  for (int i = 0; i < INNER_W; ++i) printf("─");
  printf("╮");

  for (int j = 0; j < INNER_H; ++j) {
    tty_.mvXY(topLeft_.x_, topLeft_.y_ + 1 + j);
    printf("│");

    tty_.mvXY(topLeft_.x_ + 1 + INNER_W, topLeft_.y_ + 1 + j);
    printf("│");
  }

  tty_.mvXY(topLeft_.x_, topLeft_.y_ + HEIGHT - 1);
  printf("╰");
  for (int i = 0; i < INNER_W; ++i) printf("─");
  printf("╯");
}

void Game::gameOver() {
  score_.saveScore(score_.current_);
  gameOverDraw();
}

void Game::gameOverDraw() {
  while (!tty_.isBigEnough()) {
  };
  int termWidth = this->WIDTH;
  int artWidth = this->GAME_OVER_BANNER.COLS;
  int startX = (termWidth - artWidth) / 2;
  int artHeight = 7;
  int startY = (this->HEIGHT - artHeight) / 2;

  if (gameOver_.animatedOnce_) {
    gameOver_.drawBanner(startX + this->topLeft_.x_, this->topLeft_.y_ + 5);

    startX = (termWidth - 17) / 2;
    gameOver_.animator_.runStatic(startX + topLeft_.x_,
                                  startY + this->topLeft_.y_ + 2);
    return;
  }
  draw();
  trex_.changeEye();
  trex_.changeMouth();
  gameOver_.drawBanner(startX + this->topLeft_.x_, this->topLeft_.y_ + 5);
  startX = (termWidth - 17) / 2;
  if (!gameOver_.animatedOnce_) {
    gameOver_.animator_.run(startX + topLeft_.x_,
                            startY + this->topLeft_.y_ + 2);
    gameOver_.animatedOnce_ = true;
  }
}
