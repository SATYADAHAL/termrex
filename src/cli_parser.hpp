#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP

#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "version.hpp"

namespace CLI_Parser {
struct GameSettings {
  bool asciiOnly_ = false;
  bool obstacleDino_ = true;
  int keyRepeat_ = 200;
  bool skipIntro_ = false;
};

inline void printHelp() {
  std::cout << "TermRex Runner Game CLI\n"
            << "=======================\n\n"
            << "Usage:\n"
            << "    termrex [options]\n\n"
            << "Options:\n"
            << "    -h, --help               Show this help menu\n"
            << "    -v, --version            Show game version\n"
            << "    --ascii-only             Use ASCII characters only\n"
            << "    --unicode                Use Unicode characters (default)\n"
            << "    --no-obstacle-dino       Disable flying dinosaur "
            << "(Pterodactyl) obstacles\n"
            << "    --keyrepeat <ms>         Set custom key repeat delay in "
            << "milliseconds\n"
            << "    --skip-intro             Skip the intro screen and start "
            << "game immediately\n";
}

inline void printVersion() {
  std::cout << "v" << TERMREX_VERSION_STRING << "\n";
}

inline GameSettings parseArguments(int argc, char* argv[]) {
  GameSettings settings;
  static const struct option LONG_OPTIONS[] = {
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'v'},
      {"ascii-only", no_argument, 0, 0},
      {"unicode", no_argument, 0, 0},
      {"no-obstacle-dino", no_argument, 0, 0},
      {"keyrepeat", required_argument, 0, 0},
      {"skip-intro", no_argument, 0, 0},
      {0, 0, 0, 0}};

  int optionIndex = 0;
  int c;
  optind = 1;
  while ((c = getopt_long(argc, argv, "hv", LONG_OPTIONS, &optionIndex)) !=
         -1) {
    if (c == 'h') {
      printHelp();
      exit(0);
    } else if (c == 'v') {
      printVersion();
      exit(0);
    } else if (c == 0) {
      std::string optname = LONG_OPTIONS[optionIndex].name;
      if (optname == "ascii-only") {
        settings.asciiOnly_ = true;
      } else if (optname == "unicode") {
        settings.asciiOnly_ = false;
      } else if (optname == "no-obstacle-dino") {
        settings.obstacleDino_ = false;
      } else if (optname == "skip-intro") {
        settings.skipIntro_ = true;
      } else if (optname == "keyrepeat") {
        if (optarg) {
          try {
            settings.keyRepeat_ = std::stoi(optarg);
            if (settings.keyRepeat_ < 50) {
              std::cerr << "Warning: Key repeat delay is very low ("
                        << settings.keyRepeat_
                        << "ms). Performance may suffer.\n";
              exit(1);
            } else if (settings.keyRepeat_ > 1200) {
              std::cerr << "Warning: Key repeat delay is very high ("
                        << settings.keyRepeat_
                        << "ms). Performance may suffer.\n";
              exit(1);
            }
          } catch (const std::exception& e) {
            std::cerr << "Error: Invalid number for --keyrepeat: " << optarg
                      << ". Must be an integer.\n";
            exit(1);
          }
        } else {
          std::cerr << "Error: --keyrepeat requires a numeric argument.\n";
          exit(1);
        }
      }
    } else if (c == '?') {
      exit(1);
    } else {
      std::cerr << "Unknown error during argument parsing (c=" << c << ").\n";
      exit(1);
    }
  }

  return settings;
}
}  // namespace CLI_Parser

#endif
