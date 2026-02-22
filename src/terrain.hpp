#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <deque>
#include <random>
#include <string>
#include <vector>

#define MAX_WIDTH 138

class Game;

class Terrain {
 private:
  const std::vector<std::string> CHUNKS = {
      "__,-.",   "___.__",     "__.-.__", "______________",
      "__.,.__", "___,~.____", "_,---._", "____________",
      "__,-.__", "__.-._____", "______"};

  std::deque<char> buffer_;
  std::deque<char> subBuffer_;

  std::mt19937 rng_;
  std::uniform_int_distribution<int> dist_;
  std::uniform_int_distribution<int> subDist_;
  size_t maxWidth_;

  const char SUB_SYMBOLS[3] = {'-', '_', '.'};
  const int SUB_SYMBOL_COUNT = sizeof(SUB_SYMBOLS) / sizeof(SUB_SYMBOLS[0]);

  float scrollTimer_ = 0.0f;

  Game *game_ = nullptr;

 public:
  Terrain(int width, Game *g);

  void appendChunk();
  void appendSubterrain();

  void update(float deltaTime);

  void setWidth(unsigned int width);
  void reset();

  std::string getVisible() const;
  std::string getSubterrain() const;
};

#endif
