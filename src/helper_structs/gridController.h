#pragma once
#include <algorithm>
#include <iostream>
#include <vector>

#include "../headers/VAO.h"
#include "../headers/VBO.h"
#include "../headers/EBO.h"
#include "../headers/SSBO.h"
#include "../headers/texture.h"
#include "../headers/shader.h"

#include "callbackData.h"

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
	std::vector<glm::vec2> offsetsForInstancedRenderingArray;
	glm::ivec2 cellIndicesVertexAttribute;
	// Used as a base position for instanced rendering.
	std::vector<float> verticesForBaseCell;
	TextureObject cellStatesTextureBuffer;
	std::vector<int> masterCellCurrentLifeStatesArray;


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

	void generateInstancedOffsetsFromGrid()
	{
		
		offsetsForInstancedRenderingArray.resize(static_cast<GLint64>(numRowCells * numColumnCells));
		offsetsForInstancedRenderingArray.clear();


		// Bottom left vertex
		float bottomLeftX = minQuadX;
		float bottomLeftY = minQuadY;

		// Convert to NDC and store vertices for a single cell
		verticesForBaseCell.push_back(2.f * (bottomLeftX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
		verticesForBaseCell.push_back(2.f * (bottomLeftY / windowHeight) - 1.f);
		verticesForBaseCell.push_back(0.f);

		// Bottom right vertex
		float bottomRightX = bottomLeftX + cellSize;
		float bottomRightY = bottomLeftY;

		verticesForBaseCell.push_back(2.f * (bottomRightX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
		verticesForBaseCell.push_back(2.f * (bottomRightY / windowHeight) - 1.f);
		verticesForBaseCell.push_back(0.f);

		// Top right vertex
		float topRightX = bottomRightX;
		float topRightY = bottomLeftY + cellSize;

		verticesForBaseCell.push_back(2.f * (topRightX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
		verticesForBaseCell.push_back(2.f * (topRightY / windowHeight) - 1.f);
		verticesForBaseCell.push_back(0.f);

		// Top left vertex
		float topLeftX = bottomLeftX;
		float topLeftY = topRightY;

		verticesForBaseCell.push_back(2.f * (topLeftX / windowWidth) - 1.f + centerOffsetPostGridBoxResize);
		verticesForBaseCell.push_back(2.f * (topLeftY / windowHeight) - 1.f);
		verticesForBaseCell.push_back(0.f);

		// Generate and store offsets for instanced rendering
		for (int row = 0; row < (numRowCells); ++row)
		{
			for (int col = 0; col < numColumnCells; ++col)
			{
				float offsetX = row * cellSize;
				float offsetY = col * cellSize;

				glm::vec2 relOff = glm::vec2(
					(2.f * (offsetX / windowWidth)),
					(2.f * (offsetY / windowHeight))
				);
				offsetsForInstancedRenderingArray.push_back(relOff);

				Cell tempCell;
				tempCell.setCellCoords(glm::vec2(row, col));
				tempCell.setCellOffsetVertices(relOff);
				masterCellList.push_back(tempCell);
			}
		}
	}

public:
	GridController(const unsigned int wW, const unsigned int wH, const float gridBS, const int numRC)
	{
		// Set variables
		numColumnCells = numRC;
		numRowCells = 0;
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
		cellIndicesVertexAttribute.x = 0;
		cellIndicesVertexAttribute.y = 0;

		// Clear the arrays
		gridPointsArray.clear();
		gridBoxQuadVerticesArray.clear();
		gridBoxQuadIndicesArray.clear();
		masterCellList.clear();
		verticesForBaseCell.clear();
		masterCellCurrentLifeStatesArray.clear();

		// Generate gridBox dimensions
		computeGridBoxDimensions(0.f);

		float smallestDimension = static_cast<float>(std::min(quadHeight, quadWidth));
		cellSize = smallestDimension / numColumnCells;
		numRowCells = static_cast<int>(quadWidth / cellSize);
		numColumnCells = static_cast<int>(quadHeight / cellSize);

		// Account for perfectly squared grid cells
		float excessWidth_Pixels = static_cast<float>(quadWidth - (cellSize * (numRowCells)));
		float excessWidth_NDC = static_cast<float>((2.f * (excessWidth_Pixels / windowWidth)));

		centerOffsetPostGridBoxResize = excessWidth_NDC * 0.5f;
		offsetForMouseInGridCheck = excessWidth_Pixels * 0.5f;

		// Init grid-box vertices and indices.
		gridBoxQuadVerticesArray = {
			static_cast<float>((gridBoxSize - excessWidth_NDC) + centerOffsetPostGridBoxResize), static_cast<float>(gridBoxSize), 0.f,
			static_cast<float>((gridBoxSize - excessWidth_NDC) + centerOffsetPostGridBoxResize), static_cast<float>(-gridBoxSize), 0.f,
			static_cast<float>(-gridBoxSize + centerOffsetPostGridBoxResize), static_cast<float>(-gridBoxSize), 0.f,
			static_cast<float>(-gridBoxSize + centerOffsetPostGridBoxResize), static_cast<float>(gridBoxSize), 0.f
		};

		gridBoxQuadIndicesArray = {
			0, 1, 3,
			1, 2, 3
		};

		computeGridBoxDimensions(excessWidth_Pixels);

		// Generate grid-points
		generateGridPoints();

		// Generate cell vertices from gridpoints
		generateInstancedOffsetsFromGrid();

		masterCellCurrentLifeStatesArray.resize(0);
		for (Cell& cell : masterCellList)
		{
			masterCellCurrentLifeStatesArray.push_back(cell.getCellLifeStatus());
		}

		// Setup the texture we use as a buffer to pass lifestates to the fragment shader, used to enable instanced rendering.
		setupTextureForFragShaderLifeStatesBuffer();
	}

	void setupTextureForFragShaderLifeStatesBuffer()
	{
		cellStatesTextureBuffer.setParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, true);
		glTextureStorage2D(cellStatesTextureBuffer.getTextureObjectID(), 1, GL_R32I, numColumnCells, numRowCells);
		glTextureSubImage2D(cellStatesTextureBuffer.getTextureObjectID(), 0, 0, 0, numColumnCells, numRowCells, GL_RED_INTEGER, GL_INT, (void*)masterCellCurrentLifeStatesArray.data());
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

	void activateCell(GLFWwindow& mainWindow, bool& retFlag, CallbackData* callbackData)
	{

		std::vector<int> testA;
		testA.clear();
		testA.resize(static_cast<GLint64>(numColumnCells * numRowCells));

		/*glGetNamedBufferSubData(callbackData->currentStateSBO->getSSBO(), 0, testA.size() * sizeof(int), testA.data());
		glGetNamedBufferSubData(callbackData->nextStateSBO->getSSBO(), 0, testA.size() * sizeof(int), testA.data());*/
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		//int* currentArrayFromPointer = static_cast<int*>(callbackData->pMappedCurrent);
		//for (int i = 0; i < 80; i++)
		//{
		//	testA[i] = currentArrayFromPointer[i];
		//}

		//int* nextArrayFromPointer = static_cast<int*>(callbackData->pMappedNext);
		//for (int i = 0; i < 80; i++)
		//{
		//	testA[i] = nextArrayFromPointer[i];
		//}

		glGetNamedBufferSubData(callbackData->nextStateSBO->getSSBO(), 0, testA.size() * sizeof(int), testA.data());

		retFlag = true;
		double pixelMouseX = 0, pixelMouseY = 0;
		// Obtain cursor position coordinates.
		glfwGetCursorPos(&mainWindow, &pixelMouseX, &pixelMouseY);

		// Convert to be consistent with the grid-representation. For this implementation, the bottom left most cell is 0, 0. The cell right above 0, 0 is 0, 1..... upto 0, (numColumnCells - 1). The cell right of 0, 0 is 1, 0 .... upto (numRowCells - 1), 0 for the first (bottom most) row. 
		pixelMouseY = windowHeight - pixelMouseY;

		// Check if mouse click was inside the grid-box
		if (pixelMouseX < minQuadX || pixelMouseX > (maxQuadX + offsetForMouseInGridCheck) || (windowHeight - pixelMouseY) < minQuadY || (windowHeight - pixelMouseY) > maxQuadY)
		{
			return;
		}
		// Determine which cell was clicked.
		double gridX = ((pixelMouseX - offsetForMouseInGridCheck) - minQuadX) / cellSize;
		double gridY = (pixelMouseY - minQuadY) / cellSize;

		// Band-aid fix to prevent erroneous detection and runtime crashes.
		if (gridX < 0)
			return;

		clickedCellRowColumn.x = static_cast<int>(std::floor(gridX));
		clickedCellRowColumn.y = static_cast<int>(std::floor(gridY));

		// Convert the 2D index to a 1D index to use for our lifestates array.
		int index1D = clickedCellRowColumn.x * numColumnCells + clickedCellRowColumn.y;

		// Reflect the changes in our master life states array for the grid-controller instance.
		if (masterCellCurrentLifeStatesArray[index1D] == 0)
		{
			masterCellCurrentLifeStatesArray[index1D] = 1;
		}
		else
		{
			masterCellCurrentLifeStatesArray[index1D] = 0;
		}
		// Update the currentIterationSSBO content so the compute shader correctly uses it for the next iteration update.
		GLsync fenceVar = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glClientWaitSync(fenceVar, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
		glDeleteSync(fenceVar);
		std::memcpy(callbackData->pMappedCurrent, masterCellCurrentLifeStatesArray.data(), masterCellCurrentLifeStatesArray.size() * sizeof(int));
		//glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		// Update the texture array.
		glTextureSubImage2D(callbackData->gridController->getCellStatesTextureObject().getTextureObjectID(), 0, 0, 0, callbackData->gridController->getNumColumnCells(), callbackData->gridController->getNumRowCells(), GL_RED_INTEGER, GL_INT, masterCellCurrentLifeStatesArray.data());
		// Make sure that we only access this texture after the texture fully updates.
		glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
		

		for (Cell& cell : masterCellList)
		{
			glm::ivec2 currentCellCoords = cell.getCellCoords();
			if (currentCellCoords == static_cast<glm::ivec2>(clickedCellRowColumn))
			{
				std::cout << "Cell clicked: " << clickedCellRowColumn.x << " - " << clickedCellRowColumn.y << "\n";
				std::cout << "1D Index: " << index1D << "\n\n";
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
	const float& getOffsetPostGridBoxResize()
	{
		return centerOffsetPostGridBoxResize;
	}
	const glm::ivec2& getCellIndicesForVertexAttribute()
	{
		return cellIndicesVertexAttribute;
	}
	const std::vector<float>& getVerticesForOffsetsReference()
	{
		return verticesForBaseCell;
	}
	const std::vector<glm::vec2>& getInstanceOffsetsArray()
	{
		return offsetsForInstancedRenderingArray;
	}
	const TextureObject& getCellStatesTextureObject()
	{
		return cellStatesTextureBuffer;
	}
	const std::vector<int> getMasterCellLifeStatesArray()
	{
		return masterCellCurrentLifeStatesArray;
	}

	// Setters
	void setZoomFactor(const float zoomF)
	{
		zoomFactor = zoomF;
	}
	void setMaxZoomIn(const float& maxZI)
	{
		maxZoomIn = maxZI;
	}
	void setMaxZoomOut(const float& maxZO)
	{
		maxZoomIn = maxZO;
	}
	void setGridXOffset(const float& gridX)
	{
		gridxOffset = gridX;
	}
	void setGridYOffset(const float& gridY)
	{
		gridyOffset = gridY;
	}
	void setZoomInBool(const bool& zoom)
	{ 
		zoomIn = zoom;
	}
	void setMasterCellLifeStatesArray(const std::vector<int>& lifeStatesArray)
	{
		masterCellCurrentLifeStatesArray = lifeStatesArray;
	}
};