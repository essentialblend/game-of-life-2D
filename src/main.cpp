#pragma warning( disable : 4100 ) 

#include <GLAD/glad.h>
#include <GLFWLib/glfw3.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <vector>
#include <algorithm>

#include <iostream>
#include <math.h>

#include <../gameOfLife2D/src/headers/VAO.h>
#include <../gameOfLife2D/src/headers/VBO.h>
#include <../gameOfLife2D/src/headers/EBO.h>

#include <../gameOfLife2D/src/headers/shader.h>

#include <../gameOfLife2D/src/helper_structs/gridController.h>

constexpr unsigned int WIN_HEIGHT = 600;
constexpr unsigned int WIN_WIDTH = 800;

/*UNI GRID SIZE : A 10 x 10 grid : 10*/
constexpr int UNI_GRID_SIZE = 20; 
constexpr int VERTICES_PER_LINE = 6;

/*Quad dimensions to render grid inside the gridBox*/
float minQuadX = 0.f, minQuadY = 0.f, maxQuadX = 0.f, maxQuadY = 0.f, quadHeight = 0.f, quadWidth = 0.f;
float quadAspectRatio = 0.f;

/*---- GridBox vertices/indices ----*/
float gridBoxVertices[] = {
	 0.85f,  0.85f, 0.f,
	 0.85f, -0.85f, 0.f,
	-0.85f, -0.85f, 0.f,
	-0.85f,  0.85f, 0.f
};

unsigned int gridBoxIndices[] = {
	0, 1, 3,
	1, 2, 3
};

///*Variables to handle grid-zoom and grid offsets*/
//float zoomFactor = 1.f, maxZoomIn = 20.f, maxZoomOut = 1.f;
//float gridxOffset = 0.f, gridyOffset = 0.f;
//float effectiveWidth = 2.f / zoomFactor, effectiveHeight = 2.f / zoomFactor;
//float offsetMaxXOffset = (1.0f - effectiveWidth / 2.0f) * scaleFactor, offsetMaxYOffset = (1.0f - effectiveHeight / 2.0f) * scaleFactor;
//bool zoomIn = true;

GridController* mainGrid = new GridController();

