#include "context.h"
#include <iostream>
#include "glew/GL/glew.h"
#include "glfw/glfw3.h"
using namespace std;

void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
{
	std::cerr << message << std::endl;
}

bool context::init(int width, int height, const char * title)
{
	glfwInit();
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewInit();
	cout << "OpenGL Version: " << (const char *)glGetString(GL_VERSION) << "\n";
	cout << "Renderer: " << (const char *)glGetString(GL_RENDERER) << "\n";
	cout << "Vendor: " << (const char *)glGetString(GL_VENDOR) << "\n";
	cout << "GLSL: " << (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
	//glClearColor(1.0f, 0.25f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
#endif
	return false; 
}

void context::tick()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void context::term()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	window = nullptr;
}

void context::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}
