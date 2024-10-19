#pragma once

#include <array>
#include <vector>
#include <fstream>

//The VulkanGraphicsPipeline is a standalone class, it only needs the Vulkan headers
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>




/*--------------------------------------------------------------------------------
This class is used to abstract pipeline creation away from the main renderer
file, since, as the renderer grows, the more pipeline and shader configurations
it's going to need
---------------------------------------------------------------------------------*/
class VulkanGraphicsPipeline
{
private:

	/*------------------------------------
	Macros for vulkan shader filepaths
	-------------------------------------*/

	#define SIMPLE_GEOMETRY_VERTEX_SHADER		"VulkanShaders/SimpleGeometry.vert.spv"
	#define SIMPLE_GEOMETRY_FRAGMENT_SHADER		"VulkanShaders/SimpleGeometry.frag.spv"

public:

	//Uses a manual cleanup function instead of the destructor since it needs the device
	void Cleanup(const VkDevice& device);

	//This is called at the end of each pipeline init function to actually build the pipeline
	void BuildPipeline(const VkDevice& device);

	/*---------------------------------------------------------------------
	In the public section, all functions that are called for primary
	initialization of the different kinds of pipelines for the Blitzen
	engine are declared
	-----------------------------------------------------------------------*/

	//Creates a very simple pipeline used only to create simple geometry
	void InitBasicGeometryPipeline(const VkDevice& device, VkFormat* pColorAttachmentFormats);

private:

	/*------------------------------------------------------------------------
	In the private section, all functions that are used as helpers 
	for primary pipeline creation functions are declared. These functions
	usually control different setups for some pipeline configuration structs
	--------------------------------------------------------------------------*/

	//Reads a shader file in byte format so that it can be used to create a shader module 
	void ReadShaderFile(const char* filePath, std::vector<char>& code);

	//Initializes an array of shader modules using code from the simple geometry shaders
	void SimpleGeometryShaderStagesInit(std::array<VkShaderModule, 2>& shaderModules, 
		const VkDevice& device);



public:

	/*----------------------------------------------------------------------------------
	All the functions in this class are public so that the primary Vulkan renderer
	class can have easy access to them. This will probably be fine since each instance
	of this class will be a private variable in the VulkanRenderer class and on its own
	it's useless
	------------------------------------------------------------------------------------*/

	VkPipeline graphicsPipeline{ VK_NULL_HANDLE };
	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

	VkPipelineVertexInputStateCreateInfo vertexInput{};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

	VkPipelineTessellationStateCreateInfo tessellation{};

	VkPipelineViewportStateCreateInfo viewport{};

	VkPipelineRasterizationStateCreateInfo rasterization{};

	VkPipelineMultisampleStateCreateInfo multisampling{};

	VkPipelineDepthStencilStateCreateInfo depthStencil{};

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo colorBlending{};

	VkPipelineDynamicStateCreateInfo dynamicState{};

	VkPipelineRenderingCreateInfo renderingInfo{};
	VkFormat colorAttachmentFormat;
	VkFormat depthAttachmentFormat{ VK_FORMAT_UNDEFINED };
	VkFormat stencilAttachmentFormat{ VK_FORMAT_UNDEFINED };
};