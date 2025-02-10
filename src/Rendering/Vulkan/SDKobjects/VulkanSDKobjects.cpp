#include "VulkanSDKobjects.h"


//Command object functions
void VulkanSDKobjects::CommandPoolCreateInfoInit( 
	VkCommandPoolCreateInfo& commandPoolInfo, uint32_t queueFamilyIndex, 
	VkCommandPoolCreateFlags flags)
{
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolInfo.flags = flags;
}

void VulkanSDKobjects::CommandBufferAllocInfoInit(
	VkCommandBufferAllocateInfo& commandBufferInfo, const VkCommandPool& rCommandPool, 
	VkCommandBufferLevel cmdbLevel, uint32_t commandBufferCount /* =1 */)
{
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.commandBufferCount = commandBufferCount;
	commandBufferInfo.commandPool = rCommandPool;
	commandBufferInfo.level = cmdbLevel;
}

void VulkanSDKobjects::CommandBufferBeginInfoInit(VkCommandBufferBeginInfo& commandBufferBegin,
	VkCommandBufferUsageFlags cmdbUsageFlags /* =0 */)
{
	commandBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBegin.flags = cmdbUsageFlags;
}




//Queue object functions
uint32_t VulkanSDKobjects::GetDeviceGraphicsQueueFamilyIndex(
	const VkPhysicalDevice& rPhysicalDeviceHandle)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(rPhysicalDeviceHandle, 
		&queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies
	{ 
		queueFamilyCount 
	};
	vkGetPhysicalDeviceQueueFamilyProperties(rPhysicalDeviceHandle, 
		&queueFamilyCount, queueFamilies.data());

	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			return i;
		}
	}

	return UINT32_MAX;
}

uint32_t VulkanSDKobjects::GetDevicePresentQueueFamilyIndex(
	VkPhysicalDevice& rPhysicalDeviceHandle,
	const VkSurfaceKHR& rSurface)
{
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(rPhysicalDeviceHandle,
		&queueFamilyPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueProperties{
		queueFamilyPropertiesCount };
	vkGetPhysicalDeviceQueueFamilyProperties(rPhysicalDeviceHandle,
		&queueFamilyPropertiesCount, queueProperties.data());

	for (size_t i = 0; i < queueProperties.size(); ++i)
	{
		VkBool32 bPresentFamily = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(rPhysicalDeviceHandle,
			static_cast<uint32_t>(i), rSurface, &bPresentFamily);
		if (bPresentFamily)
		{
			return (static_cast<uint32_t>(i));
		}
	}

	return UINT32_MAX;
}

void VulkanSDKobjects::PresentInfoKHRInit(VkPresentInfoKHR& presentInfo, 
	VkSwapchainKHR& swapchain, uint32_t* pImageIndex, VkSemaphore* pWaitSemaphore)
{
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = pImageIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = pWaitSemaphore;
}

void VulkanSDKobjects::CommandBufferSubmitInfoInit(
	VkCommandBufferSubmitInfo& submitInfo, VkCommandBuffer& commandBuffer)
{
	submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	submitInfo.commandBuffer = commandBuffer;
	submitInfo.deviceMask = 0;
	
}

void VulkanSDKobjects::SubmitInfo2Init(VkSubmitInfo2& queueSubmitInfo,
	VkSemaphoreSubmitInfo* waitSemaphoreInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
	VkCommandBufferSubmitInfo* commandBufferSubmit)
{
	queueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	queueSubmitInfo.commandBufferInfoCount = 1;
	queueSubmitInfo.pCommandBufferInfos = commandBufferSubmit;
	queueSubmitInfo.waitSemaphoreInfoCount = 1;
	queueSubmitInfo.pWaitSemaphoreInfos = waitSemaphoreInfo;
	queueSubmitInfo.signalSemaphoreInfoCount = 1;
	queueSubmitInfo.pSignalSemaphoreInfos = signalSemaphoreInfo;
}





//Shader input structs initialization
void VulkanSDKobjects::PipelineLayoutCreateInfoInit(VkPipelineLayoutCreateInfo& info,
	VkDescriptorSetLayout* descriptorSetLayouts, uint32_t descriptorSetLayoutCount,
	VkPushConstantRange* pPushConstants /* =nullptr */, uint32_t pushConstantCount /* =0 */)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.setLayoutCount = descriptorSetLayoutCount;
	info.pSetLayouts = descriptorSetLayouts;
	info.pushConstantRangeCount = pushConstantCount;
	info.pPushConstantRanges = pPushConstants;
}

