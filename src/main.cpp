// All necessary includes combined to one header.
#include "headers/mainIncludes.h"
// Disabling warnings from stb_image.h.

#pragma warning( push )
#pragma warning( disable: 4244 )
#pragma warning( disable: 26819 )
#pragma warning( disable: 6262 )
#pragma warning( disable: 26451 )
#define STB_IMAGE_IMPLEMENTATION
#include <../stb_image.h>
#pragma warning(pop)

// Constexprs.
constexpr unsigned int WIN_HEIGHT = 600;
constexpr unsigned int WIN_WIDTH = 800;

// Number of cells per column (fixed). This is then used to determine how many cells we can fit in a row to help maintain perfectly squared grid cells.
constexpr int INIT_COLUMN_CELLS = 8 * 5;
// Used to determine the initial size of the grid-box relative to the entire window. 
constexpr float GRIDBOX_SIZE_SPAN = 0.85f;
// Scale factor used to contain the cell within the grid (instead of on it).
constexpr float CELL_SCALE_FACTOR = 1.f;

// Iteration update frequency.
constexpr float ITERATION_UPDATE_FREQ = 2;

// Function prototypes.
void initialize_glfw_mainWindow();
int main();
void computeTranslationAndVisualOffset(float& originalCellPosX, std::vector<float>& cellVertices, float& originalCellPosY, float& offX, float& offY);
void manageTimedActions(CallbackData& callbackData, double& lastUpdateTime, float iterationUpdateFrequency);
void setupCell(GridController* mainGridController, std::vector<Cell>& masterCellList, VBO& cellVBO, VAO& cellVAO, VBO& cellInstancesVBO, VBO& cellIVBO);
void renderGrid(Shader& mainGridShader, glm::mat4& scaleMatrix, VAO& gridVAO, GridController* mainGridController);
void renderGridBox(Shader& gridBoxShader, VAO& gridBoxVAO, GridController* mainGridController);
//int setupTextRenderingWithFreeType(FT_Library& freeTypeMainObject, FT_Face& fontFaceParchment, std::map<GLchar, Character>& masterMap);

