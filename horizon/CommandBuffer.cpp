#include "CommandBuffer.h"

namespace Horizon {

	CommandBuffer::CommandBuffer(RenderContext& renderContext, std::shared_ptr<Device> device) :mRenderContext(renderContext), mDevice(device)
	{
		createCommandPool();
		allocateCommandBuffers();
		createSyncObjects();
	}

	CommandBuffer::~CommandBuffer()
	{
		for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(mDevice->get(), mRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(mDevice->get(), mImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(mDevice->get(), mInFlightFences[i], nullptr);
		}
		vkDestroyCommandPool(mDevice->get(), mCommandPool, nullptr);
	}


	VkCommandBuffer CommandBuffer::get(u32 i)
	{
		return mCommandBuffers[i];
	}

	void CommandBuffer::submit(std::shared_ptr<SwapChain> swapChain)
	{
		vkWaitForFences(mDevice->get(), 1, &mInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(mDevice->get(), swapChain->get(), UINT64_MAX, mImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(mDevice->get(), 1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		mImagesInFlight[imageIndex] = mInFlightFences[currentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(mDevice->get(), 1, &mInFlightFences[currentFrame]);

		printVkError(vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mInFlightFences[currentFrame]), "");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain->get() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(mDevice->getPresnetQueue(), &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		vkQueueWaitIdle(mDevice->getGraphicQueue());
	}

	VkCommandPool CommandBuffer::getCommandpool() const
	{
		return mCommandPool;
	}

	void CommandBuffer::createCommandPool()
	{
		// Command buffers are executed by submitting them on one of the device queues
		// like the graphicsand presentation queues we retrieved.Each command pool can
		// only allocate command buffers that are submitted on a single type of queue
		// We're going to record commands for drawing, which is why we've chosen the
		// graphics queue family.
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = mDevice->getQueueFamilyIndices().getGraphics();
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		printVkError(vkCreateCommandPool(mDevice->get(), &commandPoolCreateInfo, nullptr, &mCommandPool), "create command pool");

	}

	void CommandBuffer::allocateCommandBuffers()
	{
		// We can now start allocating command buffers and recording drawing commands in them.
		// Because one of the drawing commands involves binding the right VkFramebuffer, 
		// we'll actually have to record a command buffer for every image in the swap chain
		// once again. To that end, create a list of VkCommandBuffer objects as a class member.
		// Command buffers will be automatically freed when their command pool is destroyed, 
		// so we don't need an explicit cleanup.
		mCommandBuffers.resize(mRenderContext.swapChainImageCount);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = mCommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = static_cast<u32>(mCommandBuffers.size());

		printVkError(vkAllocateCommandBuffers(mDevice->get(), &commandBufferAllocateInfo, mCommandBuffers.data()), "allocate command buffers");
	}

	void CommandBuffer::beginRenderPass(u32 index, std::shared_ptr<Pipeline> pipeline, bool isPresent) {

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline->getRenderPass();
		if (isPresent) {
			renderPassInfo.framebuffer = pipeline->getFrameBuffer(index);
		} else {
			renderPassInfo.framebuffer = pipeline->getFrameBuffer();
		}
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{mRenderContext.width,mRenderContext.height};

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(mCommandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdSetViewport(mCommandBuffers[index], 0, 1, &pipeline->getViewport());
	}

	void CommandBuffer::endRenderPass(u32 index) {
		vkCmdEndRenderPass(mCommandBuffers[index]);
	}


	void CommandBuffer::createSyncObjects()
	{
		createSemaphores();
		createFences();
	}

	void CommandBuffer::createSemaphores()
	{
		mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			printVkError(vkCreateSemaphore(mDevice->get(), &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]), "create semaphore", logLevel::debug);
			printVkError(vkCreateSemaphore(mDevice->get(), &semaphoreCreateInfo, nullptr, &mRenderFinishedSemaphores[i]), "create semaphore", logLevel::debug);
		}
	}

	void CommandBuffer::createFences()
	{

		mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		mImagesInFlight.resize(mRenderContext.swapChainImageCount, VK_NULL_HANDLE);

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			printVkError(vkCreateFence(mDevice->get(), &fenceInfo, nullptr, &mInFlightFences[i]), "create fence", logLevel::debug);
		}
	}

	VkCommandBuffer CommandBuffer::beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(mDevice->get(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void CommandBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mDevice->getGraphicQueue());

		vkFreeCommandBuffers(mDevice->get(), mCommandPool, 1, &commandBuffer);
	}

	u32 CommandBuffer::present()
	{
		return 1;
	}

	void CommandBuffer::beginCommandRecording(u32 i)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// begin command buffer recording
		printVkError(vkBeginCommandBuffer(mCommandBuffers[i], &commandBufferBeginInfo), "");
	}

	void CommandBuffer::endCommandRecording(u32 i)
	{
		printVkError(vkEndCommandBuffer(mCommandBuffers[i]), "");
	}

}