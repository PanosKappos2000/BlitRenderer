#include "VulkanPipeline.h"
#include "VulkanShaderData.h"
#include "Rendering/Vulkan/SDKobjects/VulkanSDKobjects.h"

void VulkanGraphicsPipeline::InitBasicGeometryPipeline(const VkDevice& device, 
	VkFormat* pColorAttachmentFormats)
{
	/*-----------------------------------------------------------------
	This pipeline will use push constants to access the model matrix 
	and a gpu pointer to the vertex buffer
	-------------------------------------------------------------------*/
	VkPushConstantRange pushConstant{};
	VulkanSDKobjects::PushConstantRangeInit(pushConstant,
		static_cast<uint32_t>(sizeof(VulkanShaderData::GPUPushConstants)),
		VK_SHADER_STAGE_VERTEX_BIT);

	//Create the pipeline layout for this pipeline
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	VulkanSDKobjects::PipelineLayoutCreateInfoInit(pipelineLayoutInfo,
		nullptr, 0, &pushConstant, 1);
	vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

	//Gets the shader code, wraps it in shader modules and creates the shader stages
	std::array<VkShaderModule, 2> shaderModules{};
	SimpleGeometryShaderStagesInit(shaderModules, device);

	//Initializing the vertex input state, it will not be used with this pipeline
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	//Initializeing input assembly, with triangle topology and no primitive restart
	VulkanSDKobjects::PipelineInputAssemblyStateCreateInfoInit(inputAssembly,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	//Initializing the tesselation state, it will not be used with this pipeline
	tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

	//Simply states that the vieport state will have one viewport and one scissor
	VulkanSDKobjects::PipelineViewportStateCreateInfoInit(viewport);

	//The rasterization will not be doing backface culling
	VulkanSDKobjects::PipelineRasterizationCreateInfoSetCullMode(rasterization,
		VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	//The rasterization state will fill the triangles
	VulkanSDKobjects::PipelineRasterizationCreateInfoSetPolygonMode(rasterization,
		VK_POLYGON_MODE_FILL);

	VulkanSDKobjects::PipelineMultisampleStateCreateInfoInit(multisampling, VK_SAMPLE_COUNT_1_BIT);

	//We do not want a depth test for this pipeline
	VulkanSDKobjects::PipelineDepthStencilStateCreateInfoSetDepthTest(depthStencil);
	//We do not want a depth bounds test for this pipeline
	VulkanSDKobjects::PipelineDepthStencilStateCreateInfoSetDepthBoundsTest(depthStencil);
	//We do not want a stencil test for this pipeline
	VulkanSDKobjects::PipelineDepthStencilStateCreateInfoSetStencilTest(depthStencil);

	//We do not want color blending for this pipeline
	VulkanSDKobjects::PipelineColorBlendAttachmentStateInit(colorBlendAttachment);
	VulkanSDKobjects::PipelineColorBlendStateCreateInfoInit(colorBlending, &colorBlendAttachment, 1);

	//The viewport and the scissor will be parts of the dynamic state
	std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VulkanSDKobjects::PipelineDynamicStateCreateInfoInit(dynamicState, dynamicStates.data(), 2);

	//Creating the rendering info for dynamic rendering when we bind the pipeline
	VulkanSDKobjects::PipelineRenderingCreateInfoInit(renderingInfo, pColorAttachmentFormats,
		depthAttachmentFormat, stencilAttachmentFormat);
	colorAttachmentFormat = *pColorAttachmentFormats;

	//Create the pipeline
	BuildPipeline(device);

	//With the pipeline created, the shader modules are no longer needed
	vkDestroyShaderModule(device, shaderModules[0], nullptr);
	vkDestroyShaderModule(device, shaderModules[1], nullptr);
}







void VulkanGraphicsPipeline::BuildPipeline(const VkDevice& device)
{
	VkGraphicsPipelineCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.basePipelineHandle = nullptr;
	info.basePipelineIndex = 0;

	info.layout = pipelineLayout;

	//The blitzen engine uses dynamic rendering for all pipelines for now
	info.pNext = &renderingInfo;
	info.renderPass = VK_NULL_HANDLE;

	info.stageCount = 2;
	info.pStages = shaderStages.data();

	info.pVertexInputState = &vertexInput;
	info.pInputAssemblyState = &inputAssembly;
	info.pTessellationState = &tessellation;
	info.pViewportState = &viewport;
	info.pRasterizationState = &rasterization;
	info.pMultisampleState = &multisampling;
	info.pDepthStencilState = &depthStencil;
	info.pColorBlendState = &colorBlending;
	info.pDynamicState = &dynamicState;

	vkCreateGraphicsPipelines(device, nullptr, 1, &info, nullptr, &graphicsPipeline);
}








void VulkanGraphicsPipeline::ReadShaderFile(const char* filepath,
	std::vector<char>& code)
{
	// open the file. With cursor at the end
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		__debugbreak();
	}


	size_t filesize = static_cast<size_t>(file.tellg());

	code.resize(filesize);

	// put file cursor at beginning
	file.seekg(0);

	// load the entire file into the buffer
	file.read(code.data(), filesize);

	file.close();
}


void VulkanGraphicsPipeline::SimpleGeometryShaderStagesInit(
	std::array<VkShaderModule, 2>& shaderModules, const VkDevice& device)
{
	std::vector<char> vertCode;
	ReadShaderFile(SIMPLE_GEOMETRY_VERTEX_SHADER, vertCode);
	VkShaderModuleCreateInfo vertShaderModule{};
	VulkanSDKobjects::ShaderModuleCreateInfoInit(vertShaderModule, vertCode);
	vkCreateShaderModule(device, &vertShaderModule, nullptr, &shaderModules[0]);
	VulkanSDKobjects::PipelineShaderStageInit(shaderStages[0], shaderModules[0],
		VK_SHADER_STAGE_VERTEX_BIT);

	std::vector<char> fragCode;
	ReadShaderFile(SIMPLE_GEOMETRY_FRAGMENT_SHADER, fragCode);
	VkShaderModuleCreateInfo fragShaderModule{};
	VulkanSDKobjects::ShaderModuleCreateInfoInit(fragShaderModule, fragCode);
	vkCreateShaderModule(device, &fragShaderModule, nullptr, &shaderModules[1]);
	VulkanSDKobjects::PipelineShaderStageInit(shaderStages[1], shaderModules[1],
		VK_SHADER_STAGE_FRAGMENT_BIT);
}





void VulkanGraphicsPipeline::Cleanup(const VkDevice& device)
{
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
}