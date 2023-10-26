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
constexpr int INIT_COLUMN_CELLS = 5;
// Used to determine the initial size of the grid-box relative to the entire window. 
constexpr float GRIDBOX_SIZE_SPAN = 0.85f;
// Scale factor used to contain the cell within the grid (instead of on it).
constexpr float CELL_SCALE_FACTOR = 1.f;

// Struct for pointers to objects needed throughout run-time. (Helps avoid globals.)
struct CallbackData
{
	GridController* gridController;
};

// Function prototypes.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void initialize_glfw_mainWindow();
int main();
void computeTranslationAndVisualOffset(float& originalCellPosX, std::vector<float>& cellVertices, float& originalCellPosY, float& offX, float& offY);
void manageTimedActions(double& lastUpdateTime, std::vector<Cell>& masterCellList);
void setupCell(GridController* mainGridController, std::vector<Cell>& masterCellList, VBO& cellVBO, VAO& cellVAO, VBO& cellInstancesVBO);
void renderGrid(Shader& mainGridShader, glm::mat4& scaleMatrix, VAO& gridVAO, GridController* mainGridController);
void renderGridBox(Shader& gridBoxShader, VAO& gridBoxVAO, GridController* mainGridController);
int setupTextRenderingWithFreeType(FT_Library& freeTypeMainObject, FT_Face& fontFaceParchment, std::map<GLchar, Character>& masterMap);
void processInput(GLFWwindow* window);

int main()
{
	srand(139583);
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
	glfwSetMouseButtonCallback(mainWindow, mouse_button_callback);

	// Initialize GLAD.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR: GLAD init failed!" << std::endl;
		return -1;
	}

	// Opengl options.
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);

	/*---- TEXT-RENDERING SETUP ----*/
	// Initialize FreeType (to help use our font).
	FT_Library freeTypeMainObject;
	FT_Face fontFaceParchment;
	std::map<GLchar, Character> masterCharactersMap;

	setupTextRenderingWithFreeType(freeTypeMainObject, fontFaceParchment, masterCharactersMap);

	/*---- SHADER INITIALIZATION ----*/
	// GridBox Shader
	Shader gridBoxShader("src/shaders/gridBox.vert", "src/shaders/gridBox.frag");
	gridBoxShader.retrieveUniforms();
	// Cell Shader
	Shader mainCellShader("src/shaders/cell.vert", "src/shaders/cell.frag");
	mainCellShader.retrieveUniforms();
	// Grid Shader
	Shader mainGridShader("src/shaders/grid.vert", "src/shaders/grid.frag");
	mainGridShader.retrieveUniforms();

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

	/*---- CELLS SETUP ----*/
	VAO cellVAO;
	VBO cellVBO, cellInstancesVBO;

	setupCell(mainGridController, masterCellList, cellVBO, cellVAO, cellInstancesVBO);

	// Set callback data to obtain pointers from our window.
	CallbackData* mainData = new CallbackData;
	mainData->gridController = mainGridController;
	// Set the pointers.
	glfwSetWindowUserPointer(mainWindow, mainData);

	// Calculate the final offset simply for visual reasons to make the cell fit better within its grid points.
	std::vector<float> cellVertices = masterCellList[0].getCellQuadVertices();  
	float originalCellPosX = 0.0f;
	float originalCellPosY = 0.0f;
	float offX = 0.f, offY = 0.f;
	computeTranslationAndVisualOffset(originalCellPosX, cellVertices, originalCellPosY, offX, offY);

	/*---- RENDER LOOP ----*/
	while (!glfwWindowShouldClose(mainWindow))
	{
		processInput(mainWindow);

		// Set the color to clear the screen.
		glClearColor(0.33f, 0.33f, 0.33f, 1.f);
		// Clear the color and depth buffers.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the grid-box that contains the base grid.
		renderGridBox(gridBoxShader, gridBoxVAO, mainGridController);

		// Handle cell state (or any other time-based action) execution. (Important!!)
		manageTimedActions(lastUpdateTime, masterCellList);

		// Bind VAO.
		cellVAO.bind();
		// Bind Shader.
		mainCellShader.UseShader();
		// Set uniforms.
		glm::mat4 scaleMatrix = glm::mat4(1.f);
		scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(CELL_SCALE_FACTOR, CELL_SCALE_FACTOR, 0.f));
		mainCellShader.setMat4("scaleMatrix", scaleMatrix);
		mainCellShader.setVec2("translationOffset", glm::vec2(offX, offY));
		mainCellShader.setBool("isCellAlive",masterCellList[0].getCellLifeStatus());
		// Draw cells.
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(masterCellList[0].getCellQuadVertices().size() * sizeof(float)), (mainGridController->getNumRowCells() * mainGridController->getNumColumnCells()));
		// Render the base grid.
		renderGrid(mainGridShader, scaleMatrix, gridVAO, mainGridController);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		/*---- ----*/
	}
	
	// Memory management.
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

void manageTimedActions(double& lastUpdateTime, std::vector<Cell>& masterCellList)
{
	// Update elapsed time. 
	double currentTime = glfwGetTime();
	double elapsedTime = currentTime - lastUpdateTime;
	double updateInterval = 1.0 / (1.f * 1.f);

	// An update every x seconds. Main section for cell state updates
	if (elapsedTime >= updateInterval)
	{
		int random = rand() % 10;
		std::cout << "Pre flip 0, 0: " << " -- " << masterCellList[0].getCellLifeStatus() << "\n";
		masterCellList[0].flipCellLifeStatus();
		std::cout << "Post flip 0, 0: " << " -- " << masterCellList[0].getCellLifeStatus() << "\n";
		lastUpdateTime = currentTime;
	}
}