/*Declarations*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, GridController& mGrid);
void setZoomAndDetermineOffsets(GridController& mGrid);
void setupGrid(VAO* gridVAO, VBO* gridVBO, std::vector<float>* gridPoints);
void setupGridBox(VAO* gridBVAO, VBO* gridBVBO, EBO* gridBEBO, float* gridBoxVert, unsigned int* gridBoxInd, size_t gridBoxVertice, size_t gridBoxIndice);

int main()
{
	/*----opengl initialization----*/
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*Debug output for error checking*/
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	/*Window creation*/
	GLFWwindow* mainWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "gameOfLife2D", NULL, NULL);
	if (mainWindow == NULL)
	{
		std::cout << "FAILED: Window creation!" << std::endl;
		glfwTerminate();
		return -1;
	}
	/*Initialize viewport w/ framebuffer size callback*/
	glfwMakeContextCurrent(mainWindow);
	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);

	/*Initialize GLAD*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "FAILED: GLAD init!" << std::endl;
		return -1;
	}
	/*---- ----*/

	/*Initialize Shaders*/
	/*Grid Shader*/
	Shader gridLineShader("src/shaders/grid.vert", "src/shaders/grid.frag");
	/*GridBox Shader*/
	Shader gridBoxShader("src/shaders/gridBox.vert", "src/shaders/gridBox.frag");



	/*---- Grid initialization ----*/
	std::vector<float>* gridPoints = new std::vector<float>;
	for (int i = (-UNI_GRID_SIZE)/2; i < (+UNI_GRID_SIZE)/2; i++)
	{
		/*Vertical*/
		gridPoints->push_back(mainGrid->getScaleFactor() * (float)i / (float)(UNI_GRID_SIZE / 2));
		gridPoints->push_back(-1.f);
		gridPoints->push_back(0.f);

		gridPoints->push_back(mainGrid->getScaleFactor() * (float)i / (float)(UNI_GRID_SIZE / 2));
		gridPoints->push_back(+1.f);
		gridPoints->push_back(0.f);

		/*Horizontal*/
		gridPoints->push_back(+1.f);
		gridPoints->push_back(mainGrid->getScaleFactor() * (float)i / (float)(UNI_GRID_SIZE / 2));
		gridPoints->push_back(0.f);

		gridPoints->push_back(-1.f);
		gridPoints->push_back(mainGrid->getScaleFactor() * (float)i / (float)(UNI_GRID_SIZE / 2));
		gridPoints->push_back(0.f);

	}

	/*---- Grid rendering ----*/
	/*Line VAO/VBO*/
	VAO gridVAO;
	VBO gridVBO;
	setupGrid(&gridVAO, &gridVBO, gridPoints);

	VAO gridBoxVAO;
	VBO gridBoxVBO;
	EBO gridBoxEBO;
	setupGridBox(&gridBoxVAO, &gridBoxVBO, &gridBoxEBO, gridBoxVertices, gridBoxIndices, sizeof(gridBoxVertices), sizeof(gridBoxIndices));


	

	/*---- Cell rendering ----*/
	VAO cellVAO;
	VBO cellVBO;
	EBO cellEBO;


	/*---- Rendering loop ----*/
	while (!glfwWindowShouldClose(mainWindow))
	{
		processInput(mainWindow, *mainGrid);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		gridBoxShader.UseShader();
		gridBoxVAO.bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glEnable(GL_SCISSOR_TEST);
		glScissor((GLsizei)minQuadX, (GLsizei)minQuadY, (GLsizei)quadWidth, (GLsizei)fabs(quadHeight));

		gridLineShader.UseShader();
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(mainGrid->getZoomFactor(), mainGrid->getZoomFactor(), 1.f));
		gridLineShader.setMat4("scaleMatrix", scaleMatrix);
		gridLineShader.setFloat("gridxOffset", mainGrid->getGridXOffset());
		gridLineShader.setFloat("gridyOffset", mainGrid->getGridYOffset());
		gridVAO.bind();
		glDrawArrays(GL_LINES, 0, (4 * UNI_GRID_SIZE));
		glDisable(GL_SCISSOR_TEST);

		/*---- ----*/
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		/*---- ----*/
	}
	
	/*Memory management*/
	delete gridPoints;

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	/*Update gridBox dimensions to conform to resize*/
	/*Setting up quad dimensions for glScissor*/
	minQuadX = (1 - 0.85f) * 0.5f * width;
	minQuadY = (1 - 0.85f) * 0.5f * height;

	maxQuadX = (1 + 0.85f) * 0.5f * width;
	maxQuadY = (1 + 0.85f) * 0.5f * height;

	quadWidth = maxQuadX - minQuadX;
	quadHeight = maxQuadY - minQuadY;

	quadAspectRatio = quadWidth / quadHeight;

}

void processInput(GLFWwindow* window, GridController& mGrid)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		mGrid.setZoomFactor(5.f);
	}
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		bool T = true;
		mGrid.setZoomInBool(T);
		int keyVal = GLFW_KEY_EQUAL;
		mGrid.processInput(mGrid, setZoomAndDetermineOffsets, &keyVal);
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		bool F = false;
		mGrid.setZoomInBool(F);
		int keyVal = GLFW_KEY_MINUS;
		mGrid.processInput(mGrid, setZoomAndDetermineOffsets, &keyVal);
	
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		int keyVal = GLFW_KEY_LEFT;
		mGrid.processInput(mGrid, setZoomAndDetermineOffsets, &keyVal);
		std::cout << "grid X Off Left: " << mGrid.getGridXOffset() << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		int keyVal = GLFW_KEY_RIGHT;
		mGrid.processInput(mGrid, setZoomAndDetermineOffsets, &keyVal);
		std::cout << "grid X Off Right: " << mGrid.getGridXOffset() << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		int keyVal = GLFW_KEY_UP;
		mGrid.processInput(mGrid, setZoomAndDetermineOffsets, &keyVal);
		std::cout << "grid Y Off Up: " << mGrid.getGridYOffset() << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		int keyVal = GLFW_KEY_DOWN;
		mGrid.processInput(mGrid, setZoomAndDetermineOffsets, &keyVal);

	}

}

