#pragma once

class GridController;

struct CallbackData {
	GridController* gridController;
	SSBO* currentStateSBO;
	SSBO* nextStateSBO;
	GLvoid* pMappedCurrent;
	GLvoid* pMappedNext;
	Shader* computeShader;
	bool isIterating;
};