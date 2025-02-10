#include "VulkanRenderer.h"
#include "Rendering/Vulkan/SDKobjects/VulkanSDKobjects.h"

//Includes the Vulkan Memory Allocator with definitions
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"



/*----------------------------------------------------------------------------
Vulkan will work with glfw as its window system, initialization will happen
during the VulkanRenderer constructor function (strange choice but I can't be
bothered with changing it, so it will stay like this for now)
-----------------------------------------------------------------------------*/

void VulkanRenderer::InitGlfwAndSetupWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	windowData.pWindow = glfwCreateWindow(windowData.width,
		windowData.height, windowData.title, nullptr, nullptr);

	glfwSetWindowUserPointer(windowData.pWindow,
		reinterpret_cast<void*>(&windowData));

	//The size we're going to render to is the same as the window's size
	drawExtent.width = windowData.width;
	drawExtent.height = windowData.height;
}



/*--------------------------------------------------------------------------------
Objects that use vkBootstrap for initialization to avoid boilerplate code
---------------------------------------------------------------------------------*/


void VulkanRenderer::VulkanBootstrapHelpersInit()
{
	//Initializing the instance and debug messenger
	vkb::Instance vkbInstance = CreateInstanceAndDebugMessenger();

	//Initializing the window surface with glfw
	glfwCreateWindowSurface(vkBootstrapObjects.vulkanInstance,
		windowData.pWindow, nullptr, &(windowInterface.windowSurface));

	//Picking a physical device and creating a vulkan device based on it
	vkb::Device vkbDevice = ChoosePhysicalDeviceAndCreateVkDevice(vkbInstance);

	//Initializing the allocator
	VmaAllocatorInit();

	GetDeviceQueues(vkbDevice);

	//Initializing the swapchain and retrieving its data
	SetupSwapchain();
}

vkb::Instance VulkanRenderer::CreateInstanceAndDebugMessenger()
{
	vkb::InstanceBuilder vkbInstanceBuilder;

	vkbInstanceBuilder.set_app_name("Blitzen Vulkan Renderer")
		.request_validation_layers(vkBootstrapObjects.bUseValidationLayers) //Validation layers active for debug mode only
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0);

	//VkbInstance built to initialize instance and debug messenger
	auto vkbInstanceBuilderResult = vkbInstanceBuilder.build();
	vkb::Instance vkbInstance = vkbInstanceBuilderResult.value();

	//Instance reference from vulkan data initialized
	vkBootstrapObjects.vulkanInstance = vkbInstance.instance;

	//Debug Messenger reference from vulkan data initialized
	vkBootstrapObjects.vulkanDebugMessenger = vkbInstance.debug_messenger;

	//The vkbInstance will be used by VkBootstrap to pick the GPU later
	return vkbInstance;
}

vkb::Device VulkanRenderer::ChoosePhysicalDeviceAndCreateVkDevice( 
	vkb::Instance& rVkbInstance)
{
	//Setting desired vulkan 1.3 features
	VkPhysicalDeviceVulkan13Features vulkan13Features{};
	vulkan13Features.sType =
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vulkan13Features.dynamicRendering = true;
	vulkan13Features.synchronization2 = true;

	//Setting desired vulkan 1.2 features
	VkPhysicalDeviceVulkan12Features vulkan12Features{};
	vulkan12Features.sType =
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	vulkan12Features.bufferDeviceAddress = true;
	vulkan12Features.descriptorIndexing = true;

	//vkbDeviceSelector built with reference to vkbInstance built earlier
	vkb::PhysicalDeviceSelector vkbDeviceSelector{ rVkbInstance };

	//vkbDeviceSelector settings and picking physical device
	vkb::PhysicalDevice vkbPhysicalDevice =
		vkbDeviceSelector.set_minimum_version(1, 3)
		.set_required_features_13(vulkan13Features)
		.set_required_features_12(vulkan12Features)
		.set_surface(windowInterface.windowSurface)//Surface set with reference to VulkanData window surface
		.select()
		.value();

	//Physical device reference from Vulkan data initialized
	vkBootstrapObjects.gpuHandle = vkbPhysicalDevice.physical_device;

	//vkbDeviceBuilder built using previously selected vkbPhysicalDevice
	vkb::DeviceBuilder vkbDeviceBuilder{ vkbPhysicalDevice };
	vkb::Device vkbDevice = vkbDeviceBuilder.build().value();

	//Vulkan Device reference from Vulkan Data initialized
	device = vkbDevice.device;

	//Retrieve the vkBootstrap device for device queues
	return vkbDevice;
}

