#pragma once

#include <vector>
#include <algorithm>
#include <map>

#include <GLAD/glad/glad.h>
#include <GLFWLib/glfw3.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include <iostream>
#include <math.h>

#include <../gameOfLife2D/src/headers/VAO.h>
#include <../gameOfLife2D/src/headers/VBO.h>
#include <../gameOfLife2D/src/headers/EBO.h>
#include <../gameOfLife2D/src/headers/texture.h>

#include <../gameOfLife2D/src/headers/shader.h>

#include <../gameOfLife2D/src/helper_structs/gridController.h>
#include <../gameOfLife2D/src/helper_structs/cell.h>
#include <../gameOfLife2D/src/helper_structs/character.h>



// FreeType, to remove later if possible
#include <ft2build.h>
#include FT_FREETYPE_H