//Opengl function prototypes
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
	GLsizei length, const char* message, const void* userParam);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{

	double lastUpdateTime = glfwGetTime();
	/*---- OPENGL INITIALIZATION ----*/
	initialize_glfw_mainWindow();

	/*Window creation for borderless*/
	//GLFWmonitor* mainMonitor = glfwGetPrimaryMonitor();

	/*Set borderless fullscreen as default mode before creating an instance for the main window
	const GLFWvidmode* videoMode = glfwGetVideoMode(mainMonitor);
	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);*/

	// Create the main window
	GLFWwindow* mainWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "gameOfLife2D", NULL, NULL);
	if (mainWindow == NULL)
	{
		std::cout << "ERROR: Window creation!" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set opengl context to this window on the calling thread.
	glfwMakeContextCurrent(mainWindow);

	// Set callback functions.
	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
	glfwSetInputMode(mainWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetMouseButtonCallback(mainWindow, mouse_button_callback);
	glfwSetKeyCallback(mainWindow, key_callback);

	// Initialize GLAD.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR: GLAD init failed!" << std::endl;
		return -1;
	}

	// Opengl options.
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);

	// enable OpenGL debug context if context allows for debug context
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}


	///*---- TEXT-RENDERING SETUP ----*/
	//// Initialize FreeType (to help use our font).
	//FT_Library freeTypeMainObject;
	//FT_Face fontFaceParchment;
	//std::map<GLchar, Character> masterCharactersMap;

	//setupTextRenderingWithFreeType(freeTypeMainObject, fontFaceParchment, masterCharactersMap);

	/*---- SHADER INITIALIZATION ----*/
	// GridBox shader.
	Shader gridBoxShader("src/shaders/gridBox.vert", "src/shaders/gridBox.frag");
	gridBoxShader.retrieveUniforms();
	// Cell shader.
	Shader mainCellShader("src/shaders/cell.vert", "src/shaders/cell.frag");
	mainCellShader.retrieveUniforms();
	// Grid shader.
	Shader mainGridShader("src/shaders/grid.vert", "src/shaders/grid.frag");
	mainGridShader.retrieveUniforms();
	// Compute shader.
	Shader gameLogicComputeShader("src/shaders/logicCompute.comp");
	gameLogicComputeShader.retrieveUniforms();

	/*---- GRID AND GRID-BOX SETUP ----*/
	// Instantiate the grid controller 
	GridController* mainGridController = new GridController(WIN_WIDTH, WIN_HEIGHT, GRIDBOX_SIZE_SPAN, INIT_COLUMN_CELLS);

	// Grid-box and grid buffers
	VAO gridBoxVAO;
	VBO gridBoxVBO;
	VAO gridVAO;
	VBO gridVBO;

	// Grid-box buffers setup.
	//float cellRenderScale = 0.95f;
	mainGridController->setupGridBox(gridBoxVAO, gridBoxVBO);

	// Grid-lines buffer setup.
	mainGridController->setupGrid(gridVAO, gridVBO, mainGridController->getGridPointsArray());

	// Now that we have a grid, we extract the cells
	std::vector<Cell> masterCellList;
	masterCellList.clear();
	masterCellList = mainGridController->getMasterCellListArray();

	// This array gets used to store life states for every cell in our grid and gets accessed by the fragment shader through a texture (sampler).
	std::vector<int> masterCellLifeStatesArray;
	for (Cell& cell : masterCellList)
	{
		masterCellLifeStatesArray.push_back(cell.getCellLifeStatus());
	}

	// Buffer setup for the compute shader.
	SSBO currentIterationSBO, nextIterationSBO;

	// Set up persistent mapped buffer for the current iteration buffer.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, currentIterationSBO.getSSBO());
	currentIterationSBO.setBufferStorage((void*) nullptr, masterCellLifeStatesArray.size() * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	GLvoid* pMappedBufferCurrent = glMapNamedBufferRange(currentIterationSBO.getSSBO(), 0, masterCellLifeStatesArray.size() * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Set up persistent mapped buffer for the next iteration buffer.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nextIterationSBO.getSSBO());
	nextIterationSBO.setBufferStorage((void*) nullptr, masterCellLifeStatesArray.size() * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	GLvoid* pMappedBufferNext = glMapNamedBufferRange(nextIterationSBO.getSSBO(), 0, masterCellLifeStatesArray.size() * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Set initial data for the persistent mapped SSBO buffers.
	if (pMappedBufferCurrent != nullptr && pMappedBufferNext != nullptr)
	{
		std::memcpy(pMappedBufferCurrent, masterCellLifeStatesArray.data(), masterCellLifeStatesArray.size() * sizeof(int));
		std::memcpy(pMappedBufferNext, masterCellLifeStatesArray.data(), masterCellLifeStatesArray.size() * sizeof(int));
	}
	else
	{
		std::cout << "COMPUTE SHADER: Persistently Mapped Buffer creation failed! " << std::endl;
		return -1;
	}

	/*---- CELLS SETUP ----*/
	VAO cellVAO;
	VBO cellVBO, cellInstancesVBO, cellIndicesVBO;

	setupCell(mainGridController, masterCellList, cellVBO, cellVAO, cellInstancesVBO, cellIndicesVBO);

	// Set callback data to obtain pointers from our window.
	CallbackData* mainData = new CallbackData;
	mainData->gridController = mainGridController;
	mainData->currentStateSBO = &currentIterationSBO;
	mainData->nextStateSBO = &nextIterationSBO;
	mainData->pMappedCurrent = pMappedBufferCurrent;
	mainData->pMappedNext = pMappedBufferNext;
	mainData->computeShader = &gameLogicComputeShader;
	mainData->isIterating = false;

	// Set the pointers.
	glfwSetWindowUserPointer(mainWindow, mainData);

	// Calculate the final offset simply for visual reasons to make the cell fit better within its grid points.
	std::vector<float> cellVertices = mainGridController->getVerticesForOffsetsReference();  
	float originalCellPosX = 0.0f;
	float originalCellPosY = 0.0f;
	float offX = 0.f, offY = 0.f;

	computeTranslationAndVisualOffset(originalCellPosX, cellVertices, originalCellPosY, offX, offY);
	
	/*---- RENDER LOOP ----*/
	while (!glfwWindowShouldClose(mainWindow))
	{

		// Set the color to clear the screen.
		glClearColor(0.33f, 0.33f, 0.33f, 1.f);
		// Clear the color and depth buffers.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind the SSBOs.
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, currentIterationSBO.getSSBO());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nextIterationSBO.getSSBO());


		// Render the grid-box that contains the base grid.
		renderGridBox(gridBoxShader, gridBoxVAO, mainGridController);

		// Handle cell state (or any other time-based action) execution.
		if(mainData->isIterating == true)
			manageTimedActions(*mainData, lastUpdateTime, ITERATION_UPDATE_FREQ);


		// Bind VAO.
		cellVAO.bind();
		// Bind Shader.
		mainCellShader.UseShader();
		// Set uniforms.
		glm::mat4 scaleMatrix = glm::mat4(1.f);
		scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(CELL_SCALE_FACTOR, CELL_SCALE_FACTOR, 0.f));
		mainCellShader.setMat4("scaleMatrix", scaleMatrix);
		mainCellShader.setVec2("translationOffset", glm::vec2(offX, offY));
		glBindTextureUnit(0, mainGridController->getCellStatesTextureObject().getTextureObjectID());
		mainCellShader.setInt("cellLifeStates", 0);
		// Draw cells.
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(mainGridController->getVerticesForOffsetsReference().size() * sizeof(float)), mainGridController->getNumColumnCells() * mainGridController->getNumRowCells());
		
		// Render the base grid.
		renderGrid(mainGridShader, scaleMatrix, gridVAO, mainGridController);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}
	
	// Memory management.
	// Delete the callback struct instance.
	delete mainData;
	// Delete the SSBOs (takes care of our PMBs).
	glUnmapNamedBuffer(currentIterationSBO.getSSBO());
	glUnmapNamedBuffer(nextIterationSBO.getSSBO());

	glfwTerminate();
	return 0;
}

