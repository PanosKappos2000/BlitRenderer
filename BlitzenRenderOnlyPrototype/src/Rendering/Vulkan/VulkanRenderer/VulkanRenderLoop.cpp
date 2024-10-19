#include "VulkanRenderer.h"
#include "Rendering/Vulkan/SDKobjects/VulkanSDKobjects.h"

void VulkanRenderer::RecordFrameCommandBuffer(
	const VkCommandBuffer& commandBuffer, uint32_t swapchainImageIndex, 
	VkImage& drawingImage)
{
	//For a command buffer to record commands, it needs to be reset
	vkResetCommandBuffer(commandBuffer, 0);

	//Putting the command buffer in the ready state to be recorded
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	VulkanSDKobjects::CommandBufferBeginInfoInit(commandBufferBeginInfo);
	vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	//Transitioning the image layout so that vkCmdClearValue can write to it
	TransitionImageLayoutWhileDrawing(commandBuffer, drawingImage, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	//Drawing the background
	DrawBackground(commandBuffer, drawingImage);

	DrawGeometry(commandBuffer);

	//Changing the drawing image layout to transfer source and the swapchain's to transfer dst
	TransitionImageLayoutWhileDrawing(commandBuffer, drawingImage,
		VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	TransitionImageLayoutWhileDrawing(commandBuffer, 
		windowInterface.swapchainImages[swapchainImageIndex],
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	CopyImageToImage(commandBuffer, drawingImage, windowInterface.
		swapchainImages[swapchainImageIndex], drawExtent,
		windowInterface.swapchainExtent);

	//Transitioning the image layout so that it can be presented to the swapchain
	TransitionImageLayoutWhileDrawing(commandBuffer,
		windowInterface.swapchainImages[swapchainImageIndex],
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//The command buffer has recorded all commands, so it should not record anymore
	vkEndCommandBuffer(commandBuffer);
}

void VulkanRenderer::TransitionImageLayoutWhileDrawing(const VkCommandBuffer& commandBuffer,
	VkImage& currentImage, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	//We will need an image memory barrier before excecuting a transition command
	VkImageMemoryBarrier2 imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imageMemoryBarrier.oldLayout = initialLayout;
	imageMemoryBarrier.newLayout = finalLayout;
	imageMemoryBarrier.image = currentImage;

	//The barrier will stop all pipeline commands, even thought this is not optimal
	imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

	imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT |
		VK_ACCESS_2_MEMORY_READ_BIT;

	//The subresource range helps specify what aspect of the image the barrier should access
	VkImageAspectFlags subresourceRangeAspectMask = 
		/*Depending on the type of transitions requested, 
		the barrier will access the appropriate aspect*/(finalLayout ==
		VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? 
		VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageSubresourceRange subresourceRange{};
	VulkanSDKobjects::ImageSubresourceRangeInit(subresourceRange,
		subresourceRangeAspectMask);
	imageMemoryBarrier.subresourceRange = subresourceRange;

	//Creates a dependency for the barrier that specifies an imageMemory barrier
	VkDependencyInfo barrierDependency{};
	barrierDependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	barrierDependency.imageMemoryBarrierCount = 1;
	barrierDependency.pImageMemoryBarriers = &imageMemoryBarrier;

	//Excecutes the barrier
	vkCmdPipelineBarrier2(commandBuffer, &barrierDependency);
}

void VulkanRenderer::DrawBackground(const VkCommandBuffer& commandBuffer,
	VkImage& image)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
		gradientComputePipeline.computePipeline);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
		gradientComputePipeline.pipelineLayout, 0, 1, &backgroundDrawingDescriptorSet,
		0, nullptr);

	GradientComputePushConstant colorPushConstant;
	colorPushConstant.data1 = glm::vec4(1, 0, 0, 1);
	colorPushConstant.data2 = glm::vec4(0, 0, 1, 1);

	vkCmdPushConstants(commandBuffer, gradientComputePipeline.pipelineLayout,
		VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(GradientComputePushConstant),
		&colorPushConstant);

	vkCmdDispatch(commandBuffer, std::ceil(drawExtent.width / 16.0), 
		std::ceil(drawExtent.height / 16.0), 1);
}

void VulkanRenderer::DrawGeometry(const VkCommandBuffer& commandBuffer)
{
	//This render pass is going to use a single color attachment
	VkRenderingAttachmentInfo colorAttachment{};
	VulkanSDKobjects::RenderingAttachmentInfoInit(colorAttachment, drawingImage.imageView, 
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	//Creating the rendering info to start rendering
	VkRenderingInfo renderingInfo{};
	VulkanSDKobjects::RenderingInfoInit(renderingInfo, &colorAttachment,
		drawExtent, nullptr, nullptr);
	vkCmdBeginRendering(commandBuffer, &renderingInfo);

	//Binding the pipeline we want to use
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
		simpleGeometryGraphicsPipeline.graphicsPipeline);

	//Sets dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = drawExtent.width;
	viewport.height = drawExtent.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = drawExtent.width;
	scissor.extent.height = drawExtent.height;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdBindIndexBuffer(commandBuffer, meshBuffersList[0].indexBuffer.buffer, 0, 
		VK_INDEX_TYPE_UINT32);

	VulkanShaderData::GPUPushConstants pushConstants;
	pushConstants.modelMatrix = glm::mat4(1.0f);
	pushConstants.vertexBuffer = meshBuffersList[0].vertexBufferAddress;
	vkCmdPushConstants(commandBuffer, simpleGeometryGraphicsPipeline.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VulkanShaderData::GPUPushConstants),
		&pushConstants);

	//Draw command, draws 3 vertices
	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

	vkCmdEndRendering(commandBuffer);
}

void VulkanRenderer::CopyImageToImage(const VkCommandBuffer& commandBuffer, 
	VkImage& srcImage, VkImage& dstImage, VkExtent2D& srcSize, VkExtent2D& dstSize)
{
	VkImageBlit2 blitRegion{};
	blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

	blitRegion.srcOffsets[1].x = srcSize.width;
	blitRegion.srcOffsets[1].y = srcSize.height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dstSize.width;
	blitRegion.dstOffsets[1].y = dstSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ };
	blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitInfo.dstImage = dstImage;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = srcImage;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(commandBuffer, &blitInfo);
}