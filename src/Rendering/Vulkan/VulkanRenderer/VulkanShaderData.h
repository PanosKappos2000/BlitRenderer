#pragma once

/*-----------------------------------------------------------------------
This header file includes structs used for shader data abstractions
for the Vulkan implementation of the renderer. It is used by game
objects to communicate their data to the VulkanRenderer and by the 
renderer to pass data to the shaders
------------------------------------------------------------------------*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//Includes the math library to manipulate geometry in the shader
#include <glm/glm.hpp>

//Include the memory allocator library to allocate buffers
#include <vk_mem_alloc.h>

#include <vector>
#include <array>



namespace VulkanShaderData
{
	struct AllocatedBuffer
	{
		VkBuffer buffer{ VK_NULL_HANDLE };
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo{};
	};

	struct Vertex
	{
		glm::vec3 position;
		float uv_x;
		glm::vec3 normal;
		float uv_y;
		glm::vec4 color;
	};

	struct GPUMeshBuffers
	{
		AllocatedBuffer vertexBuffer;
		AllocatedBuffer indexBuffer;
		VkDeviceAddress vertexBufferAddress;
	};

	struct GPUPushConstants
	{
		glm::mat4 modelMatrix;
		VkDeviceAddress vertexBuffer;
	};

}