void computeTranslationAndVisualOffset(float& originalCellPosX, std::vector<float>& cellVertices, float& originalCellPosY, float& offX, float& offY)
{
	// Calculate the average x and y position of the vertices to find the center position of the cell.
	for (int i = 0; i < 12; i += 3) {
		originalCellPosX += cellVertices[i];
		originalCellPosY += cellVertices[static_cast<int>(i + 1)];
	}

	// Divide by the number of vertices to get the average x-coordinate.
	originalCellPosX /= 4;
	// Divide by the number of vertices to get the average y-coordinate.
	originalCellPosY /= 4;

	float additionalOffsetX = -0.0015f;

	// Used to correct scaling (which happens around the origin).
	offX = ((1.f - CELL_SCALE_FACTOR) * originalCellPosX) - additionalOffsetX;
	offY = (1.f - CELL_SCALE_FACTOR) * originalCellPosY;
}

void manageTimedActions(CallbackData& callbackData, double& lastUpdateTime, float iterationUpdateFrequency)
{
	// Update elapsed time. 
	double currentTime = glfwGetTime();
	double elapsedTime = currentTime - lastUpdateTime;
	double updateInterval = 1.0 / (1.f * iterationUpdateFrequency);

	// An update every x seconds. Main section for cell state updates
	if (elapsedTime >= updateInterval)
	{
		// Read back from the compute shader.
		std::vector<int> dataFromComputeShader;
		dataFromComputeShader.clear();
		dataFromComputeShader.resize(static_cast<GLint64>(callbackData.gridController->getNumColumnCells() * callbackData.gridController->getNumRowCells()));

		// Dispatch compute shader and calculate states for all our cells.
		callbackData.computeShader->UseShader();
		callbackData.computeShader->setInt("numRowCells", callbackData.gridController->getNumRowCells());
		callbackData.computeShader->setInt("numColumnCells", callbackData.gridController->getNumColumnCells());
		glDispatchCompute(callbackData.gridController->getNumColumnCells(), callbackData.gridController->getNumRowCells(), 1);
		
		// Set up a fence to ensure that we read back only after the compute shader finishes computing.
		GLsync computeShaderSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glClientWaitSync(computeShaderSync, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(GL_TIMEOUT_IGNORED));
		glDeleteSync(computeShaderSync);
		
		// Make sure that we only access the shader storage buffer after all writes are completed.
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// Read back from the compute shader.
		glGetNamedBufferSubData(callbackData.nextStateSBO->getSSBO(), 0, dataFromComputeShader.size() * sizeof(int), dataFromComputeShader.data());

		// Update the texture array.
		glTextureSubImage2D(callbackData.gridController->getCellStatesTextureObject().getTextureObjectID(), 0, 0, 0, callbackData.gridController->getNumColumnCells(), callbackData.gridController->getNumRowCells(), GL_RED_INTEGER, GL_INT, dataFromComputeShader.data());
		// Make sure that we only access this texture after the texture fully updates.
		glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

		// Update our gridController instance.
		callbackData.gridController->setMasterCellLifeStatesArray(dataFromComputeShader);

		// Copy data from next state to current state for iterational updates.
		glCopyNamedBufferSubData(callbackData.nextStateSBO->getSSBO(), callbackData.currentStateSBO->getSSBO(), 0, 0, callbackData.gridController->getMasterCellLifeStatesArray().size() * sizeof(int));

		lastUpdateTime = currentTime;
	}
}

