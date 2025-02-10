#pragma once

#include <vector>
#include <array>

//The functions in this header file are standalone, they only need the Vulkan SDK headers
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//The functions in this namespace are used to initalize key vulkan objects
namespace VulkanSDKobjects
{
	//Create a VkCommandPoolCreateInfo sturct for command pool creation
	void CommandPoolCreateInfoInit( VkCommandPoolCreateInfo& commandPoolInfo,
		uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);

	//Asks the physical device for the index of queue family that supports graphics commands
	uint32_t GetDeviceGraphicsQueueFamilyIndex(
		const VkPhysicalDevice& rPhysicalDeviceHandle);

	uint32_t GetDevicePresentQueueFamilyIndex(
		VkPhysicalDevice& rPhysicalDeviceHandle,
		const VkSurfaceKHR& rSurface);

	//Creates a VkCommandBufferAllocateInfo struct to create one or more command buffers
	void CommandBufferAllocInfoInit(VkCommandBufferAllocateInfo& commandBufferInfo, 
		const VkCommandPool& rCommandPool, VkCommandBufferLevel cmdbLevel, 
		uint32_t commandBufferCount = 1);

	//Creates a command buffer begin info struct to start command buffer recording
	void CommandBufferBeginInfoInit(VkCommandBufferBeginInfo& commandBufferBegin,
		VkCommandBufferUsageFlags cmdbUsageFlags = 0);

	//Creates a VkCommandBufferSubmitInfo which will the command buffer to be sumbitted to a queue
	void CommandBufferSubmitInfoInit(VkCommandBufferSubmitInfo& submitInfo, 
		VkCommandBuffer& commandBuffer);

	//Initializes a VkSubmitInfo2 struct to submit a command buffer to a queue
	void SubmitInfo2Init(VkSubmitInfo2& queueSubmitInfo,
		VkSemaphoreSubmitInfo* waitSemaphoreInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
		VkCommandBufferSubmitInfo* commandBufferSubmit);

	//Present a swapchain to the screen
	void PresentInfoKHRInit(VkPresentInfoKHR& presentInfo, VkSwapchainKHR& swapchain,
		uint32_t* pImageIndex, VkSemaphore* pWaitSemaphore);



	//Initializes a VkPipelineLayoutCreateInfo struct for pipeline layout creation
	void PipelineLayoutCreateInfoInit(VkPipelineLayoutCreateInfo& info,
		VkDescriptorSetLayout* descriptorSetLayouts, uint32_t descriptorSetLayoutCount, 
		VkPushConstantRange* pPushConstants = nullptr, uint32_t pushConstantCount = 0);

	//Initializes a VkDescriptorSetLayoutBinding struct for descriptor set layout creation
	void DescriptorSetLayoutBindingInit(VkDescriptorSetLayoutBinding& layoutBinding, 
		uint32_t binding, VkDescriptorType type, VkShaderStageFlags shaderStage,
		uint32_t descriptorCount = 1 );

	//Initializes a VkDescriptorSetLayoutCreateInfo struct for descriptor set layout creation
	void DescriptorSetLayoutCreateInfoInit(VkDescriptorSetLayoutCreateInfo& layout, 
		uint32_t bindingCount,VkDescriptorSetLayoutBinding* bindings,
		VkDescriptorSetLayoutCreateFlags flags = 0);

	//Initializes a VkDescriptorPoolCreateInfo struct for descriptor pool creation
	void DescriptorPoolCreateInfoInit(VkDescriptorPoolCreateInfo& poolInfo, 
		uint32_t maxSets,uint32_t poolSizeCount, VkDescriptorPoolSize* pPoolSizes,
		VkDescriptorPoolCreateFlags flags = 0 );

	//Initializes a VkDescriptorSetAllocateInfo struct for descriptor set allocation
	void DescriptorSetAllocateInfoInit(VkDescriptorSetAllocateInfo& allocInfo,
		const VkDescriptorPool& descriptorPool,
		VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount = 1);

	//Initializes a VkWriteDescriptorSet for images, to pass an image to a descriptor set
	void WriteDescriptorSetImageInit(VkWriteDescriptorSet& write,
		const VkDescriptorSet& descriptorSet, VkDescriptorType descriptorType,
		VkDescriptorImageInfo* imageInfo, uint32_t binding, uint32_t descriptorCount = 1);

	//Initializes a VkPushConstantRange used for pipeline layyout creation
	void PushConstantRangeInit(VkPushConstantRange& pushConstant, uint32_t size,
		VkShaderStageFlags shaderStage, uint32_t offset = 0);



