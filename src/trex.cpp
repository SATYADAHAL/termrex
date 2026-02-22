#include "trex.hpp"

#include <iostream>

#include "game.hpp"
#include "sprite_asset.hpp"

Trex::Trex(const TrexAssetPack& sp, TrexType t, int top, int left, Game* g)
    : SpriteInstance(sp.idle_, top, left), pack_(sp), game_(g), type_(t) {
  physics_ = (type_ == TrexType::ASCII) ? Physics{350.0f, -164.0f, 250.0f}
                                        : Physics{300.0f, -114.0f, 250.0f};
  maxHeight_ = sp.idle_.ROWS;
}
void Trex::setAsset(const SpriteAsset& newAsset) { asset_ = &newAsset; }

void Trex::changeAsset(TrexAssetName name) {
  switch (name) {
    case TrexAssetName::JUMP:
      setAsset(pack_.jump_);
      this->name_ = TrexAssetName::JUMP;
      break;
    case TrexAssetName::RUN1:
      setAsset(pack_.run1_);
      this->name_ = TrexAssetName::RUN1;
      break;
    case TrexAssetName::RUN2:
      setAsset(pack_.run2_);
      this->name_ = TrexAssetName::RUN2;
      break;
    case TrexAssetName::DOWN1:
      setAsset(pack_.down1_);
      this->name_ = TrexAssetName::DOWN1;
      break;
    case TrexAssetName::DOWN2:
      setAsset(pack_.down2_);
      this->name_ = TrexAssetName::DOWN2;
      break;
    case TrexAssetName::IDLE:
      setAsset(pack_.idle_);
      this->name_ = TrexAssetName::IDLE;
      break;
  }
}

void Trex::pressJump() {
  if (currentStatus_ != TrexStatus::JUMPING) {
    vy_ = physics_.jumpImpulse_;
    currentStatus_ = TrexStatus::JUMPING;
    changeAsset(TrexAssetName::JUMP);
  }
}

void Trex::fallFast() {
  if (currentStatus_ == TrexStatus::JUMPING) {
    if (vy_ <= 0.0f) {
      vy_ = physics_.jumpImpulse_ * 0.5f;
    }
    vy_ += physics_.gravity_ * 3.5f;
  }
}

void Trex::crouch() {
  if (currentStatus_ == TrexStatus::RUNNING && game_->settings_.obstacleDino_) {
    currentStatus_ = TrexStatus::DOWN;
    changeAsset(TrexAssetName::DOWN1);
    return;
  } else if (currentStatus_ == TrexStatus::DOWN) {
    dunktime_ = 0.0;
  }
}

void Trex::standUP() {
  currentStatus_ = TrexStatus::RUNNING;
  dunktime_ = 0.0;
}

void Trex::updateLeg(float dt) {
  legTimer_ += dt;
  if (legTimer_ >= LEG_SWITCH_INTERVAL) {
    legTimer_ = 0.0f;
    legChange_ = !legChange_;
  }
  if (currentStatus_ == TrexStatus::RUNNING) {
    if (legChange_)
      changeAsset(TrexAssetName::RUN1);
    else
      changeAsset(TrexAssetName::RUN2);
    return;
  }
  if (currentStatus_ == TrexStatus::DOWN) {
    if (legChange_) {
      changeAsset(TrexAssetName::DOWN1);
    } else
      changeAsset(TrexAssetName::DOWN2);
  }
}

void Trex::update(float dt) {
  if (currentStatus_ != TrexStatus::JUMPING) {
    updateLeg(dt);
    return;
  }

  vy_ += physics_.gravity_ * dt;
  if (vy_ > physics_.maxFallVelocity_) vy_ = physics_.maxFallVelocity_;
  top_ = top_ + (vy_ * dt);
  float topBoundary = (float)this->game_->topLeft_.y_ + 1;
  // clamp to game top
  if (top_ < topBoundary) {
    top_ = topBoundary;
    vy_ = 0;
  }
  // don't let fall below ground level
  float minY = float(this->game_->groundLevel_ - getHeight());
  if ((float)top_ >= minY) {
    top_ = (int)minY;
    vy_ = 0;
    // on ground;
    currentStatus_ = TrexStatus::RUNNING;
    if (game_->input_.downHeld_) {
      crouch();
    }
  } else {
    currentStatus_ = TrexStatus::JUMPING;
  }
  return;
}

const SpriteAsset& Trex::getAsset() const {
  if (!asset_) {
    std::cerr << "Error: asset is not set!\n";
    std::exit(EXIT_FAILURE);
  }
  return *asset_;
}

int Trex::getWidth() { return asset_->COLS; }
int Trex::getHeight() { return asset_->ROWS; }

void Trex::reset() {
  this->setPos(this->game_->groundLevel_ - rows(),
               this->game_->topLeft_.x_ + 3);
  this->changeAsset(TrexAssetName::RUN1);
  currentStatus_ = TrexStatus::RUNNING;
  legChange_ = true;
  legTimer_ = 0.0f;
  vy_ = 0.0f;
  dunktime_ = 0.0f;
}

void Trex::changeEye() {
  if (this->currentStatus_ == TrexStatus::JUMPING ||
      this->currentStatus_ == TrexStatus::RUNNING) {
    printf("\033[%d;%dH", (int)(top_ + (float)pack_.eyes_.normal_.row_),
           left_ + pack_.eyes_.normal_.col_);
  } else {
    printf("\033[%d;%dH", (int)(top_ + (float)pack_.eyes_.down_.row_),
           left_ + pack_.eyes_.down_.col_);
  }
  if (type_ == TrexType::UNICODE) {
    printf("\033[7m▣\033[27m");  // ▣ ◈ ◇
  }
  if (type_ == TrexType::ASCII) {
    printf("X");
  }
}

void Trex::changeMouth() {
  if (this->currentStatus_ == TrexStatus::JUMPING ||
      this->currentStatus_ == TrexStatus::RUNNING) {
    printf("\033[%d;%dH", (int)(top_ + (float)pack_.mouth_.normal_.row_),
           left_ + pack_.mouth_.normal_.col_);
  } else {
    printf("\033[%d;%dH", (int)(top_ + (float)pack_.mouth_.down_.row_),
           left_ + pack_.mouth_.down_.col_);
  }
  if (type_ == TrexType::UNICODE) {
    printf("█▀▀");
  }
  if (type_ == TrexType::ASCII) {
    printf("  __|");
    printf("\033[4D\033[1A");
    printf("   ");
    printf("\033[2B\033[5D");
    printf("--'  ");
  }
}