void setZoomAndDetermineOffsets(GridController& mGrid)
{
	if (mGrid.getZoomInBool())
	{
		float tempZoomF = mGrid.getZoomFactor();
		mGrid.setZoomFactor(tempZoomF *= 1.02f);
		if (mGrid.getZoomFactor() > mGrid.getMaxZoomIn())
		{
			mGrid.setZoomFactor(mGrid.getMaxZoomIn());
			//*zoomFac = *maxZoomI;
		}

	}
	else
	{
		/**zoomFac /= 1.02f;
		if (*zoomFac < *maxZoomO)
			*zoomFac = *maxZoomO;*/
		float tempZoomF = mGrid.getZoomFactor();
		mGrid.setZoomFactor(tempZoomF /= 1.02f);
		if (mGrid.getZoomFactor() < mGrid.getMaxZoomOut())
		{
			mGrid.setZoomFactor(mGrid.getMaxZoomOut());
		}
	}
	//*effectiveW = 2.0f / *zoomFac;
	//*effectiveH = 2.0f / *zoomFac;
	//*offsetMaxX = (1.0f - *effectiveW / 2.0f) * (*scaleFac);
	//*offsetMaxY = (1.0f - *effectiveH / 2.0f) * (*scaleFac);

	//*gridxOff = std::min(*gridxOff, *offsetMaxX);
	//*gridxOff = std::max(*gridxOff, -(*offsetMaxX));
	//*gridyOff = std::min(*gridyOff, *offsetMaxY);
	//*gridyOff = std::max(*gridyOff, -(*offsetMaxY));

	float tempEffWH = 2.f / mGrid.getZoomFactor();
	mGrid.setEffectiveWidth(tempEffWH);
	mGrid.setEffectiveHeight(tempEffWH);

	float tempOffMaxXOff = (1.f - (mGrid.getEffectiveWidth() / 2.f)) * (mGrid.getScaleFactor());
	float tempOffMaxYOff = (1.f - (mGrid.getEffectiveHeight() / 2.f)) * (mGrid.getScaleFactor());
	mGrid.setOffsetMaxXOffset(tempOffMaxXOff);
	mGrid.setOffsetMaxYOffset(tempOffMaxYOff);

	float tempGridX = std::min(mGrid.getGridXOffset(), mGrid.getOffsetMaxXOffset());
	tempGridX = std::max(mGrid.getGridXOffset(), -mGrid.getOffsetMaxXOffset());
	float tempGridY = std::min(mGrid.getGridYOffset(), -mGrid.getOffsetMaxYOffset());
	tempGridY = std::max(mGrid.getGridYOffset(), -mGrid.getOffsetMaxYOffset());

	mGrid.setGridXOffset(tempGridX);
	mGrid.setGridYOffset(tempGridY);
}

void setupGrid(VAO* gridVAO, VBO* gridVBO, std::vector<float>* gridPoints)
{
	gridVAO->bind();
	gridVBO->bind();
	glBufferData(GL_ARRAY_BUFFER, gridPoints->size() * sizeof(float), gridPoints->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void setupGridBox(VAO* gridBVAO, VBO* gridBVBO, EBO* gridBEBO,  float* gridBoxVert, unsigned int* gridBoxInd, size_t gridBoxVertice, size_t gridBoxIndice)
{
	gridBVAO->bind();

	gridBVBO->bind();
	glBufferData(GL_ARRAY_BUFFER, gridBoxVertice, gridBoxVert, GL_STATIC_DRAW);

	gridBEBO->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridBoxIndice, gridBoxInd, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*Setting up quad dimensions for glScissor*/
	minQuadX = (1 - 0.85f) * 0.5f * WIN_WIDTH;
	minQuadY = (1 - 0.85f) * 0.5f * WIN_HEIGHT;

	maxQuadX = (1 + 0.85f) * 0.5f * WIN_WIDTH;
	maxQuadY = (1 + 0.85f) * 0.5f * WIN_HEIGHT;

	quadWidth = maxQuadX - minQuadX;
	quadHeight = maxQuadY - minQuadY;
}
