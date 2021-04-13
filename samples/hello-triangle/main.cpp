#include "../shared/window.hpp"

#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

#include <glm/glm.hpp>

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Include/ResourceLimits.h>

static constexpr TBuiltInResource get_resource() {
	return TBuiltInResource{
		.maxLights = 32,
		.maxClipPlanes = 6,
		.maxTextureUnits = 32,
		.maxTextureCoords = 32,
		.maxVertexAttribs = 64,
		.maxVertexUniformComponents = 4096,
		.maxVaryingFloats = 64,
		.maxVertexTextureImageUnits = 32,
		.maxCombinedTextureImageUnits = 80,
		.maxTextureImageUnits = 32,
		.maxFragmentUniformComponents = 4096,
		.maxDrawBuffers = 32,
		.maxVertexUniformVectors = 128,
		.maxVaryingVectors = 8,
		.maxFragmentUniformVectors = 16,
		.maxVertexOutputVectors = 16,
		.maxFragmentInputVectors = 15,
		.minProgramTexelOffset = -8,
		.maxProgramTexelOffset = 7,
		.maxClipDistances = 8,
		.maxComputeWorkGroupCountX = 65535,
		.maxComputeWorkGroupCountY = 65535,
		.maxComputeWorkGroupCountZ = 65535,
		.maxComputeWorkGroupSizeX = 1024,
		.maxComputeWorkGroupSizeY = 1024,
		.maxComputeWorkGroupSizeZ = 64,
		.maxComputeUniformComponents = 1024,
		.maxComputeTextureImageUnits = 16,
		.maxComputeImageUniforms = 8,
		.maxComputeAtomicCounters = 8,
		.maxComputeAtomicCounterBuffers = 1,
		.maxVaryingComponents = 60,
		.maxVertexOutputComponents = 64,
		.maxGeometryInputComponents = 64,
		.maxGeometryOutputComponents = 128,
		.maxFragmentInputComponents = 128,
		.maxImageUnits = 8,
		.maxCombinedImageUnitsAndFragmentOutputs = 8,
		.maxCombinedShaderOutputResources = 8,
		.maxImageSamples = 0,
		.maxVertexImageUniforms = 0,
		.maxTessControlImageUniforms = 0,
		.maxTessEvaluationImageUniforms = 0,
		.maxGeometryImageUniforms = 0,
		.maxFragmentImageUniforms = 8,
		.maxCombinedImageUniforms = 8,
		.maxGeometryTextureImageUnits = 16,
		.maxGeometryOutputVertices = 256,
		.maxGeometryTotalOutputComponents = 1024,
		.maxGeometryUniformComponents = 1024,
		.maxGeometryVaryingComponents = 64,
		.maxTessControlInputComponents = 128,
		.maxTessControlOutputComponents = 128,
		.maxTessControlTextureImageUnits = 16,
		.maxTessControlUniformComponents = 1024,
		.maxTessControlTotalOutputComponents = 4096,
		.maxTessEvaluationInputComponents = 128,
		.maxTessEvaluationOutputComponents = 128,
		.maxTessEvaluationTextureImageUnits = 16,
		.maxTessEvaluationUniformComponents = 1024,
		.maxTessPatchComponents = 120,
		.maxPatchVertices = 32,
		.maxTessGenLevel = 64,
		.maxViewports = 16,
		.maxVertexAtomicCounters = 0,
		.maxTessControlAtomicCounters = 0,
		.maxTessEvaluationAtomicCounters = 0,
		.maxGeometryAtomicCounters = 0,
		.maxFragmentAtomicCounters = 8,
		.maxCombinedAtomicCounters = 8,
		.maxAtomicCounterBindings = 1,
		.maxVertexAtomicCounterBuffers = 0,
		.maxTessControlAtomicCounterBuffers = 0,
		.maxTessEvaluationAtomicCounterBuffers = 0,
		.maxGeometryAtomicCounterBuffers = 0,
		.maxFragmentAtomicCounterBuffers = 1,
		.maxCombinedAtomicCounterBuffers = 1,
		.maxAtomicCounterBufferSize = 16384,
		.maxTransformFeedbackBuffers = 4,
		.maxTransformFeedbackInterleavedComponents = 64,
		.maxCullDistances = 8,
		.maxCombinedClipAndCullDistances = 8,
		.maxSamples = 4,
		.maxMeshOutputVerticesNV = 256,
		.maxMeshOutputPrimitivesNV = 512,
		.maxMeshWorkGroupSizeX_NV = 32,
		.maxMeshWorkGroupSizeY_NV = 1,
		.maxMeshWorkGroupSizeZ_NV = 1,
		.maxTaskWorkGroupSizeX_NV = 32,
		.maxTaskWorkGroupSizeY_NV = 1,
		.maxTaskWorkGroupSizeZ_NV = 1,
		.maxMeshViewCountNV = 4,
		.limits{
			.nonInductiveForLoops = 1,
			.whileLoops = 1,
			.doWhileLoops = 1,
			.generalUniformIndexing = 1,
			.generalAttributeMatrixVectorIndexing = 1,
			.generalVaryingIndexing = 1,
			.generalSamplerIndexing = 1,
			.generalVariableIndexing = 1,
			.generalConstantMatrixVectorIndexing = 1,
		},
	};
}
static std::vector<std::uint32_t> glsl_to_spv(vk::ShaderStageFlagBits shader_stage, const std::string &glsl_code) {
	auto translate_shader_stage = [](vk::ShaderStageFlagBits stage) -> EShLanguage {
		switch (stage) {
		case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
		case vk::ShaderStageFlagBits::eTessellationControl: return EShLangTessControl;
		case vk::ShaderStageFlagBits::eTessellationEvaluation: return EShLangTessEvaluation;
		case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
		case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
		case vk::ShaderStageFlagBits::eCompute: return EShLangCompute;
		case vk::ShaderStageFlagBits::eRaygenNV: return EShLangRayGenNV;
		case vk::ShaderStageFlagBits::eAnyHitNV: return EShLangAnyHitNV;
		case vk::ShaderStageFlagBits::eClosestHitNV: return EShLangClosestHitNV;
		case vk::ShaderStageFlagBits::eMissNV: return EShLangMissNV;
		case vk::ShaderStageFlagBits::eIntersectionNV: return EShLangIntersectNV;
		case vk::ShaderStageFlagBits::eCallableNV: return EShLangCallableNV;
		case vk::ShaderStageFlagBits::eTaskNV: return EShLangTaskNV;
		case vk::ShaderStageFlagBits::eMeshNV: return EShLangMeshNV;
		default: throw std::runtime_error("Unknown shader stage");
		}
	};
	auto stage = translate_shader_stage(shader_stage);
	const char *shader_strings[] = {glsl_code.c_str()};
	glslang::TShader shader(stage);
	shader.setStrings(shader_strings, 1);
	auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
	TBuiltInResource resource = get_resource();
	if (!shader.parse(&resource, 100, false, messages))
		throw std::runtime_error(fmt::format("{}\n{}", shader.getInfoLog(), shader.getInfoDebugLog()).c_str());
	glslang::TProgram program;
	program.addShader(&shader);
	if (!program.link(messages))
		throw std::runtime_error(fmt::format("{}\n{}", shader.getInfoLog(), shader.getInfoDebugLog()).c_str());
	std::vector<std::uint32_t> spv_result;
	glslang::GlslangToSpv(*program.getIntermediate(stage), spv_result);
	return spv_result;
}
static std::string text_file_to_string(const std::filesystem::path &filepath) {
	std::ifstream source_file(filepath);
	std::string source_str;
	if (source_file.is_open()) {
		source_file.seekg(0, std::ios::end);
		source_str.reserve(source_file.tellg());
		source_file.seekg(0, std::ios::beg);
		source_str.assign(
			std::istreambuf_iterator<char>(source_file),
			std::istreambuf_iterator<char>());
		source_file.close();
	} else {
		auto message = fmt::format("Failed to open shader file {} in {}", filepath.string(), std::filesystem::current_path().string());
		throw std::runtime_error(message.c_str());
	}
	return source_str;
}
static std::vector<std::uint32_t> load_glsl_shader_to_spv(const std::filesystem::path &filepath) {
	auto source_string = text_file_to_string(filepath);
	vk::ShaderStageFlagBits shader_stage;

	if (filepath.extension() == ".vert") {
		shader_stage = vk::ShaderStageFlagBits::eVertex;
	} else if (filepath.extension() == ".frag") {
		shader_stage = vk::ShaderStageFlagBits::eFragment;
	} else {
		throw std::runtime_error(fmt::format("Attempting to load a currently unsupported shader file stage '{}'", filepath.extension().string()).c_str());
	}

	return glsl_to_spv(shader_stage, source_string);
}

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
	auto vert_spv = load_glsl_shader_to_spv("samples/hello-triangle/main.vert");
	auto frag_spv = load_glsl_shader_to_spv("samples/hello-triangle/main.frag");
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

