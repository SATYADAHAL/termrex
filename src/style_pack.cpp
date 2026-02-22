#include "style_pack.hpp"

#include "sprite_asset.hpp"
#include "sprite_asset_ascii.hpp"
#include "trex.hpp"

const std::vector<const SpriteAsset*> UNICODE_CACTUS_ASSETS = {
    &CACTUS_ASSET1, &CACTUS_ASSET2, &CACTUS_ASSET3, &CACTUS_ASSET4,
    &CACTUS_ASSET5, &CACTUS_ASSET6, &CACTUS_ASSET7};

const std::vector<std::pair<const SpriteAsset*, const SpriteAsset*>>
    UNICODE_PTERO_ASSETS = {{&PTERODACTYL_UP, &PTERODACTYL_DOWN}};

const ThemePack UNICODE_THEME = {TREX_UNICODE_PACK,     TrexType::UNICODE,
                                 ArtStyle::UNICODE,     ScoreStyle::UNICODE,
                                 INTRO_BANNER,          GAME_OVER_BANNER,
                                 UNICODE_CACTUS_ASSETS, UNICODE_PTERO_ASSETS};

const std::vector<const SpriteAsset*> ASCII_CACTUS_ASSETS = {
    &CACTUS_ASSET_ASCI_I1, &CACTUS_ASSET_ASCI_I2, &CACTUS_ASSET_ASCI_I3,
    &CACTUS_ASSET_ASCI_I4, &CACTUS_ASSET_ASCI_I5};

const std::vector<std::pair<const SpriteAsset*, const SpriteAsset*>>
    ASCII_PTERO_ASSETS = {{&PTERODACTYL_UP_ASCII, &PTERODACTYL_DOWN_ASCII}};

const ThemePack ASCII_THEME = {TREX_ASCII_PACK,     TrexType::ASCII,
                               ArtStyle::ASCII,     ScoreStyle::ASCII,
                               INTRO_BANNER_ASCII,  GAME_OVER_BANNER_ASCII,
                               ASCII_CACTUS_ASSETS, ASCII_PTERO_ASSETS};
