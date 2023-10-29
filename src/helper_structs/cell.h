#pragma once

#include <glm/glm.hpp>
#include "../../../includes/GLAD/glad/glad.h"

class Cell {

private:
	bool isCellAlive;
	glm::ivec2 cellCoords;

public:

	Cell()
	{
		isCellAlive = false;
		cellCoords.x = 0;
		cellCoords.y = 0;
	};

	// Getters
	const GLboolean getCellLifeStatus()
	{
		return isCellAlive;
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

	void setCellCoords(const glm::ivec2& cellC)
	{
		cellCoords = cellC;
	}
	

};