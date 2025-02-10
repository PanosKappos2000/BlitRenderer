#include "glfwInputs.h"

void glfwInputs::LoadRenderingWindowInputs(GLFWwindow* pWindow)
{
	glfwSetWindowCloseCallback(pWindow, glfwInputs::WindowCloseCallback);
}

void glfwInputs::WindowCloseCallback(GLFWwindow* pWindow)
{
	WindowData* windowData = reinterpret_cast<WindowData*>
		(glfwGetWindowUserPointer(pWindow));

	windowData->bWindowShouldEndApplication = true;
}