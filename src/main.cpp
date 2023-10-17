
#include <GLAD/glad.h>
#include <GLFWLib/glfw3.h>

#include <iostream>

constexpr unsigned int WIN_HEIGHT = 600;
constexpr unsigned int WIN_WIDTH = 800;

/*Declarations*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	/*Initialization*/
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

	/*Rendering loop*/
	while (!glfwWindowShouldClose(mainWindow))
	{
		processInput(mainWindow);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
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
