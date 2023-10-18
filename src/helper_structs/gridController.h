#pragma once
#include <algorithm>

#include <GLFWLib/glfw3.h>

class GridController {

private:
	float zoomFactor, maxZoomIn, maxZoomOut;
	float gridxOffset, gridyOffset;
	float effectiveWidth = 2.f / zoomFactor, effectiveHeight = 2.f / zoomFactor;
	float offsetMaxXOffset = (1.0f - effectiveWidth / 2.0f) * scaleFactor, offsetMaxYOffset = (1.0f - effectiveHeight/ 2.0f) * scaleFactor;
	bool zoomIn;
	float scaleFactor = 0.85f;

	void updateEffectiveDimensions() {
		effectiveWidth = 2.f / zoomFactor;
		effectiveHeight = 2.f / zoomFactor;
	}

	void updateMaxOffsets() {
		offsetMaxXOffset = (1.0f - effectiveWidth / 2.0f) * scaleFactor;
		offsetMaxYOffset = (1.0f - effectiveHeight / 2.0f) * scaleFactor;

		gridxOffset = std::min(gridxOffset, offsetMaxXOffset);
		gridxOffset = std::max(gridxOffset, (-offsetMaxXOffset));
		gridyOffset = std::min(gridyOffset, offsetMaxYOffset);
		gridyOffset = std::max(gridyOffset, -(offsetMaxYOffset));
	}

	void clampXOffset()
	{
		if (gridxOffset > offsetMaxXOffset)
			gridxOffset = offsetMaxXOffset;
		else if (gridxOffset < -offsetMaxXOffset)
		{
			gridxOffset = -offsetMaxXOffset;
		}
	}

	void clampYOffset()
	{
		if (gridyOffset > offsetMaxYOffset)
			gridyOffset = offsetMaxYOffset;
		else if (gridyOffset < -offsetMaxYOffset)
		{
			gridyOffset = -offsetMaxYOffset;
		}
	}



public:
	GridController()
		: zoomFactor(1.f), maxZoomIn(20.f), maxZoomOut(1.f),
		gridxOffset(0.f), gridyOffset(0.f), zoomIn(true)
	{
		updateEffectiveDimensions();
		updateMaxOffsets();
	}

	void processInput(GridController& gridCtrl, void (*funcZoomOff) (GridController&), int* key)
	{
		float tempG = 0;
		switch (*key) {
			case GLFW_KEY_EQUAL:
				funcZoomOff(gridCtrl);
				break;

			case GLFW_KEY_MINUS:
				funcZoomOff(gridCtrl);
				break;
			case GLFW_KEY_LEFT:
				//gridxOffset = gridxOffset - 0.002f;
				tempG = gridCtrl.getGridXOffset();
				tempG -= 0.002f;
				gridCtrl.setGridXOffset(tempG);
				gridCtrl.clampXOffset();
				break;
			case GLFW_KEY_RIGHT:
				tempG = gridCtrl.getGridXOffset();
				tempG += 0.002f;
				gridCtrl.setGridXOffset(tempG);
				gridCtrl.clampXOffset();
				break;
			case GLFW_KEY_UP:
				tempG = gridCtrl.getGridYOffset();
				tempG += 0.002f;
				gridCtrl.setGridYOffset(tempG);
				gridCtrl.clampYOffset();
				break;
			case GLFW_KEY_DOWN:
				tempG = gridCtrl.getGridYOffset();
				tempG -= 0.002f;
				gridCtrl.setGridYOffset(tempG);
				gridCtrl.clampYOffset();
				break;
		}
	}

	/* float zoomFactor, maxZoomIn, maxZoomOut;
	float gridxOffset, gridyOffset;
	float effectiveWidth, effectiveHeight;
	float offsetMaxXOffset, offsetMaxYOffset;
	bool zoomIn;
	float scaleFactor = 0.85f; */
	/*Getters*/
	float& getZoomFactor()
	{
		return zoomFactor;
	}
	float& getMaxZoomIn()
	{
		return maxZoomIn;
	}
	float& getMaxZoomOut()
	{
		return maxZoomOut;
	}
	float& getGridXOffset()
	{
		return gridxOffset;
	}
	float& getGridYOffset()
	{
		return gridyOffset;
	}
	float& getEffectiveWidth()
	{
		return effectiveWidth;
	}
	float& getEffectiveHeight()
	{
		return effectiveHeight;
	}
	float& getOffsetMaxXOffset()
	{
		return offsetMaxXOffset;
	}
	float& getOffsetMaxYOffset()
	{
		return offsetMaxYOffset;
	}
	bool& getZoomInBool()
	{
		return zoomIn;
	}
	float& getScaleFactor()
	{
		return scaleFactor;
	}
	
	/*Setters*/
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
	void setEffectiveWidth(float& effW)
	{
		effectiveWidth = effW;
	}
	void setEffectiveHeight(float& effH)
	{
		effectiveHeight = effH;
	}
	void setOffsetMaxXOffset(float& omXOffset)
	{
		offsetMaxXOffset = omXOffset;
	}
	void setOffsetMaxYOffset(float& omYOffset)
	{
		offsetMaxYOffset = omYOffset;
	}
	void setZoomInBool(bool& zoom)
	{ 
		zoomIn = zoom;
	}
	void setScaleFactor(float& scaleF)
	{
		scaleFactor = scaleF;
	}
};