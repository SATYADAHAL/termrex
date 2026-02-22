#include <stdlib.h>
#include <sys/time.h>

#include "cli_parser.hpp"
#include "game.hpp"

int main(int argc, char *argv[]) {
  CLI_Parser::GameSettings setting = CLI_Parser::parseArguments(argc, argv);
  Game game = Game(setting);
  game.loop();
  return 0;
}
