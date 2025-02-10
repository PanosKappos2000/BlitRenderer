#include <iostream>

#include "Rendering/Vulkan/VulkanRenderer/VulkanRenderer.h"


#include "Engine/GameObjects/Mesh.h"

int main(int argc, char* argv[] )
{
	std::cout << "Blitzen Boot" << '\n';

	BlitzenEngine::VulkanMesh mesh;

	mesh.vertices.resize(4);
	mesh.vertices[0].position = glm::vec3(-0.7f, 0.9f, 0.0f);
	mesh.vertices[1].position = glm::vec3(-0.7f, -0.9f, 0.0f);
	mesh.vertices[2].position = glm::vec3(0.7f, -0.9f, 0.0f);
	mesh.vertices[3].position = glm::vec3(0.7f, 0.9f, 0.0f);

	mesh.vertices[0].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	mesh.vertices[1].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	mesh.vertices[2].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	mesh.vertices[3].color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	mesh.indices.resize(6);
	mesh.indices[0] = 0;
	mesh.indices[1] = 1;
	mesh.indices[2] = 2;
	mesh.indices[3] = 2;
	mesh.indices[4] = 3;
	mesh.indices[5] = 0;

	VulkanRenderer vulkanRenderer(&mesh, 1);

	WindowData* pWindowData = &vulkanRenderer.windowData;

	glfwInputs::LoadRenderingWindowInputs(pWindowData->pWindow);

	while (!pWindowData->bWindowShouldEndApplication)
	{
		glfwPollEvents();
		vulkanRenderer.DrawFrame();
	}

	std::cout << "Blitzen End" << '\n';
	
	/* TODO: destroy all static and dynamic objects */
	/* TODO: destroy the window object */
}