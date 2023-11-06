#pragma once

class GridController;

// Projection matrix variables (for zoom func).
struct projMatVars {
	float top = 1;
	float bot = -1;
	float right = +1;
	float left = -1;
};

struct CallbackData {
	GridController* gridController;
	SSBO* previousStateSBO = nullptr;
	SSBO* currentStateSBO = nullptr;
	SSBO* nextStateSBO = nullptr;;
	GLvoid* pMappedCurrent = nullptr;;
	GLvoid* pMappedNext = nullptr;;
	Shader* computeShader = nullptr;;
	VBO* gridBoxVBO = nullptr;;
	VBO* gridVBO = nullptr;;
	VBO* cellVBO = nullptr;;
	VBO* cellInstancesVBO = nullptr;;
	VAO* gridVAO = nullptr;;
	bool isIterating = false;
	int iterationSpeed = 5;
	int iterationCount = 0;
	projMatVars mainProjMat;
};

