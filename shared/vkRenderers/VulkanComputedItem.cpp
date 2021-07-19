#include "shared/vkRenderers/VulkanComputedItem.h"

ComputedItem::ComputedItem(VulkanRenderDevice& vkDev, uint32_t uniformBufferSize)
	: vkDev(vkDev)
{
	uniformBuffer.size = uniformBufferSize;

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	if (vkCreateFence(vkDev.device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS)
		exit(EXIT_FAILURE);

	if (!createUniformBuffer(vkDev, uniformBuffer.buffer, uniformBuffer.memory, uniformBuffer.size))
		exit(EXIT_FAILURE);
}

ComputedItem::~ComputedItem()
{
	vkDestroyBuffer(vkDev.device, uniformBuffer.buffer, nullptr);
	vkFreeMemory(vkDev.device, uniformBuffer.memory, nullptr);

	vkDestroyFence(vkDev.device, fence, nullptr);

	vkDestroyDescriptorPool(vkDev.device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vkDev.device, dsLayout, nullptr);
	vkDestroyPipeline(vkDev.device, pipeline, nullptr);
	vkDestroyPipelineLayout(vkDev.device, pipelineLayout, nullptr);
}

void ComputedItem::fillComputeCommandBuffer(void* pushConstant, uint32_t pushConstantSize, uint32_t xsize, uint32_t ysize, uint32_t zsize)
{
	VkCommandBuffer commandBuffer = vkDev.computeCommandBuffer;

	const VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		.pInheritanceInfo = nullptr
	};

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);

	if (pushConstant && pushConstantSize > 0)
	{
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, pushConstantSize, pushConstant);
	}

	vkCmdDispatch(commandBuffer, xsize, ysize, zsize);
	vkEndCommandBuffer(commandBuffer);
}

bool ComputedItem::submit()
{
	// Use a fence to ensure that compute command buffer has finished executing before using it again
	waitFence();

	const VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &vkDev.computeCommandBuffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};

	return (vkQueueSubmit(vkDev.computeQueue, 1, &submitInfo, fence) == VK_SUCCESS);
}

void ComputedItem::waitFence()
{
	vkWaitForFences(vkDev.device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkResetFences(vkDev.device, 1, &fence);
}
