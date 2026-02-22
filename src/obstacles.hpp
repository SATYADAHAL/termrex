#ifndef OBSTACLES_HPP
#define OBSTACLES_HPP

#include <functional>
#include <memory>
#include <random>
#include <vector>

#include "sprite.hpp"
#include "sprite_asset.hpp"

class Game;
// Obstacle=============================================================
class Obstacle {
 public:
  bool active_ = true;
  float distanceMoved_ = 0.0f;
  float time_{0.0f};
  Game* game_ = nullptr;
  SpriteInstance sprite_;
  Obstacle(const SpriteAsset& asset_, Game* g, int top_ = -1, int left_ = -1);
  void render();

  virtual void update(float deltaTime);
  virtual ~Obstacle() = default;
  int top() const { return sprite_.getTop(); }
  int left() const { return sprite_.getLeft(); }
};

// Cactus=============================================================
class Cactus : public Obstacle {
 public:
  Cactus(const SpriteAsset& asset_, Game* g);
  void update(float deltaTime) override;
};

// Pterodactyl==========================================================
enum class PterodactylAsset { UP, DOWN };
enum class PterodactylHeight { LOW, MID, HIGH };

class Pterodactyl : public Obstacle {
 public:
  Pterodactyl(const SpriteAsset& assetUp, const SpriteAsset& assetDown, Game* g,
              PterodactylHeight mode);

  PterodactylHeight mode_;
  PterodactylAsset name_;

  SpriteInstance spriteUp_;
  SpriteInstance spriteDown_;

  void setAsset(const SpriteInstance& newSprite);
  void changeAsset(PterodactylAsset newName);
  void update(float deltaTime) override;

  float timer_ = 0;
  float switchTime_ = 0.2f;
  bool frozen_ = false;
  void freeze() { frozen_ = true; }
};

// ObstacleSpawner=============================================================
class ObstacleSpawner {
 public:
  Game* game_;
  size_t maxObstacles_;
  size_t minDistance_;
  size_t maxDistance_;
  float nextSpawnDistance_;
  std::mt19937 gen_;
  std::vector<std::unique_ptr<Obstacle>> obstacles_;
  std::vector<std::function<std::unique_ptr<Obstacle>()>> factories_;

  ObstacleSpawner(Game* g, float minD, float maxD, int maxObs = 10);

  void registerFactory(std::function<std::unique_ptr<Obstacle>()> factory);
  void update(float deltaTime);
  void render() const;
  bool checkCollision(const SpriteInstance& trex) const;
  void reset();

  void freezePterodactyls() {
    for (auto& obs : obstacles_) {
      if (auto ptero = dynamic_cast<Pterodactyl*>(obs.get())) {
        ptero->freeze();
      }
    }
  }

 private:
  void spawnRandomObstacle();
};

#endif
