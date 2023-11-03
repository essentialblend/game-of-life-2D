#pragma once

class GridController;

struct CallbackData {
	GridController* gridController;
	SSBO* previousStateSBO;
	SSBO* currentStateSBO;
	SSBO* nextStateSBO;
	GLvoid* pMappedCurrent;
	GLvoid* pMappedNext;
	Shader* computeShader;
	bool isIterating;
};