#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct WindowData
{
	GLFWwindow* pWindow{ nullptr };
	const int width = 720;
	const int height = 560;
	const char* title = "Blitzen Engine";
	bool bWindowShouldStopRendering = false;
	bool bWindowShouldEndApplication = false;
};

namespace glfwInputs
{
	void LoadRenderingWindowInputs(GLFWwindow* pWindow);

	void WindowCloseCallback(GLFWwindow* pWindow);
}