#pragma once

#include <memory>

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

	WindowPtr d_window;
};
