#pragma once

#include <glm/glm.hpp>

struct Character
{
	unsigned int textureID;
	glm::ivec2 charSize;
	glm::ivec2 charBearing;
	signed long charAdvance;
};