void VulkanSDKobjects::DescriptorSetLayoutBindingInit(
	VkDescriptorSetLayoutBinding& layoutBinding, uint32_t binding, 
	VkDescriptorType type, VkShaderStageFlags shaderStage,
	uint32_t descriptorCount /* =1 */)
{
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = type;
	layoutBinding.descriptorCount = descriptorCount;
	layoutBinding.stageFlags = shaderStage;
}

void VulkanSDKobjects::DescriptorSetLayoutCreateInfoInit(
	VkDescriptorSetLayoutCreateInfo& info, uint32_t bindingCount, 
	VkDescriptorSetLayoutBinding* bindings, 
	VkDescriptorSetLayoutCreateFlags flags /* =0 */)
{
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount = bindingCount;
	info.pBindings = bindings;
	info.flags = flags;
}

void VulkanSDKobjects::DescriptorPoolCreateInfoInit(
	VkDescriptorPoolCreateInfo& poolInfo, uint32_t maxSets, 
	uint32_t poolSizeCount, VkDescriptorPoolSize* pPoolSizes, 
	VkDescriptorPoolCreateFlags flags /* =0 */)
{
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets = maxSets;
	poolInfo.poolSizeCount = poolSizeCount;
	poolInfo.pPoolSizes = pPoolSizes;
	poolInfo.flags = flags;
}

void VulkanSDKobjects::DescriptorSetAllocateInfoInit(
	VkDescriptorSetAllocateInfo& allocInfo, 
	const VkDescriptorPool& descriptorPool, 
	VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount /* =1 */)
{
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = descriptorSetCount;
	allocInfo.pSetLayouts = pSetLayouts;
}

void VulkanSDKobjects::WriteDescriptorSetImageInit(
	VkWriteDescriptorSet& write, const VkDescriptorSet& descriptorSet, 
	VkDescriptorType descriptorType, VkDescriptorImageInfo* imageInfo, 
	uint32_t binding, uint32_t descriptorCount /* =1 */)
{
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = descriptorSet;
	write.dstBinding = binding;
	write.descriptorCount = descriptorCount;
	write.descriptorType = descriptorType;
	write.pImageInfo = imageInfo;
}

void VulkanSDKobjects::PushConstantRangeInit(VkPushConstantRange& pushConstant, 
	uint32_t size, VkShaderStageFlags shaderStage, uint32_t offset /* =0 */)
{
	pushConstant.offset = offset;
	pushConstant.size = size;
	pushConstant.stageFlags = shaderStage;
}




//Pipeline stage objects
void VulkanSDKobjects::ShaderModuleCreateInfoInit(VkShaderModuleCreateInfo& info, 
	std::vector<char>& rShaderCode)
{
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = rShaderCode.size();
	info.pCode = reinterpret_cast<const uint32_t*>
		(rShaderCode.data());
}

void VulkanSDKobjects::PipelineShaderStageInit(
	VkPipelineShaderStageCreateInfo& shaderStage,
	const VkShaderModule& shaderModule, VkShaderStageFlagBits stage)
{
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.pName = "main";
	shaderStage.module = shaderModule;
	shaderStage.stage = stage;
}

void VulkanSDKobjects::PipelineInputAssemblyStateCreateInfoInit(
	VkPipelineInputAssemblyStateCreateInfo& inputAssembly, 
	VkPrimitiveTopology primitiveTopology, VkBool32 bPrimitiveRestart /* =VK_FALSE */)
{
	inputAssembly.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.primitiveRestartEnable = bPrimitiveRestart;
	inputAssembly.topology = primitiveTopology;
}

