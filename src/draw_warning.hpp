#include <cstring>

#include "terminal.hpp"

void Terminal::drawResizeWarning() {
  clear();
  hideCursor();
  int currentWidth = getWidth();
  int currentHeight = getHeight();
  const char* header = "warning";
  char current[50];
  snprintf(current, sizeof(current), "Current: %d x %d", currentWidth,
           currentHeight);
  char required[32];
  snprintf(required, sizeof(required), "Minimum: %d x %d", minCols_, minRows_);
  int boxWidth = 30;
  int boxHeight = 7;
  int startX = (currentWidth - boxWidth) / 2;
  int startY = (currentHeight - boxHeight) / 2;
  printf("\033[33m");
  mvXY(startX, startY);
  printf("╭");
  int headerLen = (int)strlen(header);
  int dashes = (boxWidth - headerLen - 2) / 2;
  for (int i = 0; i < dashes - 1; i++) printf("─");
  printf("┤");
  printf("%s", header);
  printf("├");
  int remaining = boxWidth - 2 - dashes - headerLen;
  for (int i = 0; i < remaining - 1; i++) printf("─");
  printf("╮");
  mvXY(startX, startY + 1);
  printf("│%*s│", boxWidth - 2, "");
  mvXY(startX, startY + 2);
  printf("│");
  printf("\033[0m");
  int currentLabelLen = (int)strlen("Current: ");
  int currentPad = (boxWidth - 2 - (int)strlen(current)) / 2;
  printf("%*s", currentPad, "");
  printf("Current: \033[31m%s\033[33m", current + currentLabelLen);
  printf("%*s│", boxWidth - 2 - currentPad - (int)strlen(current), "");
  mvXY(startX, startY + 3);
  printf("│");
  int requiredPad = (boxWidth - 2 - (int)strlen(required)) / 2;
  printf("\033[0m");
  printf("%*s%s%*s\033[33m│", requiredPad, "", required,
         boxWidth - 2 - requiredPad - (int)strlen(required), "");
  mvXY(startX, startY + 4);
  printf("│%*s│", boxWidth - 2, "");
  mvXY(startX, startY + 5);
  printf("╰");
  for (int i = 0; i < boxWidth - 2; i++) printf("─");
  printf("╯");
  printf("\033[0m");
}