int main() try {
	// instance creation
	auto vulkanInstance = vkh::createInstance({}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, "VK_KHR_surface", "VK_KHR_win32_surface"});
	// enable debug messenger for validation layers
	auto debugMessenger = vkh::createDebugMessenger(vulkanInstance);
	// create window
	int sizeX = 800, sizeY = 600;
	Window window({.frameSizeX = sizeX, .frameSizeY = sizeY, .title = "Hello Triangle"});
	// create vulkan surface
	auto vulkanWindowSurface = window.createVulkanSurface(vulkanInstance);

	// device creation
	std::vector<const char *> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	// physical device selection
	auto selectedPhysicalDevice = vkh::selectPhysicalDevice(vulkanInstance, [&](vk::PhysicalDevice device) -> std::size_t {
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

	auto queueIndices = findQueueFamilyIndices(selectedPhysicalDevice, vulkanWindowSurface);
	// logical device creation
	auto logicalDevice = vkh::createLogicalDevice(selectedPhysicalDevice, queueIndices.uniqueIndices(), deviceExtensions);
	// queue retrieval
	auto graphicsQueue = logicalDevice.getQueue(static_cast<std::uint32_t>(queueIndices.graphics.value()), 0);
	auto presentQueue = logicalDevice.getQueue(static_cast<std::uint32_t>(queueIndices.presentation.value()), 0);
	// swapchain creation

	// Triangle Data
	struct TriangleVertex {
		glm::vec2 pos;
        glm::vec4 col;
	};
	TriangleVertex vertexData[]{
		// clang-format off
		{.pos = { 0.5f,  0.5f}, .col = {1, 0, 0, 1}}, // bottom right (red)
		{.pos = {-0.5f,  0.5f}, .col = {0, 1, 0, 1}}, // bottom left  (green)
		{.pos = { 0.0f, -0.5f}, .col = {0, 0, 1, 1}}, // top middle   (blue)
		// clang-format on
	};
	auto vertexbuffer = logicalDevice.createBuffer({.size = sizeof(vertexData), .usage = vk::BufferUsageFlagBits::eVertexBuffer});
	auto vertexbufferMemoryRequirements = logicalDevice.getBufferMemoryRequirements(vertexbuffer);
	auto vertexbufferMemoryTypeIndex = vkh::findMemoryTypeIndex(
		selectedPhysicalDevice.getMemoryProperties(),
		vertexbufferMemoryRequirements.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	auto vertexbufferMemory = logicalDevice.allocateMemory({
		.allocationSize = vertexbufferMemoryRequirements.size,
		.memoryTypeIndex = vertexbufferMemoryTypeIndex,
	});
	logicalDevice.bindBufferMemory(vertexbuffer, vertexbufferMemory, 0);
	{
		auto vertexbufferDeviceDataPtr = static_cast<std::uint8_t *>(logicalDevice.mapMemory(vertexbufferMemory, 0, vertexbufferMemoryRequirements.size));
		std::memcpy(vertexbufferDeviceDataPtr, vertexData, sizeof(vertexData));
		logicalDevice.unmapMemory(vertexbufferMemory);
	}

	auto [swapchain, swapchainDetails] = createSwapchain(selectedPhysicalDevice, logicalDevice, queueIndices, vulkanWindowSurface, sizeX, sizeY);
	auto swapchainImages = logicalDevice.getSwapchainImagesKHR(swapchain);
	auto swapchainImageViews = createSwapchainImageViews(swapchainImages, swapchainDetails, logicalDevice);

	auto renderpass = createRenderpass(logicalDevice, swapchainDetails.format);
	auto [graphicsPipeline, graphicsPipelineLayout] = createGraphicsPipeline(logicalDevice, renderpass, sizeof(TriangleVertex));
	auto framebuffers = createFramebuffers(logicalDevice, swapchainImageViews, renderpass, sizeX, sizeY);

	{
		vk::CommandPool graphicsCommandPool = logicalDevice.createCommandPool({
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = static_cast<std::uint32_t>(queueIndices.graphics.value()),
		});
		auto graphicsCommandBuffer = logicalDevice.allocateCommandBuffers({.commandPool = graphicsCommandPool, .commandBufferCount = 1}).front();

		while (true) {
			window.handleEvents();
			if (!window.isOpen())
				break;
            // prepare frame for drawing
            auto imageAcquiredSemaphore = logicalDevice.createSemaphore({});
            auto currentFramebuffer = logicalDevice.acquireNextImageKHR(swapchain, 100000000 /* fence timeout */, imageAcquiredSemaphore, nullptr);
            if (currentFramebuffer.result == vk::Result::eErrorOutOfDateKHR) {
                MessageBox(nullptr, "OUT OF DATE", "", MB_OK);
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

            graphicsCommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, swapchainDetails.extent.width, swapchainDetails.extent.height, 0.0f, 1.0f));
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

        logicalDevice.freeCommandBuffers(graphicsCommandPool, graphicsCommandBuffer);
		logicalDevice.destroyCommandPool(graphicsCommandPool);
	}

	// cleanup
	for (const auto &framebuffer : framebuffers)
		logicalDevice.destroyFramebuffer(framebuffer);
	framebuffers.clear();
	logicalDevice.destroyPipelineLayout(graphicsPipelineLayout);
	logicalDevice.destroyPipeline(graphicsPipeline);
	logicalDevice.destroyRenderPass(renderpass);
	for (auto &view : swapchainImageViews)
		logicalDevice.destroyImageView(view);
	swapchainImageViews.clear();
	logicalDevice.destroySwapchainKHR(swapchain);
	vulkanInstance.destroySurfaceKHR(vulkanWindowSurface);
	vulkanWindowSurface = nullptr;

	logicalDevice.freeMemory(vertexbufferMemory);
	logicalDevice.destroyBuffer(vertexbuffer);
	logicalDevice.destroy();
	vulkanInstance.destroyDebugUtilsMessengerEXT(debugMessenger);
	vulkanInstance.destroy();

} catch (const vk::SystemError &e) {
	MessageBox(nullptr, e.what(), "vk::SystemError", MB_OK);
} catch (const std::exception &e) {
	MessageBox(nullptr, e.what(), "std::exception", MB_OK);
} catch (...) {
	MessageBox(nullptr, "no details available", "Unknown exception", MB_OK);
}
