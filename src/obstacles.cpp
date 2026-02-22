#include "obstacles.hpp"

#include "game.hpp"
#include "sprite.hpp"
#include "sprite_asset.hpp"

ObstacleSpawner::ObstacleSpawner(Game* g, float minD, float maxD, int maxObs)
    : game_(g),
      maxObstacles_(maxObs),
      minDistance_(minD),
      maxDistance_(maxD),
      nextSpawnDistance_(0.0f),
      gen_(std::random_device{}()) {}

void ObstacleSpawner::registerFactory(
    std::function<std::unique_ptr<Obstacle>()> factory) {
  factories_.push_back(factory);
}
void ObstacleSpawner::reset() {
  obstacles_.clear();
  nextSpawnDistance_ = 0;
}

void ObstacleSpawner::update(float deltaTime) {
  bool canSpawn = true;

  if (obstacles_.empty()) {
    canSpawn = true;
  } else {
    if (obstacles_.back()->distanceMoved_ < nextSpawnDistance_) {
      canSpawn = false;
    }
  }

  if (canSpawn && obstacles_.size() < maxObstacles_) {
    spawnRandomObstacle();
    std::uniform_real_distribution<float> gapDist(minDistance_, maxDistance_);
    nextSpawnDistance_ = gapDist(gen_);
  }
  for (auto& obs : obstacles_) obs->update(deltaTime);

  obstacles_.erase(std::remove_if(obstacles_.begin(), obstacles_.end(),
                                  [](const auto& o) { return !o->active_; }),
                   obstacles_.end());
}

void ObstacleSpawner::render() const {
  for (auto& obs : obstacles_) obs->render();
}

bool ObstacleSpawner::checkCollision(const SpriteInstance& trex) const {
  for (const auto& obs : obstacles_) {
    if (SpriteInstance::collide(obs->sprite_, trex)) {
      return true;
    }
  }
  return false;
}

void ObstacleSpawner::spawnRandomObstacle() {
  if (factories_.empty() || !game_) return;

  size_t selectedIndex = 0;
  size_t count = factories_.size();
  if (count == 1) {
    selectedIndex = 0;
  } else {
    std::uniform_int_distribution<> chance(0, 100);
    int roll = chance(gen_);
    int birdChance = 10.0;
    if (game_->speed_.current_ < game_->speed_.start_ + 10.0) {
      birdChance = 0;
    }
    if (roll < birdChance) {
      std::uniform_int_distribution<size_t> rareDist(1, count - 1);
      selectedIndex = rareDist(gen_);
    } else {
      selectedIndex = 0;
    }
  }

  auto newObs = factories_[selectedIndex]();
  obstacles_.push_back(std::move(newObs));
}

// Obstacles================================================
Obstacle::Obstacle(const SpriteAsset& asset_, Game* g, int top_, int left_)
    : active_(true),
      distanceMoved_(0.0f),
      game_(g),
      sprite_(asset_, top_, left_) {}

void Obstacle::update(float deltaTime) {
  if (!active_ || !game_) return;
  float currentSpeed = game_->speed_.current_;
  distanceMoved_ += currentSpeed * deltaTime;
  int newLeft = game_->bottomRight_.x_ - 1 - static_cast<int>(distanceMoved_);
  sprite_.left_ = newLeft;
  time_ += deltaTime;
  if (sprite_.right() < 0) active_ = false;
}

void Obstacle::render() {
  sprite_.render(game_->topLeft_.x_, game_->bottomRight_.x_);
}

// Cactus===================================================
Cactus::Cactus(const SpriteAsset& asset_, Game* g) : Obstacle(asset_, g) {}

void Cactus::update(float deltaTime) {
  sprite_.setPos(game_->groundLevel_ - sprite_.rows(), sprite_.getLeft());
  Obstacle::update(deltaTime);
}

Pterodactyl::Pterodactyl(const SpriteAsset& assetUp,
                         const SpriteAsset& assetDown, Game* g,
                         PterodactylHeight mode)
    : Obstacle(assetUp, g),
      mode_(mode),
      name_(PterodactylAsset::UP),
      spriteUp_(assetUp, 0, 0),
      spriteDown_(assetDown, 0, 0) {
  sprite_ = spriteUp_;
}

void Pterodactyl::setAsset(const SpriteInstance& newSprite) {
  int curTop = sprite_.getTop();
  int curLeft = sprite_.getLeft();
  sprite_ = newSprite;
  sprite_.setPos(curTop, curLeft);
}

void Pterodactyl::changeAsset(PterodactylAsset newName) {
  switch (newName) {
    case PterodactylAsset::UP:
      setAsset(spriteUp_);
      break;
    case PterodactylAsset::DOWN:
      setAsset(spriteDown_);
      break;
  }
  name_ = newName;
}

void Pterodactyl::update(float deltaTime) {
  if (frozen_ || !game_->isRunning()) return;

  int val = (game_->type_ == GameType::UNICODE) ? 2 : 3;

  switch (mode_) {
    case PterodactylHeight::LOW:
      sprite_.top_ =
          game_->groundLevel_ - game_->trex_.getHeight() - sprite_.rows() + val;
      break;
    case PterodactylHeight::MID:
      sprite_.top_ = game_->groundLevel_ - game_->trex_.getHeight() -
                     sprite_.rows() + val - 2;
      break;
    case PterodactylHeight::HIGH:
      sprite_.top_ =
          game_->groundLevel_ - game_->trex_.getHeight() - sprite_.rows() + val;
      break;
  }

  timer_ += deltaTime;
  if (timer_ > switchTime_) {
    changeAsset(name_ == PterodactylAsset::UP ? PterodactylAsset::DOWN
                                              : PterodactylAsset::UP);
    timer_ = 0;
  }

  Obstacle::update(deltaTime);
}
