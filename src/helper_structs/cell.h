#pragma once

#include <glm/glm.hpp>
#include "../../../includes/GLAD/glad/glad.h"

class Cell {

private:
	bool isCellAlive;
	glm::ivec2 cellCoords;
	glm::vec2 cellOffsetVerticesXY;

public:

	Cell()
	{
		isCellAlive = false;
		cellCoords.x = 0;
		cellCoords.y = 0;
		cellOffsetVerticesXY.x = 0.f;
		cellOffsetVerticesXY.y = 0.f;
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
	const glm::vec2& getCellOffsetVertices()
	{
		return cellOffsetVerticesXY;
	}

	// Setters
	void setCellLifeStatus(const bool lifeSt)
	{
		isCellAlive = lifeSt;
	}
	void setCellCoords(const glm::ivec2& cellC)
	{
		cellCoords = cellC;
	}
	void setCellOffsetVertices(const glm::vec2& cellV)
	{
		cellOffsetVerticesXY = cellV;
	}
	

};