void VulkanRenderer::VmaAllocatorInit()
{
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.device = device;
	allocatorInfo.instance = vkBootstrapObjects.vulkanInstance;
	allocatorInfo.physicalDevice = vkBootstrapObjects.gpuHandle;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	vmaCreateAllocator(&allocatorInfo, &allocator);
}

void VulkanRenderer::GetDeviceQueues(vkb::Device& vkbDevice)
{
	//Initializing the graphics queue family index and the graphics queue
	vkBootstrapObjects.graphicsQueue = vkbDevice.get_queue(
		vkb::QueueType::graphics).value();
	vkBootstrapObjects.graphicsQueueFamilyIndex = 
		vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	//Initializing the present queue family index and the present queue
	windowInterface.presentQueue = vkbDevice.get_queue(
		vkb::QueueType::present).value();
	windowInterface.presentQueueIndex = vkbDevice.get_queue_index(
		vkb::QueueType::present).value();
}

void VulkanRenderer::SetupSwapchain()
{
	vkb::SwapchainBuilder vkSwapBuilder{ vkBootstrapObjects.gpuHandle,
		device, windowInterface.windowSurface };

	//Setting the desired image format
	windowInterface.swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	//Building the vkb swapchain so that the swapchain data can be retrieved
	vkb::Swapchain vkbSwapchain =
		vkSwapBuilder.set_desired_format(VkSurfaceFormatKHR{ 
		windowInterface.swapchainImageFormat,
		VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }) //Setting the desrired surface format
		//Setting the present mode to be limited to the speed of the monitor
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		//Setting the extent to our window's width and height
		.set_desired_extent(windowData.width, windowData.height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build().value();

	//VkSwapchain reference from VulkanData initialized
	windowInterface.swapchain = vkbSwapchain.swapchain;

	//SwapchainExtent reference from VulkanData initialized
	windowInterface.swapchainExtent = vkbSwapchain.extent;

	//SwapchainImageViews reference from VulkanData initialized
	windowInterface.swapchainImageViews = vkbSwapchain.get_image_views().value();

	//SwapchainImages reference from VulkanData initialized
	windowInterface.swapchainImages = vkbSwapchain.get_images().value();
}



/*------------------------------------------------------
The setup functions after this do not use VkBootstrap
-------------------------------------------------------*/


void VulkanRenderer::AllocateDrawingImage()
{
	drawingImage.extent =
	{
		static_cast<uint32_t>(windowData.width),
		static_cast<uint32_t>(windowData.height),
		1
	};

	//Draw data in 64bit format
	drawingImage.format = VK_FORMAT_R16G16B16A16_SFLOAT;

	//Initializing all image usage flags
	VkImageUsageFlags imageUsage{};
	imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imageUsage |= VK_IMAGE_USAGE_STORAGE_BIT;

	//Initalizing an image info struct based on our data
	VkImageCreateInfo imageInfo{};
	VulkanSDKobjects::ImageCreateInfoInit(imageInfo, drawingImage.extent,
		drawingImage.format, imageUsage);

	//To allocate with vma, a vma allocation struct is also needed
	VmaAllocationCreateInfo vmaAllocationInfo{};
	vmaAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaAllocationInfo.requiredFlags = VkMemoryPropertyFlags(
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//Allocating the image
	vmaCreateImage(allocator, &imageInfo, &vmaAllocationInfo, &(drawingImage.image),
		&(drawingImage.allocation), nullptr);

	VkImageViewCreateInfo imageViewInfo{};
	VulkanSDKobjects::ImageViewCreateInfoInit(imageViewInfo, drawingImage.image,
		VK_IMAGE_ASPECT_COLOR_BIT, drawingImage.format);

	vkCreateImageView(device, &imageViewInfo, nullptr, &drawingImage.imageView);
}





void VulkanRenderer::VulkanFrameToolsInit()
{
	//Creates the command pool for immediate submit commands
	VkCommandPoolCreateInfo immediateCommandPoolInfo{};
	VulkanSDKobjects::CommandPoolCreateInfoInit(immediateCommandPoolInfo,
		vkBootstrapObjects.graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	vkCreateCommandPool(device, &immediateCommandPoolInfo, nullptr, &immediateSubmitCommandPool);

	//Creates the command buffer for immediate submit commands
	VkCommandBufferAllocateInfo immediateCommandBufferInfo{};
	VulkanSDKobjects::CommandBufferAllocInfoInit(immediateCommandBufferInfo, immediateSubmitCommandPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	vkAllocateCommandBuffers(device, &immediateCommandBufferInfo, &immediateSubmitCommandBuffer);

	/*
		The command pool in the array have the same functionality,
		so they will use the same create info struct
	*/
	VkCommandPoolCreateInfo commandPoolInfo{};
	VulkanSDKobjects::CommandPoolCreateInfoInit(commandPoolInfo,
		vkBootstrapObjects.graphicsQueueFamilyIndex,
		/*We ask to have the ability to reset the command buffers*/
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	/*
	All semaphore infos will have similar funcionality so we only need one
	create info struct for their creation
	*/
	VkSemaphoreCreateInfo semaphoreInfo{};
	VulkanSDKobjects::SemaphoreCreateInfoInit(semaphoreInfo);
	/*
	All fence infos will have similar funcionality so we only need one
	create info struct for their creation
	*/
	VkFenceCreateInfo fenceInfo{};
	VulkanSDKobjects::FenceCreateInfoInit(fenceInfo, VK_FENCE_CREATE_SIGNALED_BIT);

	for (size_t i = 0; i < frameTools.size(); ++i)
	{
		//Create the command pool
		vkCreateCommandPool(device, &commandPoolInfo, nullptr, 
			&(frameTools[i].renderingCommandPool));
		/*
		Since each command buffer has its own command pool, the info struct will also
		have to change each time a new command buffer has to be created
		*/
		VkCommandBufferAllocateInfo commandBufferInfo{};
		VulkanSDKobjects::CommandBufferAllocInfoInit(commandBufferInfo,
			frameTools[i].renderingCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		//Allocate command buffer based on previous command pool
		vkAllocateCommandBuffers(device, &commandBufferInfo, 
			&(frameTools[i].renderingCommandBuffer));
		
		//Creating the semaphore and the fence
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, 
			&(frameTools[i].imageAvailableSeamphore));
		vkCreateSemaphore(device, &semaphoreInfo, nullptr,
			&(frameTools[i].renderFinishedSemahore));
		vkCreateFence(device, &fenceInfo, nullptr,
			&(frameTools[i].inFlightFence));
	}
}




void VulkanRenderer::AllocateMeshBuffers(BlitzenEngine::VulkanMesh* pMeshes,
	uint32_t meshCount)
{
	//Iterate through all the meshes
	for (size_t i = 0; i < static_cast<size_t>(meshCount); ++i)
	{
		//Make space in the mesh buffers list array
		meshBuffersList.resize(i + 1);
		//Allocate the buffers in the current element in the mesh buffers list
		AllocateGPUMeshBuffers(meshBuffersList[i], pMeshes[i].vertices, pMeshes[i].indices);
	}
}

void VulkanRenderer::AllocateGPUMeshBuffers(VulkanShaderData::GPUMeshBuffers& meshBuffers,
	std::vector<VulkanShaderData::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	//Find the size needed for the vertices that the buffer is going to hold
	VkDeviceSize vertexBufferSize = sizeof(VulkanShaderData::Vertex) * vertices.size();
	/*
	Allocate a vertex buffer for those vertices using the vma allocator. The buffer is an SSBO, 
	that will have a staging buffer transfer memory to it at the end of this function. It will also 
	allow us to get its address in the vertex shader and vma will allocate as a GPU only buffer
	*/
	AllocateBuffer(meshBuffers.vertexBuffer, vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo bufferAddressInfo{};
	bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferAddressInfo.buffer = meshBuffers.vertexBuffer.buffer;
	meshBuffers.vertexBufferAddress = vkGetBufferDeviceAddress(device, &bufferAddressInfo);

	//Find the size needed for the indices that the buffers is going to hold
	VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
	/*
	Allocate an index buffer for the indices using the vma allocator. The buffer has the index 
	buffer bit and also the transfer as it will accept a data transfer at the end of this function
	from a staging buffer. Vma will initialize it as GPU only
	*/
	AllocateBuffer(meshBuffers.indexBuffer, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

	/* 
	Create a staging buffer with enough space for the vertices and indices.
	It will be used to transfer the data to the actual buffers
	*/
	VulkanShaderData::AllocatedBuffer stagingBuffer;
	AllocateBuffer(stagingBuffer, vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY);

	//Get the memory address of the staging buffer
	void* data = stagingBuffer.allocation->GetMappedData();

	//Copy the data of the vertices to the void pointer data that was initalized earlier
	memcpy(data, vertices.data(), vertexBufferSize);
	//Copy the data of the indices to the indices part of the void pointer
	memcpy(reinterpret_cast<char*>(data) + vertexBufferSize, indices.data(), indexBufferSize);

	//Begin recording the immediate submit command buffer to copy the data to the actual buffers
	VkCommandBufferBeginInfo commandBufferBegin{};
	VulkanSDKobjects::CommandBufferBeginInfoInit(commandBufferBegin, 
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	vkBeginCommandBuffer(immediateSubmitCommandBuffer, &commandBufferBegin);

	//Copy the first part of the staging buffer to the vertex buffer
	VkBufferCopy vertexBufferCopy{0};
	VulkanSDKobjects::BufferCopyInit(vertexBufferCopy, vertexBufferSize);
	vkCmdCopyBuffer(immediateSubmitCommandBuffer, stagingBuffer.buffer, meshBuffers.vertexBuffer.buffer,
		1, &vertexBufferCopy);
	
	//Copy the indices part of the staging buffer to the index buffer
	VkBufferCopy indexBufferCopy{0};
	VulkanSDKobjects::BufferCopyInit(indexBufferCopy, indexBufferSize, vertexBufferSize);
	vkCmdCopyBuffer(immediateSubmitCommandBuffer, stagingBuffer.buffer, meshBuffers.indexBuffer.buffer,
		1, &indexBufferCopy);

	vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);

	vkEndCommandBuffer(immediateSubmitCommandBuffer);

	VkCommandBufferSubmitInfo submitInfo{};
	VulkanSDKobjects::CommandBufferSubmitInfoInit(submitInfo, immediateSubmitCommandBuffer);
	VkSubmitInfo2 submitInfo2{};
	submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo2.commandBufferInfoCount = 1;
	submitInfo2.pCommandBufferInfos = &submitInfo;
	vkQueueSubmit2(vkBootstrapObjects.graphicsQueue, 1, &submitInfo2, VK_NULL_HANDLE);
	vkQueueWaitIdle(vkBootstrapObjects.graphicsQueue);

	vkResetCommandBuffer(immediateSubmitCommandBuffer, 0);
}

void VulkanRenderer::AllocateBuffer(VulkanShaderData::AllocatedBuffer& vertexBuffer,
	VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo vertexBufferInfo{};
	VulkanSDKobjects::BufferCreateInfoInit(vertexBufferInfo, size, usage);

	VmaAllocationCreateInfo vertexAllocationInfo{};
	vertexAllocationInfo.usage = memoryUsage;
	vertexAllocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	vmaCreateBuffer(allocator, &vertexBufferInfo, &vertexAllocationInfo, &(vertexBuffer.buffer),
		&(vertexBuffer.allocation), &(vertexBuffer.allocationInfo));
}





void VulkanRenderer::DescriptorsInit()
{
	BackgroundShadersDescriptorSetsInit();
}

void VulkanRenderer::BackgroundShadersDescriptorSetsInit()
{
	//Add descriptor set layout bindings
	std::array<VkDescriptorSetLayoutBinding, 1> descriptorSetLayoutBindings;
	for (size_t i = 0; i < descriptorSetLayoutBindings.size(); ++i)
	{
		VulkanSDKobjects::DescriptorSetLayoutBindingInit(
			descriptorSetLayoutBindings[i], static_cast<uint32_t>(i), 
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
	}

	//Create descriptor set layouts for the descriptor set
	for (size_t i = 0; i < backgroundDrawingDescriptorSetLayouts.size(); ++i)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		VulkanSDKobjects::DescriptorSetLayoutCreateInfoInit(layoutInfo,
			1, descriptorSetLayoutBindings.data());
		vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
			&backgroundDrawingDescriptorSetLayouts[i]);
	}

	//Creating the descriptor pool that will allocate this descriptor
	//(this could be done before this function since this descriptor pool can be used for more allocations)
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize.descriptorCount = 10;
	VkDescriptorPoolCreateInfo poolInfo{};
	VulkanSDKobjects::DescriptorPoolCreateInfoInit(poolInfo, 10, 1, &poolSize);
	vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);

	VkDescriptorSetAllocateInfo descriptorSetInfo{};
	VulkanSDKobjects::DescriptorSetAllocateInfoInit(descriptorSetInfo,
		descriptorPool, backgroundDrawingDescriptorSetLayouts.data());
	vkAllocateDescriptorSets(device, &descriptorSetInfo, &backgroundDrawingDescriptorSet);

	VkDescriptorImageInfo imageDescriptor{};
	imageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageDescriptor.imageView = drawingImage.imageView;

	VkWriteDescriptorSet descriptorWrite{};
	VulkanSDKobjects::WriteDescriptorSetImageInit(descriptorWrite, 
		backgroundDrawingDescriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
		&imageDescriptor, 0);

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}





void VulkanRenderer::InitPipelines()
{
	InitGradientComputePipeline();

	//Creates a pipeline that handles drawing basic geometry
	simpleGeometryGraphicsPipeline.InitBasicGeometryPipeline(device, &(drawingImage.format));
}

void VulkanRenderer::ReadShaderFile(const char* filepath,
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

void VulkanRenderer::InitGradientComputePipeline()
{
	//Create a push constant range to pass to the pipeline layout
	VkPushConstantRange pushConstant{};
	VulkanSDKobjects::PushConstantRangeInit(pushConstant,
		sizeof(GradientComputePushConstant), VK_SHADER_STAGE_COMPUTE_BIT);

	//Create the pipeline layout info and the pipeline layout itself
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	VulkanSDKobjects::PipelineLayoutCreateInfoInit(pipelineLayoutInfo,
		backgroundDrawingDescriptorSetLayouts.data(), 1, &pushConstant, 1);
	vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, 
		&(gradientComputePipeline.pipelineLayout));

	//Read the shader code in byte form
	std::vector<char> shaderCodeBuffer;
	ReadShaderFile(BLITZEN_VULKAN_GRADIENT_COMPUTE_SHADER_FILEPATH,
		shaderCodeBuffer);

	//Pass the shader code to a shader module info and create the shader module
	VkShaderModuleCreateInfo shaderModuleInfo{};
	VulkanSDKobjects::ShaderModuleCreateInfoInit(shaderModuleInfo,
		shaderCodeBuffer);
	VkShaderModule shaderModule{};
	vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &shaderModule);

	//Create a shader stage for the shader module
	VkPipelineShaderStageCreateInfo shaderStage{};
	VulkanSDKobjects::PipelineShaderStageInit(shaderStage, shaderModule, 
		VK_SHADER_STAGE_COMPUTE_BIT);

	//Create the compute pipeline with the shader stage and the layout we created 
	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = shaderStage;
	pipelineInfo.layout = gradientComputePipeline.pipelineLayout;
	vkCreateComputePipelines(device, nullptr, 1, &pipelineInfo, nullptr,
		&(gradientComputePipeline.computePipeline));

	//Get rid of the shader module
	vkDestroyShaderModule(device, shaderModule, nullptr);
}