void setupCell(GridController* mainGridController, std::vector<Cell>& masterCellList, VBO& cellVBO, VAO& cellVAO, VBO& cellInstancesVBO, VBO& cellIVBO)
{
	const std::size_t indicesSize = mainGridController->getGridBoxQuadIndicesArray().size() * sizeof(unsigned int);
	const std::size_t verticesSize = mainGridController->getVerticesForOffsetsReference().size() * sizeof(float);

	// Set buffer storage for VBOs and EBO.
	cellVBO.setBufferStorage(nullptr, std::size_t(indicesSize + verticesSize), GL_DYNAMIC_STORAGE_BIT);
	// Use buffer sub-data to upload indices and vertices
	// Set the vertices data for one cell.
	cellVBO.setBufferSubData(0, verticesSize, mainGridController->getVerticesForOffsetsReference().data());
	// Set the indices data for one cell. 
	cellVBO.setBufferSubData(verticesSize, indicesSize, mainGridController->getGridBoxQuadIndicesArray().data());

	// Set up the buffer for instanced rendering location offsets.
	cellInstancesVBO.setBufferStorage((void*)(mainGridController->getInstanceOffsetsArray().data()), mainGridController->getInstanceOffsetsArray().size() * sizeof(glm::vec2), GL_DYNAMIC_STORAGE_BIT);

	// Set up the buffer for our cell indices to use with our compute shader.
	std::vector<glm::ivec2> masterCellIndices;
	for (Cell& cell : masterCellList)
	{
		masterCellIndices.push_back(cell.getCellCoords());
	}
	cellIVBO.setBufferStorage((void*)(masterCellIndices.data()), masterCellIndices.size() * sizeof(glm::ivec2), GL_DYNAMIC_STORAGE_BIT);

	// Enable the vertices attribute.
	glEnableVertexArrayAttrib(cellVAO.getVAO(), 0);
	glEnableVertexArrayAttrib(cellVAO.getVAO(), 1);
	glEnableVertexArrayAttrib(cellVAO.getVAO(), 2);

	// Bind cellVBO and instanceVBO to the cellVAO.
	glVertexArrayVertexBuffer(cellVAO.getVAO(), 0, cellVBO.getVBO(), 0, 3 * sizeof(float));
	glVertexArrayVertexBuffer(cellVAO.getVAO(), 1, cellInstancesVBO.getVBO(), 0, 2 * sizeof(float));
	glVertexArrayElementBuffer(cellVAO.getVAO(), cellVBO.getVBO());
	glVertexArrayVertexBuffer(cellVAO.getVAO(), 2, cellIVBO.getVBO(), 0, sizeof(glm::ivec2));


	// Specify the attribute format.
	// For the vertex positions.
	glVertexArrayAttribFormat(cellVAO.getVAO(), 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(cellVAO.getVAO(), 1, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribIFormat(cellVAO.getVAO(), 2, 2, GL_INT, 0);
	// For the instance position offsets.


	// Specify which VBO to get the attributes from.
	glVertexArrayAttribBinding(cellVAO.getVAO(), 0, 0);
	glVertexArrayAttribBinding(cellVAO.getVAO(), 1, 1);
	glVertexArrayAttribBinding(cellVAO.getVAO(), 2, 2);
	
	// Specify the attribute divisor for our vertex attributes.
	// For instanced rendering.
	glVertexArrayBindingDivisor(cellVAO.getVAO(), 1, 1);
	// For the cell indices.
	glVertexArrayBindingDivisor(cellVAO.getVAO(), 2, 1);
	
}

void renderGrid(Shader& mainGridShader, glm::mat4& scaleMatrix, VAO& gridVAO, GridController* mainGridController)
{
	// Bind shader.
	mainGridShader.UseShader();
	// Set uniforms.
	scaleMatrix = glm::mat4(1.f);
	mainGridShader.setMat4("scaleMatrix", scaleMatrix);
	// Bind VAO.
	gridVAO.bind();
	unsigned int totalLinesToDraw = (2 * (mainGridController->getNumRowCells() - 1)) + (2 * (mainGridController->getNumColumnCells() - 1));
	// Draw grid.
	glLineWidth(1.05f);
	glDrawArrays(GL_LINES, 0, totalLinesToDraw);
}

void renderGridBox(Shader& gridBoxShader, VAO& gridBoxVAO, GridController* mainGridController)
{
	// Bind shader.
	gridBoxShader.UseShader();
	// Bind VAO.
	gridBoxVAO.bind();
	const std::size_t indicesOffset = mainGridController->getGridBoxVerticesArray().size() * sizeof(float);
	// Draw grid-box.
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)indicesOffset);
}

