#pragma once

#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <thread>

//Includes the vulkan header files as well as glfw and the WindowData struct
#include "Engine/Inputs/glfwInputs/glfwInputs.h"

//VkBootstrap will help skip some of the boilerplate
#include "VkBootstrap.h"

#include "VulkanShaderData.h"

//Header file that includes different graphics pipeline configurations
#include "VulkanPipeline.h"

//Needed to pass meshes to the engine and turn it into shader data 
#include "Engine/GameObjects/Mesh.h"




/*------------------------------------------------------------
When vulkan is busy drawing a frame, the cpu should move on 
to process the next frame, but no more than two should be 
in flight at the same time
---------------------------------------------------------------*/
#define BLITZEN_VULKAN_MAX_FRAMES_IN_FLIGHT	2Ui32




/*----------------------------------
Shader filepath macros
-----------------------------------*/
#define BLITZEN_VULKAN_GRADIENT_COMPUTE_SHADER_FILEPATH \
"VulkanShaders/gradient.comp.glsl.spv"





//Holds some of the vulkan objects that will be initialized with vkBootstrap
struct VkBootstrapInitialized
{
	VkInstance vulkanInstance{ VK_NULL_HANDLE };

	//Validation layers should only be requested if we're in a debug configuration
	#ifdef NDEBUG
		const bool bUseValidationLayers = false;
	#else
		const bool bUseValidationLayers = true;
	#endif
	VkDebugUtilsMessengerEXT vulkanDebugMessenger{ VK_NULL_HANDLE };

	VkPhysicalDevice gpuHandle{ VK_NULL_HANDLE };

	uint32_t graphicsQueueFamilyIndex;
	VkQueue graphicsQueue{VK_NULL_HANDLE};
};

//Holds primary vulkan objects that are responsible for window interfacing
struct VulkanWindowInterfaceObjects
{
	VkSurfaceKHR windowSurface{ VK_NULL_HANDLE };

	VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;

	std::vector<VkImage> swapchainImages{ 0 };
	std::vector<VkImageView> swapchainImageViews{ 0 };
	
	uint32_t presentQueueIndex;
	VkQueue presentQueue{ VK_NULL_HANDLE };
};

//Holds an image allocated seperately from the swapchain
struct VulkanAllocatedImage
{
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkExtent3D extent;
	VkFormat format;
};

/*---------------------------------------------------------
Holds the objects that will be used each frame for commands
and synchronization functionality
-----------------------------------------------------------*/
struct VulkanFrameTools
{
	VkCommandPool renderingCommandPool;

	VkCommandBuffer renderingCommandBuffer;

	VkSemaphore imageAvailableSeamphore;

	VkSemaphore renderFinishedSemahore;

	VkFence inFlightFence;
};


struct ComputePipelineData
{
	VkShaderModule shaderModule{ VK_NULL_HANDLE };

	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

	VkPipeline computePipeline{ VK_NULL_HANDLE };
};




/*-------------------------------------------
Push constant structs
--------------------------------------------*/

struct GradientComputePushConstant
{
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};


/*------------------------------------------------------------
The vulkan Renderer is responsible for setting up the 
correct Vulkan objects, excecuting the right commands to render
the frame for the application
--------------------------------------------------------------*/
class VulkanRenderer
{
public:

	WindowData windowData{};

public:

	VulkanRenderer(BlitzenEngine::VulkanMesh* pMeshes, uint32_t meshCount);

	~VulkanRenderer();

	void DrawFrame();

private:

	//Records the command buffer that will draw the frame
	void RecordFrameCommandBuffer(const VkCommandBuffer& commandBuffer, 
		uint32_t swapchainImageIndex, VkImage& drawingImage);

	/*
	Called by RecordFrameCommandBuffer.
	Records the commands that will draw the background of the window
	*/
	void DrawBackground(const VkCommandBuffer& commandBuffer, 
		VkImage& image);

	void DrawGeometry(const VkCommandBuffer& commandBuffer);




	//Helper function called while command recording to change the layout of an image
	void TransitionImageLayoutWhileDrawing(const VkCommandBuffer& commandBuffer,
		VkImage& currentImage, VkImageLayout initialLayout, VkImageLayout finalLayout);

