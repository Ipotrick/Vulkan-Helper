#pragma once

#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

#include "../load-shader.hpp"
#include <glm/glm.hpp>

#include <utility>

struct QueueFamilyIndices {
	std::optional<std::size_t> graphics, presentation;
	bool complete() {
		return graphics.has_value() && presentation.has_value();
	}
	std::set<std::size_t> uniqueIndices() const {
		return {graphics.value(), presentation.value()};
	}
};

QueueFamilyIndices findQueueFamilyIndices(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
	QueueFamilyIndices resultIndices;
	auto queueFamilyProperties = device.getQueueFamilyProperties();
	for (std::size_t i = 0; i < queueFamilyProperties.size(); ++i) {
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
			resultIndices.graphics = i;
		if (device.getSurfaceSupportKHR(static_cast<std::uint32_t>(i), surface))
			resultIndices.presentation = i;
	}
	return resultIndices;
}

struct SwapchainDetails {
	vk::Format format;
	vk::PresentModeKHR presentMode;
	vk::Extent2D extent;
};

vk::Format chooseSwapchainFormat(const std::vector<vk::SurfaceFormatKHR> &availableSurfaceFormats) {
	for (const auto &surfaceFormat : availableSurfaceFormats) {
		if (surfaceFormat.format == vk::Format::eB8G8R8A8Unorm &&
			surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return surfaceFormat.format;
	}
	return availableSurfaceFormats[0].format;
}
vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR> &availableSurfacePresentModes) {
	// if vsync, change!
	return vk::PresentModeKHR::eFifo;
}
vk::Extent2D chooseSwapchainExtent(vk::SurfaceCapabilitiesKHR surfaceCapabilities, std::uint32_t desiredSizeX, std::uint32_t desiredSizeY) {
	vk::Extent2D resultExtent;
	if (surfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max()) {
		resultExtent.width = std::min(std::max(desiredSizeX, surfaceCapabilities.minImageExtent.width), surfaceCapabilities.maxImageExtent.width);
		resultExtent.height = std::min(std::max(desiredSizeY, surfaceCapabilities.minImageExtent.height), surfaceCapabilities.maxImageExtent.height);
	} else {
		resultExtent = surfaceCapabilities.currentExtent;
	}
	return resultExtent;
}