void setupCell(GridController* mainGridController, std::vector<Cell>& masterCellList, VBO& cellVBO, VAO& cellVAO, VBO& cellInstancesVBO)
{
	const std::size_t indicesSize = mainGridController->getGridBoxQuadIndicesArray().size() * sizeof(unsigned int);
	const std::size_t verticesSize = masterCellList[0].getCellQuadVertices().size() * sizeof(float);


	// Set buffer storage for VBOs and EBO.
	cellVBO.setBufferStorage(nullptr, std::size_t(indicesSize + verticesSize), GL_DYNAMIC_STORAGE_BIT);
	// Use buffer sub-data to upload indices and vertices
	// Set the vertices data for one cell.
	cellVBO.setBufferSubData(0, verticesSize, masterCellList[0].getCellQuadVertices().data());
	// Set the indices data for one cell. 
	cellVBO.setBufferSubData(verticesSize, indicesSize, mainGridController->getGridBoxQuadIndicesArray().data());

	cellInstancesVBO.setBufferStorage((void*)(mainGridController->getInstanceOffsetsArray().data()), mainGridController->getInstanceOffsetsArray().size() * sizeof(glm::vec2), GL_DYNAMIC_STORAGE_BIT);


	// Bind cellVBO, cellEBO and instanceVBO to the cellVAO.
	glVertexArrayVertexBuffer(cellVAO.getVAO(), 0, cellVBO.getVBO(), 0, 3 * sizeof(float));
	glVertexArrayVertexBuffer(cellVAO.getVAO(), 1, cellInstancesVBO.getVBO(), 0, 2 * sizeof(float));
	glVertexArrayElementBuffer(cellVAO.getVAO(), cellVBO.getVBO());

	// Enable the vertices attribute.
	glEnableVertexArrayAttrib(cellVAO.getVAO(), 0);
	glEnableVertexArrayAttrib(cellVAO.getVAO(), 1);
	// Specify the attribute format.
	// For the vertex positions.
	glVertexArrayAttribFormat(cellVAO.getVAO(), 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(cellVAO.getVAO(), 1, 2, GL_FLOAT, GL_FALSE, 0);
	// For the instance position offsets.


	// Specify which VBO to get the attributes from.
	glVertexArrayAttribBinding(cellVAO.getVAO(), 0, 0);
	glVertexArrayAttribBinding(cellVAO.getVAO(), 1, 1);

	// Specify the attribute divisor.
	glVertexArrayBindingDivisor(cellVAO.getVAO(), 1, 1);
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
	glLineWidth(1.25f);
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

int setupTextRenderingWithFreeType(FT_Library& freeTypeMainObject, FT_Face& fontFaceParchment, std::map<GLchar, Character>& masterMap)
{
	if (FT_Init_FreeType(&freeTypeMainObject))
	{
		std::cout << "ERROR: FreeType Initialization!" << std::endl;
		return -1;
	}

	// Load a font with FreeType.

	if (FT_New_Face(freeTypeMainObject, "dep/fonts/PARCHM.ttf", 0, &fontFaceParchment))
	{
		std::cout << "ERROR: FREETYPE: Failed to load font!" << std::endl;
		return -1;
	}

	// Set font pixel size.
	FT_Set_Pixel_Sizes(fontFaceParchment, 0, 48);

	// Disable byte-alignment restriction.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Store 128 chars.
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph.
		if (FT_Load_Char(fontFaceParchment, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR: FREETYPE: Glyph load failed!" << std::endl;
			continue;
		}

		// Generate a texture and set parameters.
		TextureObject fontTexture;
		glTextureStorage2D(fontTexture.getTextureObjectID(), 0, GL_RED, fontFaceParchment->glyph->bitmap.width, fontFaceParchment->glyph->bitmap.rows);
		glTextureSubImage2D(fontTexture.getTextureObjectID(), 0, 0, 0, fontFaceParchment->glyph->bitmap.width, fontFaceParchment->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, fontFaceParchment->glyph->bitmap.buffer);
		fontTexture.setParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, true);

		// Store instance in character map.
		Character tempChar =
		{
			// Texture ID.
			fontTexture.getTextureObjectID(),
			// Glyph size.
			glm::ivec2(fontFaceParchment->glyph->bitmap.width, fontFaceParchment->glyph->bitmap.rows),
			// Glyph bearings.
			glm::ivec2(fontFaceParchment->glyph->bitmap_left, fontFaceParchment->glyph->bitmap_top),
			// Offset for advancement.
			fontFaceParchment->glyph->advance.x 
		};
		masterMap.insert(std::pair<GLchar, Character>(c, tempChar));
	}
	// Clean up FreeType memory.
	FT_Done_Face(fontFaceParchment);
	FT_Done_FreeType(freeTypeMainObject);
	return {};
}

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

	// Main window hints.
	//glfwWindowHint(GLFW_FLOATING, GL_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		
	}
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{


	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{

	
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{

	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{

	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{

	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{

	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	CallbackData* cbData = static_cast<CallbackData*>(glfwGetWindowUserPointer(window));

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// Activate / deactivate a cell.
		int key = GLFW_MOUSE_BUTTON_LEFT;
		cbData->gridController->processInput(&key, *window);
	}
}


