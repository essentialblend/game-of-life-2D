#pragma once
#include <algorithm>
#include <iostream>
#include <vector>

#include "../headers/VAO.h"
#include "../headers/VBO.h"
#include "../headers/EBO.h"
#include "cell.h"

#include <GLFWLib/glfw3.h>



class GridController {

private:
	// Grid variables
	int numRowCells = 0;
	int numColumnCells = 0;
	float zoomFactor, maxZoomIn, maxZoomOut;
	float gridxOffset, gridyOffset;
	bool zoomIn;
	std::vector<float> gridPointsArray;
	std::vector<float> cellVerticesArray;
	// Window variables
	unsigned int windowWidth, windowHeight;
	/*Grid-box quad dimension variables*/
	float minQuadX, maxQuadX, minQuadY, maxQuadY;
	float quadWidth, quadHeight;
	float centerOffsetPostGridBoxResize;
	float offsetForMouseInGridCheck = 0.f;
	// Gridbox size: 0.85 means the quad spans an NDC distance of -0.85 to 0.85
	float cellSize, gridBoxSize;
	std::vector<float> gridBoxQuadVerticesArray;
	std::vector<unsigned int> gridBoxQuadIndicesArray;
	std::vector<Cell> masterCellList;
	glm::ivec2 clickedCellRowColumn;
	std::vector<glm::vec2> offsetsForInstancedRendering;


	void generateGridPoints()
	{
		float testOffset = 0.002f;
		// Generate vertices for vertical lines
		for (int i = 1; i < (numRowCells); ++i)
		{
			float xLine = minQuadX + (i * cellSize);
			if (xLine < minQuadX || xLine > maxQuadX)
				continue;

			// Convert to NDC
			float xLineNDC = (2.f * (xLine / windowWidth)) - 1.f + centerOffsetPostGridBoxResize;
			float minQuadY_NDC = (2.f * (minQuadY / windowHeight)) - 1.f;
			float maxQuadY_NDC = (2.f * (maxQuadY / windowHeight)) - 1.f;

			// Vertical lines
			// Start point
			gridPointsArray.push_back(xLineNDC);
			gridPointsArray.push_back(minQuadY_NDC + testOffset);
			gridPointsArray.push_back(0.f);

			// End point
			gridPointsArray.push_back(xLineNDC);
			gridPointsArray.push_back(maxQuadY_NDC);
			gridPointsArray.push_back(0.f);
		}
		// Generate vertices for horizontal lines
		for (int i = 1; i < (numColumnCells); ++i)
		{
			float yLine = minQuadY + (i * cellSize);
			if (yLine < minQuadY || yLine > maxQuadY)
				continue;

			// Convert to NDC
			float yLineNDC = (2.f * (yLine / windowHeight)) - 1.f;
			float minQuadX_NDC = (2.f * (minQuadX / windowWidth)) - 1.f + centerOffsetPostGridBoxResize;
			float maxQuadX_NDC = (2.f * (maxQuadX / windowWidth)) - 1.f + centerOffsetPostGridBoxResize;

			// Horizontal lines
			// Start point
			gridPointsArray.push_back(minQuadX_NDC + testOffset);
			gridPointsArray.push_back(yLineNDC);
			gridPointsArray.push_back(0.f);

			// End point
			gridPointsArray.push_back(maxQuadX_NDC);
			gridPointsArray.push_back(yLineNDC);
			gridPointsArray.push_back(0.f);
		}
	}

	void computeGridBoxDimensions(const float excessWidth_Pixels)
	{
		minQuadX = (1 - (gridBoxSize)) * 0.5f * windowWidth;
		minQuadY = (1 - (gridBoxSize)) * 0.5f * windowHeight;

		maxQuadX = (((1 + (gridBoxSize)) * 0.5f * windowWidth) - (excessWidth_Pixels));
		maxQuadY = (1 + (gridBoxSize)) * 0.5f * windowHeight;

		quadWidth = maxQuadX - minQuadX;
		quadHeight = maxQuadY - minQuadY;
	}