void VulkanSDKobjects::CreateVertexInputBindingDescription(
	VkVertexInputBindingDescription& inputBinding, 
	uint32_t binding, uint32_t stride, VkVertexInputRate rate)
{
	inputBinding.binding = binding;
	inputBinding.stride = stride;
	inputBinding.inputRate = rate;
}

void VulkanSDKobjects::CreateVertexInputAttributeDescription(
	VkVertexInputAttributeDescription& inputAttribute, uint32_t location, 
	uint32_t binding, VkFormat format, uint32_t offsetof)
{
	inputAttribute.binding = binding;
	inputAttribute.location = location;
	inputAttribute.format = format;
	inputAttribute.offset = offsetof;
}

void VulkanSDKobjects::CreatePipelineVertexInputState(
	VkPipelineVertexInputStateCreateInfo& vertexInput, 
	VkVertexInputBindingDescription* pInputBindings, 
	uint32_t inputBindingCount, 
	VkVertexInputAttributeDescription* pInputAttributes,
	uint32_t inputAttributeCount)
{
	vertexInput.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.vertexBindingDescriptionCount = inputBindingCount;
	vertexInput.pVertexBindingDescriptions = pInputBindings;
	vertexInput.vertexAttributeDescriptionCount = inputAttributeCount;
	vertexInput.pVertexAttributeDescriptions = pInputAttributes;
}

void VulkanSDKobjects::PipelineViewportStateCreateInfoInit(
	VkPipelineViewportStateCreateInfo& viewport, uint32_t viewportCount /* =1 */, 
	uint32_t scissorCount /* =1 */)
{
	viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.viewportCount = viewportCount;
	viewport.scissorCount = scissorCount;
}

void VulkanSDKobjects::PipelineRasterizationCreateInfoSetPolygonMode(
	VkPipelineRasterizationStateCreateInfo& info,
	VkPolygonMode polygonMode /*=VK_POLYGON_MODE_FILL*/, float lineWidth /* =1.0f */)
{
	info.polygonMode = polygonMode;
	info.lineWidth = lineWidth;
}

void VulkanSDKobjects::PipelineRasterizationCreateInfoSetCullMode(
	VkPipelineRasterizationStateCreateInfo& info, VkCullModeFlags cullMode,
	VkFrontFace frontFace)
{
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.rasterizerDiscardEnable = VK_FALSE;
	info.cullMode = cullMode;
	info.frontFace = frontFace;
}

void VulkanSDKobjects::PipelineRasterizationCreateInfoSetDepthBias(
	VkPipelineRasterizationStateCreateInfo& info, VkBool32 bDepthClampEnable, 
	float depthBiasClamp, VkBool32 bDepthBiasEnable, float depthBiasConstantFactor, 
	float depthBiasSlopeFactor)
{
	info.depthClampEnable = bDepthClampEnable;
	info.depthBiasClamp = depthBiasClamp;
	info.depthBiasEnable = bDepthBiasEnable;
	info.depthBiasConstantFactor = depthBiasConstantFactor;
	info.depthBiasSlopeFactor = depthBiasSlopeFactor;
}

void VulkanSDKobjects::PipelineMultisampleStateCreateInfoInit(
	VkPipelineMultisampleStateCreateInfo& multisampling,
	VkSampleCountFlagBits rasterizationSamples, VkBool32 bSampleShading /* =VK_FALSE */,
	float minSampleShading /* =1.0f */, VkSampleMask* pSampleMask /* =nullptr */,
	VkBool32 bAlphaToCoverage /* =VK_FALSE */, VkBool32 bAlphaToOne /* =VK_FALSE */)
{
	multisampling.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.sampleShadingEnable = bSampleShading;
	multisampling.minSampleShading = minSampleShading;
	multisampling.pSampleMask = pSampleMask;
	multisampling.alphaToCoverageEnable = bAlphaToCoverage;
	multisampling.alphaToOneEnable = bAlphaToOne;
}

