#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <../gameOfLife2D/src/helper_structs/cell.h>
#include <../gameoflife2D/src/helper_structs/gridController.h>

class CellGrid {
private:
    int numRows, numColumns;
    std::vector<Cell> cellArray;
    float cellSize;

public:
    CellGrid(int rows, int cols)
        : numRows(rows), numColumns(cols)
    {
        cellSize = 1.f;
        
    }
};