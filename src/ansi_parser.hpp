#ifndef ANSI_PARSER_H
#define ANSI_PARSER_H

#include <string>
#include <vector>

struct Cell {
  std::string ch_;
  std::string ansi_;
};

std::vector<std::vector<Cell>> parseAnsiGridRegex(
    const std::vector<std::string>& lines, int cols);

void renderGrid(const std::vector<std::vector<Cell>>& grid, int startRow = 1,
                int startCol = 1);
void renderGridSlice(const std::vector<std::vector<Cell>>& grid,
                     int startRow = 1, int startCol = 1, int sliceLeft = 0,
                     int sliceRight = -1);

void renderGridLeftSlice(const std::vector<std::vector<Cell>>& grid,
                         int startRow, int startCol, int sliceLeft = 0);

void renderGridRightSlice(const std::vector<std::vector<Cell>>& grid,
                          int startRow, int startCol, int sliceRight);

#endif
