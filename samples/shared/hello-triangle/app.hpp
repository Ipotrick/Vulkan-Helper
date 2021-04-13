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
	auto queue_family_properties = device.getQueueFamilyProperties();
	for (std::size_t i = 0; i < queue_family_properties.size(); ++i) {
		if (queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
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
	auto vert_shader_module = logical_device.createShaderModule({.codeSize = vert_spv.size() * sizeof(vert_spv[0]), .pCode = vert_spv.data()});
	auto frag_shader_module = logical_device.createShaderModule({.codeSize = frag_spv.size() * sizeof(frag_spv[0]), .pCode = frag_spv.data()});

	auto graphics_pipeline_layout = logical_device.createPipelineLayout({});

	std::array<vk::PipelineShaderStageCreateInfo, 2> pipeline_shaderstage_createinfos = {{
		{.stage = vk::ShaderStageFlagBits::eVertex, .module = vert_shader_module, .pName = "main"},
		{.stage = vk::ShaderStageFlagBits::eFragment, .module = frag_shader_module, .pName = "main"},
	}};
	vk::VertexInputBindingDescription vinput_binding_description(0, (std::uint32_t)vertexSize);
	std::array<vk::VertexInputAttributeDescription, 2> vinput_attribute_descriptions{{
		vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, 0),
		vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32A32Sfloat, sizeof(glm::vec2)),
	}};
	vk::PipelineVertexInputStateCreateInfo pipeline_vinputstate_createinfo{
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vinput_binding_description,
		.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vinput_attribute_descriptions.size()),
		.pVertexAttributeDescriptions = vinput_attribute_descriptions.data(),
	};
	vk::PipelineInputAssemblyStateCreateInfo pipeline_inputassemblystate_createinfo{.topology = vk::PrimitiveTopology::eTriangleList};
	vk::PipelineViewportStateCreateInfo pipeline_viewportstate_createinfo{.viewportCount = 1, .scissorCount = 1};

	vk::PipelineRasterizationStateCreateInfo pipeline_rasterizationstate_createinfo{
		.depthClampEnable = false,
		.rasterizerDiscardEnable = false,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise,
		.depthBiasEnable = false,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f,
	};
	vk::PipelineMultisampleStateCreateInfo pipeline_multisamplestate_createinfo{.rasterizationSamples = vk::SampleCountFlagBits::e1};
	vk::StencilOpState stencil_opstate(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
	vk::PipelineDepthStencilStateCreateInfo pipeline_depthstencilstate_createinfo{
		.depthTestEnable = true,
		.depthWriteEnable = true,
		.depthCompareOp = vk::CompareOp::eLessOrEqual,
		.depthBoundsTestEnable = false,
		.stencilTestEnable = false,
		.front = stencil_opstate,
		.back = stencil_opstate,
	};

	vk::ColorComponentFlags colorcomponent_flags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
												 vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	vk::PipelineColorBlendAttachmentState pipeline_colorblendattachment_state(
		false,
		vk::BlendFactor::eZero,
		vk::BlendFactor::eZero,
		vk::BlendOp::eAdd,
		vk::BlendFactor::eZero,
		vk::BlendFactor::eZero,
		vk::BlendOp::eAdd,
		colorcomponent_flags);
	vk::PipelineColorBlendStateCreateInfo pipeline_colorblendstate_createinfo{
		.logicOpEnable = false,
		.logicOp = vk::LogicOp::eNoOp,
		.attachmentCount = 1,
		.pAttachments = &pipeline_colorblendattachment_state,
		.blendConstants = {{1.0f, 1.0f, 1.0f, 1.0f}},
	};

	std::array<vk::DynamicState, 2> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	vk::PipelineDynamicStateCreateInfo pipeline_dynamicstate_createinfo{
		.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size()),
		.pDynamicStates = dynamic_states.data(),
	};

	vk::GraphicsPipelineCreateInfo graphics_pipeline_createinfo{
		.stageCount = static_cast<std::uint32_t>(pipeline_shaderstage_createinfos.size()),
		.pStages = pipeline_shaderstage_createinfos.data(),
		.pVertexInputState = &pipeline_vinputstate_createinfo,
		.pInputAssemblyState = &pipeline_inputassemblystate_createinfo,
		.pViewportState = &pipeline_viewportstate_createinfo,
		.pRasterizationState = &pipeline_rasterizationstate_createinfo,
		.pMultisampleState = &pipeline_multisamplestate_createinfo,
		.pDepthStencilState = &pipeline_depthstencilstate_createinfo,
		.pColorBlendState = &pipeline_colorblendstate_createinfo,
		.pDynamicState = &pipeline_dynamicstate_createinfo,
		.layout = graphics_pipeline_layout,
		.renderPass = renderpass,
	};

	auto [graphics_pipeline_creation_result, result_graphics_pipeline] = logical_device.createGraphicsPipeline(nullptr, graphics_pipeline_createinfo);
	if (graphics_pipeline_creation_result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create graphics pipeline");

	logical_device.destroyShaderModule(vert_shader_module);
	logical_device.destroyShaderModule(frag_shader_module);

	return std::make_pair(result_graphics_pipeline, graphics_pipeline_layout);
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
	vk::Instance vulkanInstance;
	vk::DebugUtilsMessengerEXT debugMessenger;
	vk::SurfaceKHR vulkanWindowSurface = nullptr;
	vk::PhysicalDevice selectedPhysicalDevice;
	vk::Device logicalDevice;
	QueueFamilyIndices queueIndices;
	vk::Queue graphicsQueue, presentQueue;
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
	vk::Buffer vertexbuffer;
	vk::DeviceMemory vertexbufferMemory;
	vk::SwapchainKHR swapchain;
	SwapchainDetails swapchainDetails;
	std::vector<vk::Image> swapchainImages;
	std::vector<vk::ImageView> swapchainImageViews;
	vk::Pipeline graphicsPipeline;
	vk::PipelineLayout graphicsPipelineLayout;
	std::vector<vk::Framebuffer> framebuffers;
	vk::RenderPass renderpass;
	vk::CommandPool graphicsCommandPool;
	vk::CommandBuffer graphicsCommandBuffer;

	int frameSizeX, frameSizeY;

	void initInstance() {
		vulkanInstance = vkh::createInstance({}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, "VK_KHR_surface", "VK_KHR_win32_surface"});
		// enable debug messenger for validation layers
		debugMessenger = vkh::createDebugMessenger(vulkanInstance);
	}

	void initDevice() {
		std::vector<const char *> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		// physical device selection
		selectedPhysicalDevice = vkh::selectPhysicalDevice(vulkanInstance, [&](vk::PhysicalDevice device) -> std::size_t {
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
		fmt::print("Selected Physical Device: {}\n", selectedPhysicalDeviceProperties.deviceName);

		queueIndices = findQueueFamilyIndices(selectedPhysicalDevice, vulkanWindowSurface);
		// logical device creation
		logicalDevice = vkh::createLogicalDevice(selectedPhysicalDevice, queueIndices.uniqueIndices(), deviceExtensions);
		// queue retrieval
		graphicsQueue = logicalDevice.getQueue(static_cast<std::uint32_t>(queueIndices.graphics.value()), 0);
		presentQueue = logicalDevice.getQueue(static_cast<std::uint32_t>(queueIndices.presentation.value()), 0);
	}

	void initVertexbuffer() {
		vertexbuffer = logicalDevice.createBuffer({.size = sizeof(vertexData), .usage = vk::BufferUsageFlagBits::eVertexBuffer});
		auto vertexbufferMemoryRequirements = logicalDevice.getBufferMemoryRequirements(vertexbuffer);
		auto vertexbufferMemoryTypeIndex = vkh::findMemoryTypeIndex(
			selectedPhysicalDevice.getMemoryProperties(),
			vertexbufferMemoryRequirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vertexbufferMemory = logicalDevice.allocateMemory({
			.allocationSize = vertexbufferMemoryRequirements.size,
			.memoryTypeIndex = vertexbufferMemoryTypeIndex,
		});
		logicalDevice.bindBufferMemory(vertexbuffer, vertexbufferMemory, 0);
		auto vertexbufferDeviceDataPtr = static_cast<std::uint8_t *>(logicalDevice.mapMemory(vertexbufferMemory, 0, vertexbufferMemoryRequirements.size));
		std::memcpy(vertexbufferDeviceDataPtr, vertexData, sizeof(vertexData));
		logicalDevice.unmapMemory(vertexbufferMemory);
	}

	void initSwapchain() {
		auto createSwapchainResult = createSwapchain(selectedPhysicalDevice, logicalDevice, queueIndices, vulkanWindowSurface, frameSizeX, frameSizeY);
		swapchain = std::get<0>(createSwapchainResult);
		swapchainDetails = std::get<1>(createSwapchainResult);
		swapchainImages = logicalDevice.getSwapchainImagesKHR(swapchain);
		swapchainImageViews = createSwapchainImageViews(swapchainImages, swapchainDetails, logicalDevice);
	}

	void initFramebuffers() {
		framebuffers = createFramebuffers(logicalDevice, swapchainImageViews, renderpass, frameSizeX, frameSizeY);
	}

	void initPipeline() {
		renderpass = createRenderpass(logicalDevice, swapchainDetails.format);
		auto createGraphicsPipelineResult = createGraphicsPipeline(logicalDevice, renderpass, sizeof(TriangleVertex));
		graphicsPipeline = std::get<0>(createGraphicsPipelineResult);
		graphicsPipelineLayout = std::get<1>(createGraphicsPipelineResult);

		framebuffers = createFramebuffers(logicalDevice, swapchainImageViews, renderpass, frameSizeX, frameSizeY);

		graphicsCommandPool = logicalDevice.createCommandPool({
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = static_cast<std::uint32_t>(queueIndices.graphics.value()),
		});
		graphicsCommandBuffer = logicalDevice.allocateCommandBuffers({.commandPool = graphicsCommandPool, .commandBufferCount = 1}).front();
	}

	void deinit() {
		deinitSwapchain();

		if (vulkanInstance) {
			if (logicalDevice) {
				if (graphicsCommandPool)
					logicalDevice.destroyCommandPool(graphicsCommandPool);

				if (graphicsPipelineLayout)
					logicalDevice.destroyPipelineLayout(graphicsPipelineLayout);
				if (graphicsPipeline)
					logicalDevice.destroyPipeline(graphicsPipeline);
				if (renderpass)
					logicalDevice.destroyRenderPass(renderpass);

				if (vertexbufferMemory)
					logicalDevice.freeMemory(vertexbufferMemory);
				if (vertexbuffer)
					logicalDevice.destroyBuffer(vertexbuffer);
				logicalDevice.destroy();
			}

			if (debugMessenger)
				vulkanInstance.destroyDebugUtilsMessengerEXT(debugMessenger);
			vulkanInstance.destroy();
		}
	}

	void deinitSwapchain() {
		if (vulkanInstance && logicalDevice) {
			if (graphicsCommandBuffer)
				logicalDevice.freeCommandBuffers(graphicsCommandPool, graphicsCommandBuffer);
			for (const auto &framebuffer : framebuffers)
				if (framebuffer)
					logicalDevice.destroyFramebuffer(framebuffer);
			framebuffers.clear();
			for (auto &view : swapchainImageViews)
				if (view)
					logicalDevice.destroyImageView(view);
			swapchainImageViews.clear();
			if (swapchain)
				logicalDevice.destroySwapchainKHR(swapchain);
			if (vulkanWindowSurface)
				vulkanInstance.destroySurfaceKHR(vulkanWindowSurface);
			vulkanWindowSurface = nullptr;
		}
	}

	void draw() {
		// prepare frame for drawing
		auto imageAcquiredSemaphore = logicalDevice.createSemaphore({});
		auto currentFramebuffer = logicalDevice.acquireNextImageKHR(swapchain, 100000000 /* fence timeout */, imageAcquiredSemaphore, nullptr);
		if (currentFramebuffer.result == vk::Result::eErrorOutOfDateKHR) {
			throw std::runtime_error("Swapchain OUT OF DATE");
		} else {
			if (currentFramebuffer.result != vk::Result::eSuccess)
				throw std::runtime_error("failed to acquire next swapchain image");
			if (currentFramebuffer.value >= framebuffers.size())
				throw std::runtime_error("grabbed an invalid framebuffer index");
		}
		// prepare frame clear values
		std::array<vk::ClearValue, 2> clearValues;
		clearValues[0].color = vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}}));
		clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
		// set up command buffer
		graphicsCommandBuffer.begin(vk::CommandBufferBeginInfo{});
		graphicsCommandBuffer.beginRenderPass(
			{
				.renderPass = renderpass,
				.framebuffer = framebuffers[currentFramebuffer.value],
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
		auto drawFence = logicalDevice.createFence({});
		graphicsQueue.submit({{.commandBufferCount = 1, .pCommandBuffers = &graphicsCommandBuffer}}, drawFence);
		while (vk::Result::eTimeout == logicalDevice.waitForFences(drawFence, VK_TRUE, 100000000 /* fence timeout */)) {
			// wait
		}

		auto presentResult = presentQueue.presentKHR({
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &currentFramebuffer.value,
		});

		if (presentResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to execute present queue");

		logicalDevice.destroyFence(drawFence);
		logicalDevice.destroySemaphore(imageAcquiredSemaphore);
	}
};
