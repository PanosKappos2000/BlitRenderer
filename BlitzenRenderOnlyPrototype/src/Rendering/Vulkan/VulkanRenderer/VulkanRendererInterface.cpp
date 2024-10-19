#include "VulkanRenderer.h"
#include "Rendering/Vulkan/SDKobjects/VulkanSDKobjects.h"

VulkanRenderer::VulkanRenderer(BlitzenEngine::VulkanMesh* pMeshes, uint32_t meshCount)
{
	InitGlfwAndSetupWindow();

	VulkanBootstrapHelpersInit();

	AllocateDrawingImage();

	VulkanFrameToolsInit();

	AllocateMeshBuffers(pMeshes, meshCount);

	DescriptorsInit();

	InitPipelines();
}

VulkanRenderer::~VulkanRenderer()
{
	vkDeviceWaitIdle(device);

	simpleGeometryGraphicsPipeline.Cleanup(device);

	vkDestroyPipeline(device, gradientComputePipeline.computePipeline, nullptr);

	vkDestroyPipelineLayout(device, gradientComputePipeline.pipelineLayout,
		nullptr);

	for (size_t i = 0; i < backgroundDrawingDescriptorSetLayouts.
		size(); ++i)
	{
		vkDestroyDescriptorSetLayout(device, 
			backgroundDrawingDescriptorSetLayouts[i],
			nullptr);
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);

	vmaDestroyBuffer(allocator, meshBuffersList[0].vertexBuffer.buffer, 
		meshBuffersList[0].vertexBuffer.allocation);
	vmaDestroyBuffer(allocator, meshBuffersList[0].indexBuffer.buffer,
		meshBuffersList[0].indexBuffer.allocation);

	//Destroying the objects in the frame tools array
	for (size_t i = 0; i < frameTools.size(); ++i)
	{
		//Destroy synchronization structs in array
		vkDestroySemaphore(device, frameTools[i].imageAvailableSeamphore, nullptr);
		vkDestroySemaphore(device, frameTools[i].renderFinishedSemahore, nullptr);
		vkDestroyFence(device, frameTools[i].inFlightFence, nullptr);

		//Destroying each command pool also deallocates the command buffers
		vkDestroyCommandPool(device, frameTools[i].renderingCommandPool, 
			nullptr);
	}

	vkDestroyCommandPool(device, immediateSubmitCommandPool, nullptr);

	vkDestroyImageView(device, drawingImage.imageView, nullptr);
	vmaDestroyImage(allocator, drawingImage.image, drawingImage.allocation);

	for (size_t i = 0; i < windowInterface.swapchainImageViews.size(); ++i)
	{
		vkDestroyImageView(device, windowInterface.swapchainImageViews[i], 
			nullptr);
	}
	
	vkDestroySwapchainKHR(device, windowInterface.swapchain, nullptr);

	vmaDestroyAllocator(allocator);

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(vkBootstrapObjects.vulkanInstance, 
		windowInterface.windowSurface, nullptr);

	vkb::destroy_debug_utils_messenger(vkBootstrapObjects.vulkanInstance,
		vkBootstrapObjects.vulkanDebugMessenger, nullptr);

	vkDestroyInstance(vkBootstrapObjects.vulkanInstance, nullptr);

	glfwDestroyWindow(windowData.pWindow);

	glfwTerminate();
}







/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Calls the one and only DrawFrame/renderLoop function when Vulkan is
used for rendering. From here all the previously initialized structures
are manipulated to call the right render loop funtions and draw a scene
correctly
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

void VulkanRenderer::DrawFrame()
{
	if (windowData.bWindowShouldStopRendering)
	{
		vkDeviceWaitIdle(device);
	}
	/*
	The cpu waits for the previous frame to signal the fence and resets it
	when it gets the signal. We should not face a deadlock here as the fence
	was initialized to be in the signaled state
	*/
	vkWaitForFences(device, 1, &(frameTools[frameQueue].inFlightFence),
		VK_TRUE, 1000000000);
	vkResetFences(device, 1, &(frameTools[frameQueue].inFlightFence));

	/*
	The next image that can show rendering results is requested from the swapchain
	When it is found the image available seamphore of this frame is signaled, to allow
	for rendering to start
	*/
	uint32_t swapchainImageIndex;
	vkAcquireNextImageKHR(device, windowInterface.swapchain, 1000000000,
		frameTools[frameQueue].imageAvailableSeamphore, nullptr, &swapchainImageIndex);

	//Records commands for drawing to the frame
	RecordFrameCommandBuffer(frameTools[frameQueue].
		renderingCommandBuffer, swapchainImageIndex, drawingImage.image);


	//With the command buffer recorded, it should now be submitted to the graphics queue
	/*
	The wait semaphore info specifies a pipeline stage that should be stopped.
	Specifically the pipeline should stop before rendering the color attachment's
	output
	*/
	VkSemaphoreSubmitInfo waitSemaphoreInfo{};
	VulkanSDKobjects::SemaphoreSubmitInfoInit(waitSemaphoreInfo, 
		frameTools[frameQueue].imageAvailableSeamphore, 
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);

	//The signal seamaphore will stop all pipeline stages before being finished
	VkSemaphoreSubmitInfo signalSemaphoreInfo{};
	VulkanSDKobjects::SemaphoreSubmitInfoInit(signalSemaphoreInfo,
		frameTools[frameQueue].renderFinishedSemahore,
		VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR);

	//Creating a command buffer submit struct as well
	VkCommandBufferSubmitInfo commandBufferSubmit{};
	VulkanSDKobjects::CommandBufferSubmitInfoInit(commandBufferSubmit,
		frameTools[frameQueue].renderingCommandBuffer);

	//The submit info will include the semaphores and command buffers
	VkSubmitInfo2 queueSubmitInfo{};
	VulkanSDKobjects::SubmitInfo2Init(queueSubmitInfo, &waitSemaphoreInfo,
		&signalSemaphoreInfo, &commandBufferSubmit);

	//When the command buffer is submitted the fence of this frame number is signalled
	vkQueueSubmit2(vkBootstrapObjects.graphicsQueue, 1, &queueSubmitInfo, 
		frameTools[frameQueue].inFlightFence);

	//Finally the rendering results are presented to the swapchain
	VkPresentInfoKHR presentInfo{};
	VulkanSDKobjects::PresentInfoKHRInit(presentInfo, windowInterface.swapchain,
		&swapchainImageIndex, &(frameTools[frameQueue].renderFinishedSemahore));
	vkQueuePresentKHR(windowInterface.presentQueue, &presentInfo);

	//Add the new frame to the frame count and update the frame queue variable
	++frameCount;
	frameQueue = frameCount % BLITZEN_VULKAN_MAX_FRAMES_IN_FLIGHT;
}