void VulkanSDKobjects::PipelineDepthStencilStateCreateInfoSetDepthTest(
	VkPipelineDepthStencilStateCreateInfo& depthStencil, 
	VkBool32 bDepthTest /*=VK_FALSE*/, VkBool32 bDepthWrite /*=VK_FALSE*/, 
	VkCompareOp depthCompareOp /* =VK_COMPARE_OP_NEVER */)
{
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = bDepthTest;
	depthStencil.depthWriteEnable = bDepthWrite;
	depthStencil.depthCompareOp = depthCompareOp;
}

void VulkanSDKobjects::PipelineDepthStencilStateCreateInfoSetDepthBoundsTest(
	VkPipelineDepthStencilStateCreateInfo& depthStencil, 
	VkBool32 bDepthBounds /* =VK_FALSE */, float minDepthBounds /* =0.f */, 
	float maxDepthBounds /* =1.f */)
{
	depthStencil.depthBoundsTestEnable = bDepthBounds;
	depthStencil.minDepthBounds = minDepthBounds;
	depthStencil.maxDepthBounds = maxDepthBounds;
}

void VulkanSDKobjects::PipelineDepthStencilStateCreateInfoSetStencilTest(
	VkPipelineDepthStencilStateCreateInfo& depthStencil, VkBool32 bStencilTest /*=VK_FALSE*/,
	VkStencilOpState front /*={}*/, VkStencilOpState back /*={}*/)
{
	depthStencil.stencilTestEnable = bStencilTest;
	depthStencil.front = front;
	depthStencil.back = back;
}

void VulkanSDKobjects::PipelineColorBlendAttachmentStateInit(
	VkPipelineColorBlendAttachmentState& colorBlendAttachment, 
	VkColorComponentFlags colorWriteMask, VkBool32 bBlendEnalbe)
{
	colorBlendAttachment.colorWriteMask = colorWriteMask;
	colorBlendAttachment.blendEnable = bBlendEnalbe;
}

void VulkanSDKobjects::PipelineColorBlendStateCreateInfoInit(
	VkPipelineColorBlendStateCreateInfo& colorBlending,
	VkPipelineColorBlendAttachmentState* pColorBlendAttachments, 
	uint32_t colorBlendAttachmentCount, VkBool32 bLogicOp)
{
	colorBlending.sType = 
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	colorBlending.attachmentCount = colorBlendAttachmentCount;
	colorBlending.pAttachments = pColorBlendAttachments;
	colorBlending.logicOpEnable = bLogicOp;
}

void VulkanSDKobjects::PipelineDynamicStateCreateInfoInit(
	VkPipelineDynamicStateCreateInfo& dynamicState, VkDynamicState* pDynamicStates, 
	uint32_t dynamicStateCount)
{
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = pDynamicStates;
	dynamicState.dynamicStateCount = dynamicStateCount;
}

void VulkanSDKobjects::PipelineRenderingCreateInfoInit(
	VkPipelineRenderingCreateInfo& renderingInfo,
	VkFormat* pColorAttachmentFormats, VkFormat& depthAttachmentFormat, 
	VkFormat& stencilAttachmentFormat, uint32_t colorAttachmentCount /* =1 */)
{
	renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	renderingInfo.pColorAttachmentFormats = pColorAttachmentFormats;
	renderingInfo.colorAttachmentCount = colorAttachmentCount;
	renderingInfo.depthAttachmentFormat = depthAttachmentFormat;
	renderingInfo.stencilAttachmentFormat = stencilAttachmentFormat;
}//Pipeline stage objects





//Image info structures initalization
void VulkanSDKobjects::ImageCreateInfoInit(VkImageCreateInfo& imageInfo, 
	VkExtent3D& extent, VkFormat& format, VkImageUsageFlags usageFlags)
{
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.extent = extent;
	imageInfo.format = format;

	//Image tiling optimal allow the gpu to shuffle data, which allows for faster excecution
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	imageInfo.imageType = VK_IMAGE_TYPE_2D;

	imageInfo.arrayLayers = 1;
	imageInfo.mipLevels = 1;

	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.usage = usageFlags;
}