	//Helper function called while command recording to copy one image to another
	void CopyImageToImage(const VkCommandBuffer& commandBuffer, VkImage& srcImage,
		VkImage& dstImage, VkExtent2D& srcSize, VkExtent2D& dstSize);

private:

	//Initalizes glfw and creates a window without a host API for Vulkan
	void InitGlfwAndSetupWindow();




	/*------------------------------------------------------------------------
	Initializes the instance, device, surface and swapchain with vkBootstrap
	It also initalizes the vma allocator
	--------------------------------------------------------------------------*/
	void VulkanBootstrapHelpersInit();

	/*----------------------------------------------------
	Creates the instance to interface with the SDK and
	request validation layers if we're in debug mode
	------------------------------------------------------*/
	vkb::Instance CreateInstanceAndDebugMessenger();

	//Chooses the gpu and creates the VkDevice that will interface with it
	vkb::Device ChoosePhysicalDeviceAndCreateVkDevice(vkb::Instance& rVkbInstance);

	//Initializes the allocator
	void VmaAllocatorInit();

	//Gets all required device queues and queue family indices from the device
	void GetDeviceQueues(vkb::Device& vkbDevice);

	//Initializes the swapchain, saves the image format and extent and gets the swapchain images
	void SetupSwapchain();




	void AllocateDrawingImage();




	void VulkanFrameToolsInit();




	void AllocateMeshBuffers(BlitzenEngine::VulkanMesh* pMeshes, uint32_t meshCount);

	//Fills the buffers in s GPUMeshBuffers struct
	void AllocateGPUMeshBuffers(VulkanShaderData::GPUMeshBuffers& meshBuffers, 
		std::vector<VulkanShaderData::Vertex>& vertices, std::vector<uint32_t>& indices);

	//Allocates a AllocatedBuffer struct
	void AllocateBuffer(VulkanShaderData::AllocatedBuffer& vertexBuffer, VkDeviceSize size,
		VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);




	//Allocates all descriptors required for the program
	void DescriptorsInit();

	//Allocates descriptors for shaders that handle background drawing
	void BackgroundShadersDescriptorSetsInit();



	
	//Initializes all pipelines that will be used throughout the program
	void InitPipelines();

	//Read a shader file into an array in byte format
	void ReadShaderFile(const char* filepath, std::vector<char>& code);

	//Initializes the gradient compute pipeline
	void InitGradientComputePipeline();

private:

	//The device interfaces with the GPU that was chosen at initialization
	VkDevice device{ VK_NULL_HANDLE };

	//Responsible for vulkan object allocations
	VmaAllocator allocator;

	VkBootstrapInitialized vkBootstrapObjects{};

	VulkanWindowInterfaceObjects windowInterface{};

	const char* staticObjectVertexShaderFilepath =
		"VulkanShaders/StaticObjectVertexShader.spv";
	const char* staticObjectFragmentShaderFilepath =
		"VulkanShaders/StaticObjectFragmentShader.spv";

	VulkanAllocatedImage drawingImage;
	VkExtent2D drawExtent;

	//Holds how many frames have been rendered
	uint64_t frameCount = 0;
	uint8_t frameQueue = 0;

	//These command objects are mostly used during setup, for operations like data copies
	VkCommandPool immediateSubmitCommandPool{ VK_NULL_HANDLE };
	VkCommandBuffer immediateSubmitCommandBuffer{ VK_NULL_HANDLE };

	//Holds a list frame tools for each frame in flight that Vulkan is allowed
	std::array<VulkanFrameTools, BLITZEN_VULKAN_MAX_FRAMES_IN_FLIGHT>
		frameTools;

	std::vector<VulkanShaderData::GPUMeshBuffers> meshBuffersList;

	//Holds a descriptor pool which will allocate descriptor sets
	VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

	VkDescriptorSet backgroundDrawingDescriptorSet{ VK_NULL_HANDLE };
	std::array<VkDescriptorSetLayout, 1> backgroundDrawingDescriptorSetLayouts;

	ComputePipelineData gradientComputePipeline;

	VulkanGraphicsPipeline simpleGeometryGraphicsPipeline;
};