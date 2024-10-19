#pragma once

#include "Rendering/Vulkan/VulkanRenderer/VulkanShaderData.h"

namespace BlitzenEngine
{
	struct MeshData
	{
		std::vector<VulkanShaderData::Vertex> vertices;

		std::vector<uint32_t> indices;
	};

	struct VulkanMesh
	{
		std::vector<VulkanShaderData::Vertex> vertices;

		std::vector<uint32_t> indices;

		glm::mat4 modelMatrix;
	};
}