	//Wraps an array of shader code into a shader module object
	void ShaderModuleCreateInfoInit(VkShaderModuleCreateInfo& info, 
		std::vector<char>& rShaderCode);

	//Assigns a shader module to a shader stage
	void PipelineShaderStageInit(VkPipelineShaderStageCreateInfo& shaderStage,
		const VkShaderModule& shaderModule, VkShaderStageFlagBits stage);

	//Initializes a VkPipelineInputAssemblyStateCreateInfo struct for graphics pipeline creation
	void PipelineInputAssemblyStateCreateInfoInit(
		VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
		VkPrimitiveTopology primitiveTopology, VkBool32 bPrimitiveRestart = VK_FALSE );

	//Creates a single vertex input binding description
	void CreateVertexInputBindingDescription(
		VkVertexInputBindingDescription& inputBinding,
		uint32_t binding, uint32_t stride, VkVertexInputRate rate);

	void CreateVertexInputAttributeDescription(
		VkVertexInputAttributeDescription& inputAttribute, uint32_t location,
		uint32_t binding, VkFormat format, uint32_t offsetof);

	//Creates the vertex input state, essential if we're vertex buffers to the vertex shader
	void CreatePipelineVertexInputState(
		VkPipelineVertexInputStateCreateInfo& vertexInput,
		VkVertexInputBindingDescription* pInputBindings,
		uint32_t inputBindingCount,
		VkVertexInputAttributeDescription* pInputAttributes,
		uint32_t inputAttributeCount);

	//Initalizes a VkPipelineViewportStateCreateInfo struct for graphics pipeline creation
	void PipelineViewportStateCreateInfoInit(VkPipelineViewportStateCreateInfo& viewport, 
		uint32_t viewportCount = 1, uint32_t scissorCount = 1);

	/*--------------------------------------------------------------------------------------
	The 3 functions below are responsible for setting the VkPipelineRasterizationCreateInfo
	that is used for pipeline creation. The PipelineRasterizationCreateInfoSetCullMode
	function also sets the sType, so it NEEDS to be called when creating a pipeline
	--------------------------------------------------------------------------------------*/
	//Sets the polygon mode for a VkPipelineRasterizationCreateInfo struct
	void PipelineRasterizationCreateInfoSetPolygonMode(
		VkPipelineRasterizationStateCreateInfo& info,
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL, float lineWidth = 1.0f);
	//Sets the cull mode for a VkPipelineRasterizationCreateInfo struct
	void PipelineRasterizationCreateInfoSetCullMode(
		VkPipelineRasterizationStateCreateInfo& info,VkCullModeFlags cullMode, 
		VkFrontFace frontFace);
	//Sets the depth bias for a VkPipelineRasterizationCreateInfo struct
	void PipelineRasterizationCreateInfoSetDepthBias(
		VkPipelineRasterizationStateCreateInfo& info, VkBool32 bDepthClampEnable,
		float depthBiasClamp, VkBool32 bDepthBiasEnable, float depthBiasConstantFactor,
		float depthBiasSlopeFactor);

	//Initializes a VkPipelineMultisampleStateCreateInfo struct for graphics pipeline creation
	void PipelineMultisampleStateCreateInfoInit(
		VkPipelineMultisampleStateCreateInfo& multisampling,
		VkSampleCountFlagBits rasterizationSamples, VkBool32 bSampleShading = VK_FALSE,
		float minSampleShading = 1.0f, VkSampleMask* pSampleMask = nullptr,
		VkBool32 bAlphaToCoverage = VK_FALSE, VkBool32 bAlphaToOne = VK_FALSE);

	/*------------------------------------------------------------------------------------------------
	The 3 functions below are used to fully initialize a VkPipelineDepthStencilStateCreateInfo
	struct for graphics pipeline creation. The PipelineDepthStencilStateCreateInfoSetDepthTest
	function will also initialize the sType, so it should always be called (but it is best to call
	all the functions)
	---------------------------------------------------------------------------------------------------*/
	//Set depth test and depth write for a VkPipelineDepthStencilStateCreateInfo struct
	void PipelineDepthStencilStateCreateInfoSetDepthTest(
		VkPipelineDepthStencilStateCreateInfo& depthStencil,
		VkBool32 bDepthTest = VK_FALSE, VkBool32 bDepthWrite = VK_FALSE, 
		VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER);
	//Set depth bound test for a VkPipelineDepthStencilStateCreateInfo struct
	void PipelineDepthStencilStateCreateInfoSetDepthBoundsTest(
		VkPipelineDepthStencilStateCreateInfo& depthStencil, 
		VkBool32 bDepthBounds = VK_FALSE,  float minDepthBounds = 0.f, float maxDepthBounds = 1.f);
	//Set stencil test for a VkPipelineDepthStencilStateCreateInfo struct
	void PipelineDepthStencilStateCreateInfoSetStencilTest(
		VkPipelineDepthStencilStateCreateInfo& depthStencil, VkBool32 bStencilTest = VK_FALSE, 
		VkStencilOpState front = {}, VkStencilOpState back = {});

