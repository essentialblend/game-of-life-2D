#pragma once

#include <glm/glm.hpp>

class Cell {

private:
	std::vector<float> cellQuadVertices;
	bool isCellAlive;
	glm::ivec2 cellCoords;

public:

	Cell()
	{
		isCellAlive = false;
		cellQuadVertices.clear();
		cellCoords.x = 0;
		cellCoords.y = 0;
	};

	Cell(const std::vector<float>& cellQuadVert, bool isAlive) : Cell()

	{
		cellQuadVertices = cellQuadVert;
		isCellAlive = isAlive;
		cellCoords.x = 0;
		cellCoords.y = 0;
	};

	// Getters
	const bool getCellLifeStatus()
	{
		return isCellAlive;
	}
	const std::vector<float>& getCellQuadVertices()
	{
		return cellQuadVertices;
	}
	const glm::ivec2& getCellCoords()
	{
		return cellCoords;
	}

	// Setters
	void flipCellLifeStatus()
	{
		if (isCellAlive)
			isCellAlive = false;
		else
			isCellAlive = true;
	}
	void setCellLifeStatus(const bool lifeSt)
	{
		isCellAlive = lifeSt;
	}
	void setCellQuadVertices(const std::vector<float>& cellQuadVert)
	{
		cellQuadVertices.clear();
		cellQuadVertices = cellQuadVert;
	}
	void setCellCoords(const glm::ivec2& cellC)
	{
		cellCoords = cellC;
	}
	

};