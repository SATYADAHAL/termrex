#ifndef SCREEN_INTRO_HPP
#define SCREEN_INTRO_HPP
#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "sprite_asset.hpp"
#include "trex.hpp"

class Game;

class ScreenIntro {
 public:
  explicit ScreenIntro(Game* game);

  void drawBanner(const SpriteAsset& banner) const;
  void drawHelpMenu(bool blink) const;
  bool wantsToQuit() const;
  void drawFooter() const;
  void run();
  const float BLINK_TIME = 0.5f;
  std::chrono::steady_clock::time_point currentTime_;
  std::chrono::steady_clock::time_point oldTime_;

 private:
  Game* game_;
  void playIntroJump();
  void drawIntroGround();
};

struct AnimationElement {
  int x_;
  int y_;
  char character_;
  std::string style_;
};

using Frame = std::vector<AnimationElement>;
using Frames = std::vector<Frame>;

class ZAnimation {
 private:
  int start_x_;
  int start_y_;
  const std::chrono::milliseconds FRAME_DELAY_MS;
  unsigned int current_frame_ = 0;

  Frames FRAMES_DATA_;
  std::map<std::string, std::string> STYLE_MAP_;

  std::chrono::steady_clock::time_point last_update_;
  Frame previous_elements_;

  void moveCursor(int row, int col) const;
  const std::string& getStyleCode(const std::string& style) const;
  void eraseFrame(const Frame& frame);

 public:
  ZAnimation(int start_x, int start_y, double frame_delay);
  void updatePoint(int x, int y);
  bool drawAndAdvance();
};

#endif
