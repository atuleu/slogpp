#include "Display.hpp"

#include <exception>
#include <stdexcept>
#include <utility>

#include <GLFW/glfw3.h>

Display::Ptr Display::Create() {
	return std::unique_ptr<Display>(new Display());
}

#define throw_glfw_error(ctx)                                                  \
	do {                                                                       \
		const char *glfwErrorDescription;                                      \
		glfwGetError(&glfwErrorDescription);                                   \
		throw std::runtime_error(std::string(ctx) + ": " +                     \
		                         glfwErrorDescription);                        \
	} while (0)

Display::Display() : d_window(NULL, [](GLFWwindow *) {}) {
	if (!glfwInit()) {
		throw_glfw_error("could not initialize GLFW");
	}

	try {
		d_window = OpenWindow();
	} catch (const std::exception &) {
		glfwTerminate();
		throw;
	}

	try {
		InitOpenGL();
	} catch (const std::exception &) {
		d_window.reset();
		glfwTerminate();
		throw;
	}
}

void Display::InitOpenGL() {
	glfwMakeContextCurrent(d_window.get());
	const GLenum err = glewInit();
	if (err != GLEW_OK) {
		throw std::runtime_error(
		    "Could not initialize GLEW: " +
		    std::string((const char *)glewGetErrorString(err)));
	}

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffer(2, d_pbos);
	for (int i = 0; i < 2; i++) {
	}
}

void Display::Draw() {}

Display::WindowPtr Display::OpenWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
	               GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	auto window = glfwCreateWindow(1920, 1080, "apollo", NULL, NULL);
	if (!window) {
		throw_glfw_error("could not open window");
	}

	return WindowPtr(window, glfwDestroyWindow);
}
