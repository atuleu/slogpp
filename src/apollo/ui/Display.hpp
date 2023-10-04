#pragma once

#include <memory>

// Note: glew need to be included before OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Display {
public:
	typedef std::unique_ptr<Display> Ptr;
	Ptr Create();

private:
	typedef std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)> WindowPtr;

	static WindowPtr OpenWindow();

	Display();

	void Draw();
	void InitOpenGL();

	WindowPtr d_window;
	GLuint d_pbos;
};