//int setupTextRenderingWithFreeType(FT_Library& freeTypeMainObject, FT_Face& fontFaceParchment, std::map<GLchar, Character>& masterMap)
//{
//	if (FT_Init_FreeType(&freeTypeMainObject))
//	{
//		std::cout << "ERROR: FreeType Initialization!" << std::endl;
//		return -1;
//	}
//
//	// Load a font with FreeType.
//
//	if (FT_New_Face(freeTypeMainObject, "dep/fonts/PARCHM.ttf", 0, &fontFaceParchment))
//	{
//		std::cout << "ERROR: FREETYPE: Failed to load font!" << std::endl;
//		return -1;
//	}
//
//	// Set font pixel size.
//	FT_Set_Pixel_Sizes(fontFaceParchment, 0, 48);
//
//	// Disable byte-alignment restriction.
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//	// Store 128 chars.
//	for (unsigned char c = 0; c < 128; c++)
//	{
//		// Load character glyph.
//		if (FT_Load_Char(fontFaceParchment, c, FT_LOAD_RENDER))
//		{
//			std::cout << "ERROR: FREETYPE: Glyph load failed!" << std::endl;
//			continue;
//		}
//
//		// Generate a texture and set parameters.
//		TextureObject fontTexture;
//		glTextureStorage2D(fontTexture.getTextureObjectID(), 0, GL_RED, fontFaceParchment->glyph->bitmap.width, fontFaceParchment->glyph->bitmap.rows);
//		glTextureSubImage2D(fontTexture.getTextureObjectID(), 0, 0, 0, fontFaceParchment->glyph->bitmap.width, fontFaceParchment->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, fontFaceParchment->glyph->bitmap.buffer);
//		fontTexture.setParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, true);
//
//		// Store instance in character map.
//		Character tempChar =
//		{
//			// Texture ID.
//			fontTexture.getTextureObjectID(),
//			// Glyph size.
//			glm::ivec2(fontFaceParchment->glyph->bitmap.width, fontFaceParchment->glyph->bitmap.rows),
//			// Glyph bearings.
//			glm::ivec2(fontFaceParchment->glyph->bitmap_left, fontFaceParchment->glyph->bitmap_top),
//			// Offset for advancement.
//			fontFaceParchment->glyph->advance.x 
//		};
//		masterMap.insert(std::pair<GLchar, Character>(c, tempChar));
//	}
//	// Clean up FreeType memory.
//	FT_Done_Face(fontFaceParchment);
//	FT_Done_FreeType(freeTypeMainObject);
//	return {};
//}

void initialize_glfw_mainWindow()
{
	// Init GLFW.
	glfwInit();

	// Set window hints.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Debug output for error checking.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
}

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	//CallbackData* callbackData = static_cast<CallbackData*>(glfwGetWindowUserPointer(window));

	//// Update the window resolutions.
	//callbackData->gridController->setWindowDimensions(width, height);
	//// Re-compute gridbox dimensions.
	//callbackData->gridController->computeGridBoxDimensions(0.f);
	//// Calculate cells per dimension and cell-size.
	//callbackData->gridController->calculateCellsPerRowColumnAndCellSize();
	//// Re-compute excess width for gridbox centering, and offsets for gridbox and mouse actions.
	//callbackData->gridController->setExcessWidthNDCAndPixels();
	//callbackData->gridController->setOffsetsForCenteredGridBoxAndMouseActions();
	//// Re-init the gridbox vertices to use for instanced rendering.
	//callbackData->gridController->computeGridboxVertices();
	//// (Re)Re-compute gridbox dimensions post gridbox squaring and grid-centering.
	//callbackData->gridController->computeGridBoxDimensions(callbackData->gridController->getExcessWidthForCenteringPixels());
	//// Re-compute the grid-points and offsets for instanced rendering.
	//callbackData->gridController->generateGridPoints();
	//callbackData->gridController->generateInstancedOffsetsFromGrid();
	//// TBD Update buffer data for gridbox, gridlines and offsets.



}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	CallbackData* callbackData = static_cast<CallbackData*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		callbackData->isIterating = !callbackData->isIterating;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	CallbackData* callbackData = static_cast<CallbackData*>(glfwGetWindowUserPointer(window));
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		bool retFlag = false;
		callbackData->gridController->activateCell(*window, retFlag, callbackData);
	}
}


