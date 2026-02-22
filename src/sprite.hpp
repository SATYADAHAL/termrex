#ifndef SPRITE_INSTANCE_H
#define SPRITE_INSTANCE_H

#include <unistd.h>

#include <algorithm>
#include <cstdio>

#include "ansi_parser.hpp"
#include "sprite_asset.hpp"

class SpriteInstance {
 protected:
  std::vector<std::vector<Cell>> grid_{};

 public:
  const SpriteAsset* asset_;
  int top_;
  int left_;

  SpriteInstance(const SpriteAsset& asset_, int top_ = -1, int left_ = -1)
      : asset_(&asset_), top_(top_), left_(left_) {
    this->grid_ =
        parseAnsiGridRegex(this->asset_->ANSI_ART, this->asset_->COLS);
  }

  int getTop() const { return top_; }
  int getLeft() const { return left_; }
  int rows() const { return asset_->ROWS; }
  int cols() const { return asset_->COLS; }
  int bottom() const { return top_ + rows() - 1; }
  int right() const { return left_ + cols() - 1; }

  void setTop(int newTop) { top_ = newTop; }
  void setLeft(int newLeft) { left_ = newLeft; }
  void setPos(int top_, int left_) {
    this->top_ = top_;
    this->left_ = left_;
  }
  void moveBy(int dRow, int dCol) {
    top_ += dRow;
    left_ += dCol;
  }

  void print() const {
    if (!asset_ || asset_->ANSI_ART.empty()) return;
    for (int r = 0; r < rows(); ++r) {
      printf("\x1b[%d;%dH%s", top_ + r, left_, asset_->ANSI_ART[r].c_str());
    }
  }

  void render(int gameLeft, int gameRight) {
    if (this->left_ > gameRight - 1) return;
    if (this->right() < gameLeft + 1) return;

    int sprRight = (this->left_ + this->asset_->COLS);
    if (sprRight > gameRight - 1) {
      renderGridRightSlice(this->grid_, this->top_, this->left_,
                           (gameRight - this->left_ - 1));
      return;
    }

    if (this->left_ < gameLeft + 1) {
      renderGridLeftSlice(this->grid_, this->top_, gameLeft,
                          (gameLeft - left_ + 1));
      return;
    }
    print();
  }

  static bool collide(const SpriteInstance& a, const SpriteInstance& b) {
    if (a.right() < b.left_ || b.right() < a.left_ || a.bottom() < b.top_ ||
        b.bottom() < a.top_)
      return false;

    int rowStart = std::max(a.top_, b.top_);
    int rowEnd = std::min(a.bottom(), b.bottom());
    int colStart = std::max(a.left_, b.left_);
    int colEnd = std::min(a.right(), b.right());

    for (int r = rowStart; r <= rowEnd; ++r) {
      int ar = r - a.top_;
      int br = r - b.top_;

      if (ar < 0 || ar >= static_cast<int>(a.asset_->COLLISION_MASK.size()))
        continue;
      if (br < 0 || br >= static_cast<int>(b.asset_->COLLISION_MASK.size()))
        continue;

      for (int c = colStart; c <= colEnd; ++c) {
        int ac = c - a.left_;
        int bc = c - b.left_;

        if (ac < 0 ||
            ac >= static_cast<int>(a.asset_->COLLISION_MASK[ar].size()))
          continue;
        if (bc < 0 ||
            bc >= static_cast<int>(b.asset_->COLLISION_MASK[br].size()))
          continue;

        if (a.asset_->COLLISION_MASK[ar][ac] &&
            b.asset_->COLLISION_MASK[br][bc])
          return true;
      }
    }
    return false;
  }
};

#endif
