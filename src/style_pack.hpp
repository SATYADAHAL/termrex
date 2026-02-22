#ifndef STYLE_PACK_HPP
#define STYLE_PACK_HPP

#include <utility>
#include <vector>

#include "game_over.hpp"
#include "score.hpp"
#include "sprite_asset.hpp"
#include "trex.hpp"

struct ThemePack {
  const TrexAssetPack& trexPack_;
  TrexType trexType_;

  // Banners & style
  ArtStyle artStyle_;
  ScoreStyle scoreStyle_;
  const SpriteAsset& introBanner_;
  const SpriteAsset& gameOverBanner_;

  // Obstacles
  const std::vector<const SpriteAsset*> CACTUS_ASSETS;
  const std::vector<std::pair<const SpriteAsset*, const SpriteAsset*>>
      PTERO_ASSETS;
};

extern const ThemePack UNICODE_THEME;
extern const ThemePack ASCII_THEME;

#endif
