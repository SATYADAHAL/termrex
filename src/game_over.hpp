#ifndef GAMEOVER_HPP
#define GAMEOVER_HPP

#include <string>
#include <vector>

class Game;

enum class ArtStyle { ASCII, UNICODE };
class GameOverAnimator {
 public:
  GameOverAnimator(ArtStyle style, Game* g);

  void run(int x, int y);
  void runStatic(int x, int y);

 private:
  ArtStyle style_;
  Game* game_;

  std::vector<std::vector<std::string>> g1_;
  std::vector<std::vector<std::string>> g2_;

  int rows_;
  int cols_;

  std::vector<std::pair<int, int>> asciiBlinkPoints_;
  std::vector<std::pair<int, int>> unicodeBlinkPoints_;

  void loadAsciiArt();
  void loadUnicodeArt();
  void drawFirstArt(int x, int y);
  void drawSecondArt(int x, int y);
  void blinkPoints(int x, int y,
                   const std::vector<std::pair<int, int>>& points);
  void radialSwipe(int x, int y);

  int utf8Len(unsigned char c);
  std::vector<std::string> splitUTF8(const std::string& s);
  void gotoXY(int y, int x);
};

class GameOver {
 public:
  GameOver(ArtStyle style, Game* g);
  void animate(int x, int y);
  void showStatic(int x, int y);
  void reset();
  bool animatedOnce_ = false;
  Game* game_;
  GameOverAnimator animator_;
  void drawBanner(int x, int y);
  void drawEnvironmentBeforeArt();
};

#endif