	//Creates a color blend attachment that the color blend state will reference
	void PipelineColorBlendAttachmentStateInit(
		VkPipelineColorBlendAttachmentState& colorBlendAttachment,
		VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VkBool32 bBlendEnalbe = VK_FALSE);

	//Sets up a color blend state for the graphics pipeline if desired
	void PipelineColorBlendStateCreateInfoInit(
		VkPipelineColorBlendStateCreateInfo& colorBlending,
		VkPipelineColorBlendAttachmentState* pColorBlendAttachments,
		uint32_t colorBlendAttachmentCount, VkBool32 bLogicOp = VK_FALSE);

	//Specifies which states of the pipeline should be configurable at draw time
	void PipelineDynamicStateCreateInfoInit(VkPipelineDynamicStateCreateInfo&
		dynamicState, VkDynamicState* pDynamicStates, uint32_t dynamicStateCount);

	//Initializes a VkPipelineRenderingCreateInfo for dynamic rendering graphics pipeline creation
	void PipelineRenderingCreateInfoInit(VkPipelineRenderingCreateInfo& renderingInfo,
		VkFormat* pColorAttachmentFormats, VkFormat& depthAttachmentFormat,
		VkFormat& stencilAttachmentFormat, uint32_t colorAttachmentCount = 1);



	//Initializes a VkImageCreateInfo struct for image view creation
	void ImageCreateInfoInit(VkImageCreateInfo& imageInfo, VkExtent3D& extent, 
		VkFormat& format, VkImageUsageFlags usageFlags);

	//Initializes a VkImageViewCreateInfo struct for image view creation
	void ImageViewCreateInfoInit(VkImageViewCreateInfo& imageViewInfo, 
		 VkImage& image, VkImageAspectFlags aspectMask, VkFormat& format);

	//Creates a VkImageSubresourceRange used to access aspects of an image
	void ImageSubresourceRangeInit(VkImageSubresourceRange& subresourceRange, 
		VkImageAspectFlags aspectMask, uint32_t levelCount =0,
		uint32_t baseMipLevel = VK_REMAINING_MIP_LEVELS);



	//Initializes a VkBufferCreateInfo struct for buffer allocation
	void BufferCreateInfoInit(VkBufferCreateInfo& info, VkDeviceSize allocSize, 
		VkBufferUsageFlags usage);

	//Initializes a VkBufferCopy struct for buffer copying
	void BufferCopyInit(VkBufferCopy& bufferCopy, VkDeviceSize size, 
		VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);



	//Creates a single VkSemaphoreSubmitInfo used to submit a wait or signal semaphore to a queue
	void SemaphoreSubmitInfoInit(VkSemaphoreSubmitInfo& semaphoreInfo,
		VkSemaphore& waitSemaphore, VkPipelineStageFlags2 stageMask);

	//Creates a single VkSemaphoreCreateInfo stuct
	void SemaphoreCreateInfoInit(VkSemaphoreCreateInfo& semaphoreInfo,
		VkSemaphoreCreateFlags flags = 0);

	//Creates a single VkFenceCreateInfo struct
	void FenceCreateInfoInit(VkFenceCreateInfo& fenceInfo,
		VkFenceCreateFlags flags = 0);




	/*------------------------------------------------------------------------------------------------
	Initializes a VkRenderingAttachmentInfo to pass to VkRenderingInfo so that vkCmdBeginRendering
	works like we want it to
	--------------------------------------------------------------------------------------------------*/
	void RenderingAttachmentInfoInit(VkRenderingAttachmentInfo& renderingAttachment, 
		VkImageView& imageView,VkImageLayout imageLayout, VkClearValue* pClearValue = nullptr);

	//Initializes a VkRenderingInfo struct to call vkCmdBeginRendering during the render loop
	void RenderingInfoInit(VkRenderingInfo& renderingInfo,VkRenderingAttachmentInfo* pColorAttachments, 
		VkExtent2D extent,VkRenderingAttachmentInfo* pDepthAttachment, 
		VkRenderingAttachmentInfo* pStencilAttachment,uint32_t layerCount = 1, 
		uint32_t colorAttachmentCount = 1, uint32_t viewMask = 0);
}