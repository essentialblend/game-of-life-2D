#pragma warning( disable : 4100 ) 

#include <GLAD/glad.h>
#include <GLFWLib/glfw3.h>

#include <iostream>

#include <../gameOfLife2D/src/headers/VAO.h>
#include <../gameOfLife2D/src/headers/VBO.h>

#include <../gameOfLife2D/src/headers/shader.h>

constexpr unsigned int WIN_HEIGHT = 600;
constexpr unsigned int WIN_WIDTH = 800;

/*Declarations*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	/*----Initialization----*/
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	/*Line points*/
	float linePoints[] = {
		-1.f, 0.f, 0.f,
		+1.f, 0.f, 0.f,
	};

	/*Line VAO/VBO*/
	VAO lineVAO;
	VBO lineVBO;

	lineVAO.bind();
	lineVBO.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(linePoints), linePoints, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*LineShader*/
	Shader lineShader("src/shaders/line.vert", "src/shaders/line.frag");
	






	/*Rendering loop*/
	while (!glfwWindowShouldClose(mainWindow))
	{
		processInput(mainWindow);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		lineShader.UseShader();
		lineVAO.bind();
		glDrawArrays(GL_LINES, 0, 2);
		
		/*---- ----*/
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
		/*---- ----*/
	}
	
	glfwTerminate();
	return 0;
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
}
