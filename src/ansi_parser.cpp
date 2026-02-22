#include "ansi_parser.hpp"

#include <unistd.h>

#include <regex>

static inline size_t utf8CharLen(unsigned char b) {
  if (b < 0x80) return 1;
  if ((b >> 5) == 0x6) return 2;
  if ((b >> 4) == 0xE) return 3;
  if ((b >> 3) == 0x1E) return 4;
  return 1;
}

static std::string nextUtf8Char(const std::string& s, size_t pos) {
  if (pos >= s.size()) return "";
  size_t len = utf8CharLen(static_cast<unsigned char>(s[pos]));

  if (pos + len > s.size()) len = s.size() - pos;
  return s.substr(pos, len);
}

std::vector<std::vector<Cell>> parseAnsiGridRegex(
    const std::vector<std::string>& lines, int cols) {
  std::vector<std::vector<Cell>> grid;
  std::regex ansiRegex("\x1b\\[[0-9;]*[A-Za-z]");
  std::string currentAnsi;
  for (const auto& line : lines) {
    std::vector<Cell> row(cols, {"", ""});
    int col = 0;
    auto it = std::sregex_iterator(line.begin(), line.end(), ansiRegex);
    auto end = std::sregex_iterator();
    size_t pos = 0;
    for (; it != end && col < cols; ++it) {
      std::smatch match = *it;
      size_t matchPos = static_cast<size_t>(match.position());
      while (pos < matchPos && col < cols) {
        std::string ch = nextUtf8Char(line, pos);
        if (ch.empty()) break;
        row[col++] = {ch, currentAnsi};
        pos += ch.size();
      }
      std::string seq = match.str();
      if (seq.empty()) {
        pos = matchPos + match.length();
        continue;
      }
      char code = seq.back();

      if (code == 'C') {
        try {
          size_t numStart = 2;
          size_t numLen = (seq.size() >= 3) ? seq.size() - 3 : 0;
          int n = 1;
          if (numLen > 0) {
            n = std::stoi(seq.substr(numStart, numLen));
          }
          col += n;
          if (col > cols) col = cols;
        } catch (...) {
          ++col;
        }
      } else {
        currentAnsi = seq;
      }
      pos = matchPos + match.length();
    }

    while (pos < line.size() && col < cols) {
      std::string ch = nextUtf8Char(line, pos);
      if (ch.empty()) break;
      row[col++] = {ch, currentAnsi};
      pos += ch.size();
    }

    grid.push_back(row);
  }
  return grid;
}

void renderGrid(const std::vector<std::vector<Cell>>& grid, int startRow,
                int startCol) {
  if (grid.empty()) return;

  const size_t ROWS = static_cast<int>(grid.size());
  const size_t COLS = static_cast<int>(grid[0].size());

  std::string output;
  output.reserve(ROWS * COLS * 4);
  std::string lastAnsi;
  for (size_t r = 0; r < ROWS; ++r) {
    output.append("\033[");
    output.append(std::to_string(startRow + r));
    output.push_back(';');
    output.append(std::to_string(startCol));
    output.push_back('H');

    int spaceCount = 0;
    lastAnsi.clear();

    for (size_t c = 0; c < COLS; ++c) {
      const auto& cell = grid[r][c];

      if (cell.ansi_ != lastAnsi) {
        if (spaceCount > 0) {
          output.append("\033[");
          output.append(std::to_string(spaceCount));
          output.push_back('C');
          spaceCount = 0;
        }
        if (!cell.ansi_.empty()) {
          output.append(cell.ansi_);
          lastAnsi = cell.ansi_;
        } else {
          lastAnsi.clear();
        }
      }

      if (cell.ch_.empty()) {
        ++spaceCount;
      } else {
        if (spaceCount > 0) {
          output.append("\033[");
          output.append(std::to_string(spaceCount));
          output.push_back('C');
          spaceCount = 0;
        }
        output.append(cell.ch_);
      }
    }

    if (spaceCount > 0) {
      output.append("\033[");
      output.append(std::to_string(spaceCount));
      output.push_back('C');
    }
  }
  output.append("\033[0m");
  fflush(stdout);
}

void renderGridLeftSlice(const std::vector<std::vector<Cell>>& grid,
                         int startRow, int startCol, int sliceLeft) {
  if (grid.empty()) return;

  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());
  int left = std::max(0, sliceLeft);
  printf("\x1b[%d;%dH", startRow, startCol + 1);
  for (int r = 0; r < rows; ++r) {
    const auto& currentRow = grid[r];
    for (int c = left; c < cols; ++c) {
      if (currentRow[c].ch_.empty()) {
        printf("\033[1C");
      } else {
        printf("%s%s", currentRow[c].ansi_.c_str(), currentRow[c].ch_.c_str());
      }
    }
    printf("\033[%dD\033[1B", cols - left);
  }
  fflush(stdout);
}

void renderGridRightSlice(const std::vector<std::vector<Cell>>& grid,
                          int startRow, int startCol, int sliceRight) {
  if (grid.empty()) return;

  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());
  int right = std::clamp(sliceRight, 0, cols);

  std::string output;
  for (int r = 0; r < rows; ++r) {
    output += "\033[" + std::to_string(startRow + r) + ";" +
              std::to_string(startCol) + "H";
    output += "\033[0m";
    std::string lastAnsi;
    for (int c = 0; c < right; ++c) {
      const auto& cell = grid[r][c];
      if (cell.ansi_ != lastAnsi) {
        if (!cell.ansi_.empty()) {
          output += cell.ansi_;
          lastAnsi = cell.ansi_;
        } else {
          output += "\033[0m";
          lastAnsi.clear();
        }
      }
      if (cell.ch_.empty()) {
        output += "\033[1C";
      } else {
        output += cell.ch_;
      }
    }
  }
  fputs(output.c_str(), stdout);
  fflush(stdout);
}