	void generateVerticesAndInstancedOffsetsFromGrid()
	{
		offsetsForInstancedRendering.clear();

		for (int row = 0; row < (1); ++row)
		{
			for (int col = 0; col < 2; ++col)
			{
				Cell tempCell;
				
				std::vector<float> tempVertexArray;
				tempVertexArray.clear();


				// Bottom left vertex
				float bottomLeftX = minQuadX + col * cellSize;
				float bottomLeftY = minQuadY + row * cellSize;

				// Check for redundancy: Convert to NDC and store vertices for our Cell object instance
				tempVertexArray.push_back(2.f * (bottomLeftX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
				tempVertexArray.push_back(2.f * (bottomLeftY / windowHeight ) - 1.f);
				tempVertexArray.push_back(0.f);

				// Bottom right vertex
				float bottomRightX = bottomLeftX + cellSize;
				float bottomRightY = bottomLeftY;

				tempVertexArray.push_back(2.f * (bottomRightX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
				tempVertexArray.push_back(2.f * (bottomRightY / windowHeight) - 1.f);
				tempVertexArray.push_back(0.f);

				// Top right vertex
				float topRightX = bottomRightX;
				float topRightY = bottomLeftY + cellSize;

				tempVertexArray.push_back(2.f * (topRightX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
				tempVertexArray.push_back(2.f * (topRightY / windowHeight) - 1.f);
				tempVertexArray.push_back(0.f);


				// Top left vertex
				float topLeftX = bottomLeftX;
				float topLeftY = topRightY;

				tempVertexArray.push_back(2.f * (topLeftX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
				tempVertexArray.push_back(2.f * (topLeftY / windowHeight) - 1.f);
				tempVertexArray.push_back(0.f);

				//Store vertices for the one cell and add the cell to our master cell list
				tempCell.setCellQuadVertices(tempVertexArray);
				tempCell.setCellCoords(glm::ivec2(row, col));
				masterCellList.push_back(tempCell);

				// Convert to NDC and store to master cell vertices array
				cellVerticesArray.push_back((2.f * (bottomLeftX / windowWidth)) - 1.f + centerOffsetPostGridBoxResize);
				cellVerticesArray.push_back((2.f * (bottomLeftY / windowHeight)) - 1.f);
				cellVerticesArray.push_back(0.f);

				cellVerticesArray.push_back((2.f * (bottomRightX / windowWidth)) - 1.f + centerOffsetPostGridBoxResize);
				cellVerticesArray.push_back((2.f * (bottomRightY / windowHeight)) - 1.f);
				cellVerticesArray.push_back(0.f);

				cellVerticesArray.push_back((2.f * (topRightX / windowWidth)) - 1.f + centerOffsetPostGridBoxResize);
				cellVerticesArray.push_back((2.f * (topRightY / windowHeight)) - 1.f);
				cellVerticesArray.push_back(0.f);

				cellVerticesArray.push_back((2.f * (topLeftX / windowWidth)) - 1.f + centerOffsetPostGridBoxResize);
				cellVerticesArray.push_back((2.f * (topLeftY / windowHeight)) - 1.f);
				cellVerticesArray.push_back(0.f);

				// generate and store offsets for instanced rendering. maybe issue
				float offsetX = (col * cellSize);
				float offsetY = (row * cellSize);

				glm::vec2 relOff = glm::vec2(
					(2.f * (offsetX / windowWidth)),
					(2.f * (offsetY / windowHeight))
				);
				offsetsForInstancedRendering.push_back(relOff);
			}
		}
	}

public:
	GridController(const unsigned int wW, const unsigned int wH, const float gridBS, const int numRC)
	{
		// Set variables
		numRowCells = numRC;
		numColumnCells = 0;
		windowWidth = wW;
		windowHeight = wH;
		minQuadX = 0.f;
		maxQuadX = 0.f;
		minQuadY = 0.f;
		maxQuadY = 0.f;
		quadWidth = 0.f;
		quadHeight = 0.f;
		cellSize = 0.f;
		gridBoxSize = gridBS;
		centerOffsetPostGridBoxResize = 0.f;
		clickedCellRowColumn.x = 0;
		clickedCellRowColumn.y = 0;


		// Clear the arrays
		gridPointsArray.clear();
		cellVerticesArray.clear();
		gridBoxQuadVerticesArray.clear();
		gridBoxQuadIndicesArray.clear();
		masterCellList.clear();

		// Generate gridBox dimensions
		computeGridBoxDimensions(0.f);

		float smallestDimension = std::min(quadHeight, quadWidth);
		cellSize = smallestDimension / numRowCells;
		numRowCells = static_cast<int>(quadWidth / cellSize);
		numColumnCells = static_cast<int>(quadHeight / cellSize);

		// Account for perfectly squared grid cells
		float excessWidth_Pixels = quadWidth - (cellSize * (numRowCells));
		float excessWidth_NDC = (2.f * (excessWidth_Pixels / windowWidth));

		centerOffsetPostGridBoxResize = excessWidth_NDC * 0.5f;
		offsetForMouseInGridCheck = excessWidth_Pixels * 0.5f;

		// Init grid-box vertices
		gridBoxQuadVerticesArray = {
			(gridBoxSize - excessWidth_NDC) + centerOffsetPostGridBoxResize,  gridBoxSize, 0.f,
			(gridBoxSize - excessWidth_NDC) + centerOffsetPostGridBoxResize, -gridBoxSize, 0.f,
			-gridBoxSize + centerOffsetPostGridBoxResize, -gridBoxSize, 0.f,
			-gridBoxSize + centerOffsetPostGridBoxResize,  gridBoxSize, 0.f
		};

		gridBoxQuadIndicesArray = {
			0, 1, 3,
			1, 2, 3
		};

		computeGridBoxDimensions(excessWidth_Pixels);

		// Generate grid-points
		generateGridPoints();

		// Generate cell vertices from gridpoints
		generateVerticesAndInstancedOffsetsFromGrid();
	}

	void setupGridBox(VAO& gridBVAO, VBO& gridBVBO)
	{
		const std::size_t indicesSize = gridBoxQuadIndicesArray.size() * sizeof(unsigned int);
		const std::size_t verticesSize = gridBoxQuadVerticesArray.size() * sizeof(float);

		// Set buffer storage for VBO and EBO.
		gridBVBO.setBufferStorage(nullptr, std::size_t(indicesSize + verticesSize), GL_DYNAMIC_STORAGE_BIT);

		// Use buffer sub-data to upload indices and vertices
		gridBVBO.setBufferSubData(0, verticesSize, gridBoxQuadVerticesArray.data());
		gridBVBO.setBufferSubData(verticesSize, indicesSize, gridBoxQuadIndicesArray.data());

		// Bind VBO and EBO to VAO.
		glVertexArrayVertexBuffer(gridBVAO.getVAO(), 0, gridBVBO.getVBO(), 0, 3 * sizeof(float));
		glVertexArrayElementBuffer(gridBVAO.getVAO(), gridBVBO.getVBO());

		// Enable the vertices attribute.
		glEnableVertexArrayAttrib(gridBVAO.getVAO(), 0);

		// Specify the attribute format.
		glVertexArrayAttribFormat(gridBVAO.getVAO(), 0, 3, GL_FLOAT, GL_FALSE, 0);

		// Specify which VBO to get the attributes from.
		glVertexArrayAttribBinding(gridBVAO.getVAO(), 0, 0);
	}

	void setupGrid(VAO& gridVAO, VBO& gridVBO, const std::vector<float>& gridP)
	{
		/*gridVAO.bind();
		gridVBO.bind();
		glBufferData(GL_ARRAY_BUFFER, gridP.size() * sizeof(float), gridP.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);*/

		gridVBO.setBufferStorage(gridP.data(), gridP.size() * sizeof(float), GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(gridVAO.getVAO(), 0, gridVBO.getVBO(), 0, 3 * sizeof(float));

		// Enable the vertices attribute.
		glEnableVertexArrayAttrib(gridVAO.getVAO(), 0);

		// Specify the attribute format.
		glVertexArrayAttribFormat(gridVAO.getVAO(), 0, 3, GL_FLOAT, GL_FALSE, 0);

		// Specify which VBO to get the attributes from.
		glVertexArrayAttribBinding(gridVAO.getVAO(), 0, 0);
	}

	void processInput(const int* key, GLFWwindow& mainWindow)
	{
		switch (*key) {
			case GLFW_KEY_EQUAL:
				break;
			case GLFW_KEY_MINUS:
				break;
			case GLFW_KEY_LEFT:
				break;
			case GLFW_KEY_RIGHT:
				break;
			case GLFW_KEY_UP:
				break;
			case GLFW_KEY_DOWN:
				break;
			case GLFW_MOUSE_BUTTON_LEFT:
				bool retFlag;
				activateCell(mainWindow, retFlag);
				if (retFlag) return;
				break;
		}
	}

	void activateCell(GLFWwindow& mainWindow, bool& retFlag)
	{
		retFlag = true;
		double pixelMouseX = 0, pixelMouseY = 0;
		glfwGetCursorPos(&mainWindow, &pixelMouseX, &pixelMouseY);

		// Check if mouse click was inside the grid-box
		if (pixelMouseX < minQuadX || pixelMouseX >(maxQuadX + offsetForMouseInGridCheck) || (windowHeight - pixelMouseY) < minQuadY || (windowHeight - pixelMouseY) > maxQuadY)
		{
			return;
		}
		// Determine which cell was clicked.
		double gridX = ((pixelMouseX - offsetForMouseInGridCheck) - minQuadX) / cellSize;
		double gridY = (pixelMouseY - minQuadY) / cellSize;

		clickedCellRowColumn.x = static_cast<int>(std::floor(gridY));
		clickedCellRowColumn.y = static_cast<int>(std::floor(gridX));

		for (Cell& cell : masterCellList)
		{
			glm::ivec2 currentCellCoords = cell.getCellCoords();
			if (currentCellCoords == clickedCellRowColumn)
			{
				std::cout << "Cell clicked: " << clickedCellRowColumn.x << " - " << clickedCellRowColumn.y << "\n";
				std::cout << "Pre cell activation: " << cell.getCellLifeStatus() << "\n";
				cell.flipCellLifeStatus();
				std::cout << "Post cell activation: " << cell.getCellLifeStatus() << "\n";
			}
		}
		retFlag = false;
	}

	// Getters
	const float& getZoomFactor()
	{
		return zoomFactor;
	}
	const float& getMaxZoomIn()
	{
		return maxZoomIn;
	}
	const float& getMaxZoomOut()
	{
		return maxZoomOut;
	}
	const float& getGridXOffset()
	{
		return gridxOffset;
	}
	const float& getGridYOffset()
	{
		return gridyOffset;
	}
	const bool getZoomInBool()
	{
		return zoomIn;
	}
	const std::vector<float>& getGridPointsArray()
	{
		return gridPointsArray;
	}
	const std::vector<float>& getCellVerticesArray()
	{
		return cellVerticesArray;
	}
	const std::vector<float>& getGridBoxVerticesArray()
	{
		return gridBoxQuadVerticesArray;
	}
	const std::vector<unsigned int>& getGridBoxQuadIndicesArray()
	{
		return gridBoxQuadIndicesArray;
	}
	const int& getNumRowCells()
	{
		return numRowCells;
	}
	const int& getNumColumnCells()
	{
		return numColumnCells;
	}
	const std::vector<Cell>& getMasterCellListArray()
	{
		return masterCellList;
	}
	const float& getCellSize()
	{
		return cellSize;
	}
	const std::vector<glm::vec2>& getInstanceOffsetsArray()
	{
		return offsetsForInstancedRendering;
	}
	const float& getOffsetPostGridBoxResize()
	{
		return centerOffsetPostGridBoxResize;
	}
	
	// Setters
	void setZoomFactor(float zoomF)
	{
		zoomFactor = zoomF;
	}
	void setMaxZoomIn(float& maxZI)
	{
		maxZoomIn = maxZI;
	}
	void setMaxZoomOut(float& maxZO)
	{
		maxZoomIn = maxZO;
	}
	void setGridXOffset(float& gridX)
	{
		gridxOffset = gridX;
	}
	void setGridYOffset(float& gridY)
	{
		gridyOffset = gridY;
	}
	void setZoomInBool(bool& zoom)
	{ 
		zoomIn = zoom;
	}
};