void VulkanSDKobjects::ImageViewCreateInfoInit(VkImageViewCreateInfo& 
	imageViewInfo, VkImage& image, VkImageAspectFlags aspectMask, VkFormat& format)
{
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = image;
	imageViewInfo.format = format;

	VkImageSubresourceRange subresourceRange{};
	ImageSubresourceRangeInit(subresourceRange, aspectMask);
	imageViewInfo.subresourceRange = subresourceRange;

	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
}

void VulkanSDKobjects::ImageSubresourceRangeInit(VkImageSubresourceRange& subresourceRange,
	VkImageAspectFlags aspectMask, uint32_t levelCount /* =0 */,
	uint32_t baseMipLevel /* =VK_REMAINING_MIP_LEVELS */)
{
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
}//Image info structures initalization end





void VulkanSDKobjects::BufferCreateInfoInit(VkBufferCreateInfo& info, 
	VkDeviceSize allocSize, VkBufferUsageFlags usage)
{
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.size = allocSize;
	info.usage = usage;
}

void VulkanSDKobjects::BufferCopyInit(VkBufferCopy& bufferCopy, 
	VkDeviceSize size, VkDeviceSize srcOffset /* =0 */, 
	VkDeviceSize dstOffset /* =0 */)
{
	bufferCopy.size = size;
	bufferCopy.srcOffset = srcOffset;
	bufferCopy.dstOffset = dstOffset;
}




//Sync object info sturctures initialization
void VulkanSDKobjects::SemaphoreCreateInfoInit(
	VkSemaphoreCreateInfo& semaphoreInfo, VkSemaphoreCreateFlags flags/* =0 */)
{
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.flags = flags;
}

void VulkanSDKobjects::SemaphoreSubmitInfoInit(
	VkSemaphoreSubmitInfo& semaphoreInfo, VkSemaphore& waitSemaphore,
	VkPipelineStageFlags2 stageMask)
{
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	semaphoreInfo.semaphore = waitSemaphore;
	semaphoreInfo.stageMask = stageMask;
	semaphoreInfo.deviceIndex = 0;
	semaphoreInfo.value = 1;
}

void VulkanSDKobjects::FenceCreateInfoInit(
	VkFenceCreateInfo& fenceInfo, VkFenceCreateFlags flags /* =0 */)
{
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = flags;
}//Sync object info structure initialization end




void VulkanSDKobjects::RenderingAttachmentInfoInit(
	VkRenderingAttachmentInfo& renderingAttachment, VkImageView& imageView, 
	VkImageLayout imageLayout, VkClearValue* pClearValue /* =nullptr */)
{
	renderingAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	renderingAttachment.imageView = imageView;
	renderingAttachment.imageLayout = imageLayout;
	renderingAttachment.loadOp = pClearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : 
		VK_ATTACHMENT_LOAD_OP_LOAD;
	renderingAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	if (pClearValue)
	{
		renderingAttachment.clearValue = *pClearValue;
	}
}

void VulkanSDKobjects::RenderingInfoInit(VkRenderingInfo& renderingInfo, 
	VkRenderingAttachmentInfo* pColorAttachments, VkExtent2D extent,
	VkRenderingAttachmentInfo* pDepthAttachment, VkRenderingAttachmentInfo* pStencilAttachment, 
	uint32_t layerCount /* =1 */, uint32_t colorAttachmentCount /* =1 */, uint32_t viewMask /* =0 */)
{
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = extent;
	renderingInfo.colorAttachmentCount = colorAttachmentCount;
	renderingInfo.pColorAttachments = pColorAttachments;
	renderingInfo.pDepthAttachment = pDepthAttachment;
	renderingInfo.pStencilAttachment = pStencilAttachment;
	renderingInfo.layerCount = layerCount;
	renderingInfo.viewMask = viewMask;
}