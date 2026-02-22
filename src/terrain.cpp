#include "terrain.hpp"

#include "game.hpp"

// Constructor
Terrain::Terrain(int width, Game* g) : rng_(std::random_device{}()) {
  this->game_ = g;
  dist_ = std::uniform_int_distribution<int>(0, (int)CHUNKS.size() - 1);
  subDist_ = std::uniform_int_distribution<int>(0, 8);
  this->maxWidth_ = width;

  while (buffer_.size() < this->maxWidth_) appendChunk();
  while (subBuffer_.size() < this->maxWidth_) appendSubterrain();
}

void Terrain::update(float deltaTime) {
  if (!game_) return;

  scrollTimer_ += deltaTime;

  float currentSpeed = game_->speed_.current_;

  if (currentSpeed <= 0.0f) return;

  float stepInterval = 1.0f / currentSpeed;

  while (scrollTimer_ >= stepInterval) {
    if (!buffer_.empty()) buffer_.pop_front();
    if (!subBuffer_.empty()) subBuffer_.pop_front();

    while (buffer_.size() < this->maxWidth_) appendChunk();
    while (subBuffer_.size() < this->maxWidth_) appendSubterrain();

    scrollTimer_ -= stepInterval;
  }
}

void Terrain::appendChunk() {
  const std::string& chunk = CHUNKS[dist_(rng_)];
  for (char c : chunk) buffer_.push_back(c);
}

void Terrain::appendSubterrain() {
  std::uniform_real_distribution<double> prob(0.0, 1.0);
  double p = prob(rng_);

  if (p < 0.1)
    subBuffer_.push_back(SUB_SYMBOLS[subDist_(rng_) % SUB_SYMBOL_COUNT]);
  else
    subBuffer_.push_back(' ');
}

void Terrain::setWidth(unsigned int width) { this->maxWidth_ = width; }

std::string Terrain::getVisible() const {
  return std::string(buffer_.begin(), buffer_.begin() + (int)this->maxWidth_);
}

std::string Terrain::getSubterrain() const {
  return std::string(subBuffer_.begin(),
                     subBuffer_.begin() + (int)this->maxWidth_);
}

void Terrain::reset() {
  buffer_.clear();
  subBuffer_.clear();
  scrollTimer_ = 0.0f;
  appendChunk();
  appendSubterrain();
}
