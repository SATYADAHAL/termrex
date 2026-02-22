#ifndef GAME_H
#define GAME_H
#include <unistd.h>

#include <chrono>

#include "cli_parser.hpp"
#include "game_over.hpp"
#include "input_handler.hpp"
#include "obstacles.hpp"
#include "score.hpp"
#include "sprite_asset.hpp"
#include "style_pack.hpp"
#include "terminal.hpp"
#include "terrain.hpp"
#include "trex.hpp"

using GameSettings = CLI_Parser::GameSettings;

enum class GameStatus { INITIALISED, RUNNING, GAME_OVER, INTRO_SCREEN, PAUSED };
enum class GameType { ASCII, UNICODE };

struct Coord {
  int x_;
  int y_;
};

struct ScrollSpeed {
  float start_;
  float current_;
  float max_;
  float accel_;
  std::string prevBuffer_;
  ScrollSpeed(float start = 90.0f, float max_ = 220.0f, float accel_ = 0.4f)
      : start_(start), current_(start), max_(max_), accel_(accel_) {}

  void reset() { current_ = start_; }

  void update(float dt) {
    current_ += accel_ * dt;
    if (current_ > max_) current_ = max_;
  }
};

class Game {
 private:
  static constexpr int BUFSIZE = 64;

 public:
  const float FPS;

  GameSettings settings_;
  GameType type_;
  ThemePack theme_;
  ScrollSpeed speed_;
  std::string prevBuffer_;
  InputHandler input_;

  Trex trex_;
  Terrain terrain_;
  ObstacleSpawner spawner_;

  using FrameBuffer = std::vector<std::vector<Cell>>;

  std::chrono::time_point<std::chrono::steady_clock> last_;
  std::chrono::time_point<std::chrono::steady_clock> now_;
  float dt_;

  const float FRAME_TIME;

  const SpriteAsset INTROBANNER;
  const SpriteAsset GAME_OVER_BANNER;
  GameOver gameOver_;

  GameStatus status_;
  Coord topLeft_;
  Coord bottomRight_;
  const int HEIGHT;
  const int WIDTH;
  int groundLevel_;
  Score score_;
  Game(GameSettings& settings);

  Terminal& tty_;
  void initializeGameEnvironment();

  void draw();
  void update();
  bool isTerminalBigEnough();

  void showIntro();

  bool isRunning();

  void takeInput();

  void gameOver();
  void gameOverDraw();

  void loop();

  void drawBox() const;
  void drawBoxNoFill() const;
  void updatePosition();

  void reset();
};

#endif