std::pair<vk::SwapchainKHR, SwapchainDetails> createSwapchain(vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, const QueueFamilyIndices queueIndices, vk::SurfaceKHR surface, int sizeX, int sizeY) {
	auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	SwapchainDetails details{
		.format = chooseSwapchainFormat(surfaceFormats),
		.presentMode = chooseSwapchainPresentMode(presentModes),
		.extent = chooseSwapchainExtent(surfaceCapabilities, sizeX, sizeY),
	};

	vk::SurfaceTransformFlagBitsKHR preTransform =
		(surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
			? vk::SurfaceTransformFlagBitsKHR::eIdentity
			: surfaceCapabilities.currentTransform;

	vk::CompositeAlphaFlagBitsKHR compositeAlpha =
		(surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
			? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
		: (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
			? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
		: (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
			? vk::CompositeAlphaFlagBitsKHR::eInherit
			: vk::CompositeAlphaFlagBitsKHR::eOpaque;

	std::uint32_t queueFamilyIndices[2] = {
		static_cast<std::uint32_t>(queueIndices.graphics.value()),
		static_cast<std::uint32_t>(queueIndices.presentation.value()),
	};
	vk::SwapchainCreateInfoKHR swapchainCreateinfo{
		.flags = {},
		.surface = surface,
		.minImageCount = surfaceCapabilities.minImageCount,
		.imageFormat = details.format,
		.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
		.imageExtent = details.extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.pQueueFamilyIndices = queueFamilyIndices,
		.preTransform = preTransform,
		.compositeAlpha = compositeAlpha,
		.presentMode = details.presentMode,
		.clipped = true,
	};
	if (queueIndices.graphics.value() != queueIndices.presentation.value()) {
		swapchainCreateinfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapchainCreateinfo.queueFamilyIndexCount = 2;
	} else {
		swapchainCreateinfo.queueFamilyIndexCount = 1;
	}
	return std::make_pair(logicalDevice.createSwapchainKHR(swapchainCreateinfo), details);
}

std::vector<vk::ImageView> createSwapchainImageViews(const std::vector<vk::Image> &swapchainImages, const SwapchainDetails &swapchainDetails, vk::Device logicalDevice) {
	std::vector<vk::ImageView> resultantViews;
	resultantViews.reserve(swapchainImages.size());
	vk::ImageSubresourceRange swapchainSubresourceRange{
		.aspectMask = vk::ImageAspectFlagBits::eColor,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1,
	};
	for (auto &image : swapchainImages) {
		resultantViews.push_back(logicalDevice.createImageView({
			.image = image,
			.viewType = vk::ImageViewType::e2D,
			.format = swapchainDetails.format,
			.components = {
				vk::ComponentSwizzle::eR,
				vk::ComponentSwizzle::eG,
				vk::ComponentSwizzle::eB,
				vk::ComponentSwizzle::eA,
			},
			.subresourceRange = swapchainSubresourceRange,
		}));
	}
	return resultantViews;
}

vk::RenderPass createRenderpass(vk::Device logicalDevice, vk::Format swapchainFormat) {
	std::array renderpass_attachment_descriptions{
		vk::AttachmentDescription{
			.format = swapchainFormat,
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::ePresentSrcKHR,
		}};
	vk::AttachmentReference color_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::SubpassDescription subpass{
		.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_reference,
	};
	return logicalDevice.createRenderPass({
		.attachmentCount = static_cast<std::uint32_t>(renderpass_attachment_descriptions.size()),
		.pAttachments = renderpass_attachment_descriptions.data(),
		.subpassCount = 1,
		.pSubpasses = &subpass,
	});
}

std::pair<vk::Pipeline, vk::PipelineLayout> createGraphicsPipeline(vk::Device logical_device, vk::RenderPass renderpass, std::size_t vertexSize) {
	glslang::InitializeProcess();
	auto vert_spv = loadGlslShaderToSpv("samples/shared/hello-triangle/main.vert");
	auto frag_spv = loadGlslShaderToSpv("samples/shared/hello-triangle/main.frag");
	glslang::FinalizeProcess();
	auto vert_shader_module = logical_device.createShaderModuleUnique({.codeSize = vert_spv.size() * sizeof(vert_spv[0]), .pCode = vert_spv.data()});
	auto frag_shader_module = logical_device.createShaderModuleUnique({.codeSize = frag_spv.size() * sizeof(frag_spv[0]), .pCode = frag_spv.data()});

	vkh::VertexDiscriptionBuilder vertBuilder;
	auto vertDesc = vertBuilder
		.beginBinding((std::uint32_t)vertexSize)
		.addAttribute(vk::Format::eR32G32Sfloat)
		.addAttribute(vk::Format::eR32G32B32A32Sfloat)
		.build();
	

	vkh::GraphicsPipelineBuilder pipelineBuilder;
	pipelineBuilder
		.setVertexInput(vertDesc.makePipelineVertexInputStateCreateInfo())
		.addShaderStage({.stage = vk::ShaderStageFlagBits::eVertex, .module = *vert_shader_module, .pName = "main"})
		.addShaderStage({.stage = vk::ShaderStageFlagBits::eFragment, .module = *frag_shader_module, .pName = "main"});
	vkh::Pipeline pipe = pipelineBuilder.build(logical_device, renderpass);
	return std::make_pair(pipe.pipeline.release(), pipe.layout.release());
}

std::vector<vk::Framebuffer> createFramebuffers(vk::Device logicalDevice, const std::vector<vk::ImageView> &swapchainImageViews, vk::RenderPass renderpass, int sizeX, int sizeY) {
	std::array<vk::ImageView, 1> attachments{{{}}};
	std::vector<vk::Framebuffer> framebuffers;
	framebuffers.reserve(swapchainImageViews.size());
	vk::FramebufferCreateInfo framebuffer_createinfo{
		.renderPass = renderpass,
		.attachmentCount = static_cast<std::uint32_t>(attachments.size()),
		.pAttachments = attachments.data(),
		.width = static_cast<std::uint32_t>(sizeX),
		.height = static_cast<std::uint32_t>(sizeY),
		.layers = 1,
	};
	for (const auto &image_view : swapchainImageViews) {
		attachments[0] = image_view;
		framebuffers.push_back(logicalDevice.createFramebuffer(framebuffer_createinfo));
	}
	return framebuffers;
}

struct HelloTriangle {
	vk::UniqueInstance vulkanInstance;
	vk::UniqueDebugUtilsMessengerEXT debugMessenger;
	vk::SurfaceKHR vulkanWindowSurface = nullptr;

	vk::PhysicalDevice selectedPhysicalDevice;
	vk::UniqueDevice logicalDevice;
	vk::Queue graphicsQueue, presentQueue;

	vk::Buffer vertexbuffer;
	vk::DeviceMemory vertexbufferMemory;

	vk::SwapchainKHR swapchain;
	SwapchainDetails swapchainDetails;
	std::vector<vk::Image> swapchainImages;
	std::vector<vk::ImageView> swapchainImageViews;
	std::vector<vk::Framebuffer> swapchainFramebuffers;

	vk::RenderPass renderpass;
	vk::PipelineLayout graphicsPipelineLayout;
	vk::Pipeline graphicsPipeline;

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	std::vector<vk::Semaphore> imagesAcquired;
	std::vector<vk::Semaphore> drawsFinished;
	std::vector<vk::Fence> imagesInFlight;
	std::vector<vk::Fence> inFlight;
	std::size_t currentFrame = 0;
	const std::size_t MAX_IMAGES_IN_FLIGHT = 2;
	const std::size_t FENCE_TIMEOUT = UINT64_MAX;

	struct TriangleVertex {
		glm::vec2 pos;
		glm::vec4 col;
	};
	TriangleVertex vertexData[3]{
		// clang-format off
        {.pos = { 0.5f,  0.5f}, .col = {1, 0, 0, 1}}, // bottom right (red)
        {.pos = {-0.5f,  0.5f}, .col = {0, 1, 0, 1}}, // bottom left  (green)
        {.pos = { 0.0f, -0.5f}, .col = {0, 0, 1, 1}}, // top middle   (blue)
		// clang-format on
	};

	int frameSizeX, frameSizeY;

	void initInstance() {
		vulkanInstance = vkh::createInstance({}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, "VK_KHR_surface", "VK_KHR_win32_surface"});
		debugMessenger = vkh::createDebugMessenger(*vulkanInstance);
	}

	void initRenderContext() {
		initDevice();
		initVertexbuffer();
		initSwapchain();
		initPipeline();
		initFramebuffers();
		initSyncObjects();
	}

	void initDevice() {
		std::vector<const char *> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		// physical device selection
		selectedPhysicalDevice = vkh::selectPhysicalDevice(*vulkanInstance, [&](vk::PhysicalDevice device) -> std::size_t {
			std::size_t score = 0;
			auto indices = findQueueFamilyIndices(device, vulkanWindowSurface);
			auto deviceProperties = device.getProperties();
			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
				score += 1000;
			score += deviceProperties.limits.maxUniformBufferRange;
			score += deviceProperties.limits.maxStorageBufferRange;
			auto deviceFeatures = device.getFeatures();
			if (!deviceFeatures.geometryShader)
				return 0;
			if (!indices.complete())
				return 0;
			std::set<std::string> requiredExtensionsSet(deviceExtensions.begin(), deviceExtensions.end());
			auto deviceExtensionProperties = device.enumerateDeviceExtensionProperties();
			for (const auto &availableExtension : deviceExtensionProperties)
				requiredExtensionsSet.erase(availableExtension.extensionName);
			if (!requiredExtensionsSet.empty())
				return 0;
			auto surface_formats = device.getSurfaceFormatsKHR(vulkanWindowSurface);
			auto present_modes = device.getSurfacePresentModesKHR(vulkanWindowSurface);
			if (present_modes.empty() || surface_formats.empty())
				return 0;
			return score;
		});

		auto selectedPhysicalDeviceProperties = selectedPhysicalDevice.getProperties();

		auto queueIndices = findQueueFamilyIndices(selectedPhysicalDevice, vulkanWindowSurface);
		// logical device creation
		logicalDevice = vkh::createLogicalDevice(selectedPhysicalDevice, queueIndices.uniqueIndices(), deviceExtensions);
		// queue retrieval
		graphicsQueue = logicalDevice->getQueue(static_cast<std::uint32_t>(queueIndices.graphics.value()), 0);
		presentQueue = logicalDevice->getQueue(static_cast<std::uint32_t>(queueIndices.presentation.value()), 0);
	}

	void initVertexbuffer() {
		vertexbuffer = logicalDevice->createBuffer({.size = sizeof(vertexData), .usage = vk::BufferUsageFlagBits::eVertexBuffer});
		auto vertexbufferMemoryRequirements = logicalDevice->getBufferMemoryRequirements(vertexbuffer);
		auto vertexbufferMemoryTypeIndex = vkh::findMemoryTypeIndex(
			selectedPhysicalDevice.getMemoryProperties(),
			vertexbufferMemoryRequirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vertexbufferMemory = logicalDevice->allocateMemory({
			.allocationSize = vertexbufferMemoryRequirements.size,
			.memoryTypeIndex = vertexbufferMemoryTypeIndex,
		});
		logicalDevice->bindBufferMemory(vertexbuffer, vertexbufferMemory, 0);
		auto vertexbufferDeviceDataPtr = static_cast<std::uint8_t *>(logicalDevice->mapMemory(vertexbufferMemory, 0, vertexbufferMemoryRequirements.size));
		std::memcpy(vertexbufferDeviceDataPtr, vertexData, sizeof(vertexData));
		logicalDevice->unmapMemory(vertexbufferMemory);
	}

	void initSwapchain() {
		auto queueIndices = findQueueFamilyIndices(selectedPhysicalDevice, vulkanWindowSurface);
		auto createSwapchainResult = createSwapchain(selectedPhysicalDevice, *logicalDevice, queueIndices, vulkanWindowSurface, frameSizeX, frameSizeY);
		swapchain = std::get<0>(createSwapchainResult);
		swapchainDetails = std::get<1>(createSwapchainResult);
		swapchainImages = logicalDevice->getSwapchainImagesKHR(swapchain);
		swapchainImageViews = createSwapchainImageViews(swapchainImages, swapchainDetails, *logicalDevice);
	}

	void initPipeline() {
		renderpass = createRenderpass(*logicalDevice, swapchainDetails.format);
		auto createGraphicsPipelineResult = createGraphicsPipeline(*logicalDevice, renderpass, sizeof(TriangleVertex));
		graphicsPipeline = std::get<0>(createGraphicsPipelineResult);
		graphicsPipelineLayout = std::get<1>(createGraphicsPipelineResult);

		initFramebuffers();

		auto queueIndices = findQueueFamilyIndices(selectedPhysicalDevice, vulkanWindowSurface);
		commandPool = logicalDevice->createCommandPool({
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = static_cast<std::uint32_t>(queueIndices.graphics.value()),
		});

		commandBuffers = logicalDevice->allocateCommandBuffers({.commandPool = commandPool, .commandBufferCount = (std::uint32_t)swapchainFramebuffers.size()});
	}

	void initSyncObjects() {
		imagesAcquired.resize(MAX_IMAGES_IN_FLIGHT);
		drawsFinished.resize(MAX_IMAGES_IN_FLIGHT);
		inFlight.resize(MAX_IMAGES_IN_FLIGHT);
		imagesInFlight.resize(swapchainImages.size(), nullptr);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_IMAGES_IN_FLIGHT; i++) {
			imagesAcquired[i] = logicalDevice->createSemaphore({});
			drawsFinished[i] = logicalDevice->createSemaphore({});
			inFlight[i] = logicalDevice->createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
		}
	}

	void initFramebuffers() {
		swapchainFramebuffers = createFramebuffers(*logicalDevice, swapchainImageViews, renderpass, frameSizeX, frameSizeY);
	}

	void deinit() {
		if (vulkanInstance) {
			if (logicalDevice) {
				logicalDevice->waitIdle();
				deinitSwapchain();

				for (auto elem : inFlight) {
					if (elem) {
						while (vk::Result::eTimeout == logicalDevice->waitForFences(elem, VK_TRUE, FENCE_TIMEOUT)) {
						}
						logicalDevice->destroyFence(elem);
					}
				}
				for (auto elem : imagesAcquired)
					if (elem)
						logicalDevice->destroySemaphore(elem);
				for (auto elem : drawsFinished)
					if (elem)
						logicalDevice->destroySemaphore(elem);

				if (commandPool)
					logicalDevice->destroyCommandPool(commandPool);

				if (graphicsPipelineLayout)
					logicalDevice->destroyPipelineLayout(graphicsPipelineLayout);
				if (graphicsPipeline)
					logicalDevice->destroyPipeline(graphicsPipeline);
				if (renderpass)
					logicalDevice->destroyRenderPass(renderpass);

				if (vertexbufferMemory)
					logicalDevice->freeMemory(vertexbufferMemory);
				if (vertexbuffer)
					logicalDevice->destroyBuffer(vertexbuffer);
			}
		}
	}

	void deinitSwapchain() {
		if (vulkanInstance && logicalDevice) {
            logicalDevice->waitIdle();
			for (auto elem : commandBuffers)
				if (elem)
					logicalDevice->freeCommandBuffers(commandPool, elem);
			for (const auto &elem : swapchainFramebuffers)
				if (elem)
					logicalDevice->destroyFramebuffer(elem);
			swapchainFramebuffers.clear();
			for (auto &view : swapchainImageViews)
				if (view)
					logicalDevice->destroyImageView(view);
			swapchainImageViews.clear();
			if (swapchain)
				logicalDevice->destroySwapchainKHR(swapchain);
			if (vulkanWindowSurface)
				vulkanInstance->destroySurfaceKHR(vulkanWindowSurface);
			vulkanWindowSurface = nullptr;
		}
	}

	void draw() {
		while (vk::Result::eTimeout == logicalDevice->waitForFences(inFlight[currentFrame], VK_TRUE, FENCE_TIMEOUT)) {
		}
		// prepare frame for drawing
		auto imageIndex = logicalDevice->acquireNextImageKHR(swapchain, FENCE_TIMEOUT, imagesAcquired[currentFrame], nullptr);
		if (imageIndex.result == vk::Result::eErrorOutOfDateKHR) {
			throw std::runtime_error("Swapchain OUT OF DATE");
		} else {
			if (imageIndex.result != vk::Result::eSuccess)
				throw std::runtime_error("failed to acquire next swapchain image");
			if (imageIndex.value >= swapchainFramebuffers.size())
				throw std::runtime_error("grabbed an invalid framebuffer index");
		}
		// prepare frame clear values
		std::array<vk::ClearValue, 2> clearValues;
		clearValues[0].color = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 1.0f}}));
		clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

		if (imagesInFlight[imageIndex.value]) {
			while (vk::Result::eTimeout == logicalDevice->waitForFences(imagesInFlight[imageIndex.value], VK_TRUE, FENCE_TIMEOUT)) {
			}
		}
		imagesInFlight[imageIndex.value] = inFlight[currentFrame];

		// set up command buffer
		auto &graphicsCommandBuffer = commandBuffers[imageIndex.value];
		graphicsCommandBuffer.begin(vk::CommandBufferBeginInfo{});
		graphicsCommandBuffer.beginRenderPass(
			{
				.renderPass = renderpass,
				.framebuffer = swapchainFramebuffers[imageIndex.value],
				.renderArea = vk::Rect2D(vk::Offset2D(0, 0), swapchainDetails.extent),
				.clearValueCount = static_cast<std::uint32_t>(clearValues.size()),
				.pClearValues = clearValues.data(),
			},
			vk::SubpassContents::eInline);
		graphicsCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
		graphicsCommandBuffer.bindVertexBuffers(0, vertexbuffer, {0});

		graphicsCommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainDetails.extent.width), static_cast<float>(swapchainDetails.extent.height), 0.0f, 1.0f));
		graphicsCommandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), {swapchainDetails.extent.width, swapchainDetails.extent.height}));
		graphicsCommandBuffer.draw(1 * 3, 1, 0, 0);

		graphicsCommandBuffer.endRenderPass();
		graphicsCommandBuffer.end();

		// submit command buffer to graphics queue
		vk::Semaphore waitSemaphores[] = {imagesAcquired[currentFrame]};
		vk::Semaphore signalSemaphores[] = {drawsFinished[currentFrame]};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		auto resetFenceResult = logicalDevice->resetFences(1, &inFlight[currentFrame]);
		graphicsQueue.submit(
			{{
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = waitSemaphores,
				.pWaitDstStageMask = waitStages,
				.commandBufferCount = 1,
				.pCommandBuffers = &commandBuffers[imageIndex.value],
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = signalSemaphores,
			}},
			inFlight[currentFrame]);

		auto presentResult = presentQueue.presentKHR({
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = signalSemaphores,
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &imageIndex.value,
		});

		if (presentResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to execute present queue");

		currentFrame = (currentFrame + 1) % MAX_IMAGES_IN_FLIGHT;
	}
};
