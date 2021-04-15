#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <filesystem>
#include <functional>

#if defined(VULKANHELPER_IMPLEMENTATION)
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <iostream>
#include <fstream>

// temp?
#include <fmt/core.h>
#include <set>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#undef max
#undef min
#else
#error "CURRENTLY UNSUPPORTED PLATFORM"
#endif

namespace vkh {
	struct VertexDescription {
		std::vector<vk::VertexInputBindingDescription> bindings;
		std::vector<vk::VertexInputAttributeDescription> attributes;
		vk::PipelineVertexInputStateCreateFlags flags{};

		vk::PipelineVertexInputStateCreateInfo makePipelineVertexInputStateCreateInfo() const {
			return vk::PipelineVertexInputStateCreateInfo{
				.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
				.pVertexBindingDescriptions = bindings.data(),
				.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
				.pVertexAttributeDescriptions = attributes.data(),
			};
		}
	};

	class VertexDiscriptionBuilder {
	public:
		VertexDiscriptionBuilder &beginBinding(uint32_t stride, vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex);
		VertexDiscriptionBuilder &addAttribute(vk::Format format);
		VertexDiscriptionBuilder &stageCreateFlags(vk::PipelineVertexInputStateCreateFlags flags);
		VertexDescription build();

	private:
		uint32_t stride{0};
		uint32_t offset{0};
		uint32_t location{0};
		vk::PipelineVertexInputStateCreateFlags flags{};
		std::vector<vk::VertexInputBindingDescription> bindings;
		std::vector<vk::VertexInputAttributeDescription> attributes;
	};

#if defined(VULKANHELPER_IMPLEMENTATION)
	VertexDiscriptionBuilder &VertexDiscriptionBuilder::beginBinding(uint32_t stride, vk::VertexInputRate inputRate) {
		offset = 0;
		location = 0;
		vk::VertexInputBindingDescription binding{
			.binding = static_cast<uint32_t>(bindings.size()),
			.stride = stride,
			.inputRate = inputRate,
		};
		bindings.push_back(binding);
		return *this;
	}

	VertexDiscriptionBuilder &VertexDiscriptionBuilder::addAttribute(vk::Format format) {
		vk::VertexInputAttributeDescription attribute{
			.location = location,
			.binding = static_cast<uint32_t>(bindings.size() - 1),
			.format = format,
			.offset = offset,
		};

		attributes.push_back(attribute);

		location += 1;

		switch (format) {
		case vk::Format::eR32G32B32A32Sfloat:
		case vk::Format::eR32G32B32A32Sint:
		case vk::Format::eR32G32B32A32Uint:
			offset += sizeof(uint32_t) * 4;
			break;
		case vk::Format::eR32G32B32Sfloat:
		case vk::Format::eR32G32B32Sint:
		case vk::Format::eR32G32B32Uint:
			offset += sizeof(uint32_t) * 3;
			break;
		case vk::Format::eR32G32Sfloat:
		case vk::Format::eR32G32Sint:
		case vk::Format::eR32G32Uint:
			offset += sizeof(uint32_t) * 2;
			break;
		case vk::Format::eR32Sfloat:
		case vk::Format::eR32Sint:
		case vk::Format::eR32Uint:
			offset += sizeof(uint32_t) * 1;
			break;
		default:
			assert(false);
		}

		return *this;
	}

	VertexDiscriptionBuilder &VertexDiscriptionBuilder::stageCreateFlags(vk::PipelineVertexInputStateCreateFlags flags) {
		this->flags = flags;
		return *this;
	}

	VertexDescription VertexDiscriptionBuilder::build() {
		assert(bindings.size() > 0);
		return VertexDescription{
			bindings,
			attributes,
			flags};
	}
#endif

	vk::PipelineRasterizationStateCreateInfo makeDefaultRasterisationStateCreateInfo(vk::PolygonMode polygonMode);

	vk::PipelineMultisampleStateCreateInfo makeDefaultMultisampleStateCreateInfo();

	vk::PipelineColorBlendAttachmentState makeDefaultColorBlendSAttachmentState();

	struct Pipeline {
		vk::UniquePipeline pipeline;
		vk::UniquePipelineLayout layout;
	};

	class PipelineBuilder {
	public:
		Pipeline build(vk::Device device, vk::RenderPass pass, uint32_t subpass = 0);

		void setInitialViewPort(const vk::Viewport &viewport);
		void setInitialScissor(const vk::Rect2D &scissor);
		void setVertexInput(const vk::PipelineVertexInputStateCreateInfo &vertexInput);
		void setInputAssembly(const vk::PipelineInputAssemblyStateCreateInfo &inputassembly);
		void setRasterization(const vk::PipelineRasterizationStateCreateInfo &rasterization);
		void setMultisampling(const vk::PipelineMultisampleStateCreateInfo &multisampling);
		void setDepthStencil(const vk::PipelineDepthStencilStateCreateInfo &depthStencil);
		//void setColorBlendAttachment(const vk::PipelineColorBlendAttachmentState &colorBlendAttachment);		dont understand color lbending in vulkan
		void addShaderStages(const vk::PipelineShaderStageCreateInfo &shaderStage);
		void addDynamicState(const vk::DynamicState &dynamicstates);
		void addPushConstants(const vk::PushConstantRange &pushconstants);

	private:
		std::optional<vk::Viewport> viewport;
		std::optional<vk::Rect2D> scissor;
		std::optional<vk::PipelineVertexInputStateCreateInfo> vertexInput;
		std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssembly;
		std::optional<vk::PipelineRasterizationStateCreateInfo> rasterization;
		std::optional<vk::PipelineMultisampleStateCreateInfo> multisampling;
		std::optional<vk::PipelineDepthStencilStateCreateInfo> depthStencil;
		std::optional<vk::PipelineColorBlendAttachmentState> colorBlendAttachment;
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
		std::vector<vk::DynamicState> dynamicStateEnable;
		std::vector<vk::PushConstantRange> pushConstants;
	};

#if defined(VULKANHELPER_IMPLEMENTATION)
	void PipelineBuilder::setInitialViewPort(const vk::Viewport &viewport) {
		this->viewport = viewport;
	}
	void PipelineBuilder::setInitialScissor(const vk::Rect2D &scissor) {
		this->scissor = scissor;
	}
	void PipelineBuilder::setVertexInput(const vk::PipelineVertexInputStateCreateInfo &vertexInput) {
		this->vertexInput = vertexInput;
	}
	void PipelineBuilder::setInputAssembly(const vk::PipelineInputAssemblyStateCreateInfo &inputAssembly) {
		this->inputAssembly = inputAssembly;
	}
	void PipelineBuilder::setRasterization(const vk::PipelineRasterizationStateCreateInfo &rasterization) {
		this->rasterization = rasterization;
	}
	void PipelineBuilder::setMultisampling(const vk::PipelineMultisampleStateCreateInfo &multisampling) {
		this->multisampling = multisampling;
	}
	void PipelineBuilder::setDepthStencil(const vk::PipelineDepthStencilStateCreateInfo &depthStencil) {
		this->depthStencil = depthStencil;
	}
	void PipelineBuilder::addShaderStages(const vk::PipelineShaderStageCreateInfo &shaderStage) {
		this->shaderStages.push_back(shaderStage);
	}
	void PipelineBuilder::addDynamicState(const vk::DynamicState &dynamicstates) {
		this->dynamicStateEnable.push_back(dynamicstates);
	}
	void PipelineBuilder::addPushConstants(const vk::PushConstantRange &pushconstant) {
		this->pushConstants.push_back(pushconstant);
	}

	vk::PipelineRasterizationStateCreateInfo makeDefaultRasterisationStateCreateInfo(vk::PolygonMode polygonMode) {
		return vk::PipelineRasterizationStateCreateInfo{
			.polygonMode = polygonMode,
			.frontFace = vk::FrontFace::eClockwise,
			.lineWidth = 1.0f,
		};
	}

	vk::PipelineMultisampleStateCreateInfo makeDefaultMultisampleStateCreateInfo() {
		return vk::PipelineMultisampleStateCreateInfo{.minSampleShading = 1.0f};
	}

	vk::PipelineColorBlendAttachmentState makeDefaultColorBlendSAttachmentState() {
		return vk::PipelineColorBlendAttachmentState{
			.colorWriteMask =
				vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA,
		};
	}

	Pipeline PipelineBuilder::build(vk::Device device, vk::RenderPass pass, uint32_t subpass) {
		if (!vertexInput) {
			std::cout << "error: vertexInput was not specified in pipeline builder!\n";
			exit(-1);
		}

		// set state create infos:
		vk::PipelineVertexInputStateCreateInfo pvertexInputCI = vertexInput.value();
		vk::PipelineColorBlendAttachmentState pcolorBlendAttachmentCI = colorBlendAttachment.value_or(vkh::makeDefaultColorBlendSAttachmentState());
		vk::PipelineInputAssemblyStateCreateInfo pinputAssemlyStateCI = inputAssembly.value_or(vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eTriangleList});
		vk::PipelineRasterizationStateCreateInfo prasterizationStateCI = rasterization.value_or(vkh::makeDefaultRasterisationStateCreateInfo(vk::PolygonMode::eFill));
		vk::PipelineMultisampleStateCreateInfo multisamplerStateCI = multisampling.value_or(vkh::makeDefaultMultisampleStateCreateInfo());
		vk::PipelineDepthStencilStateCreateInfo pDepthStencilStateCI = depthStencil.value_or(vk::PipelineDepthStencilStateCreateInfo{});
		vk::Viewport pviewport = viewport.value_or(vk::Viewport{.width = 1, .height = 1});
		vk::Rect2D pscissor = scissor.value_or(vk::Rect2D{.extent = {static_cast<uint32_t>(pviewport.width), static_cast<uint32_t>(pviewport.height)}});

		Pipeline pipeline;

		//build pipeline layout:
		vk::PipelineLayoutCreateInfo layoutCI{
			.pushConstantRangeCount = uint32_t(pushConstants.size()),
			.pPushConstantRanges = pushConstants.data(),
		};
		pipeline.layout = device.createPipelineLayoutUnique(layoutCI);

		vk::PipelineViewportStateCreateInfo viewportStateCI{
			.viewportCount = 1,
			.pViewports = &pviewport,
			.scissorCount = 1,
			.pScissors = &pscissor,
		};

		//setup dummy color blending. We aren't using transparent objects yet
		//the blending is just "no blend", but we do write to the color attachment
		vk::PipelineColorBlendStateCreateInfo colorBlendingSCI{
			.logicOpEnable = VK_FALSE,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &pcolorBlendAttachmentCI,
		};

		// dynamic state setup:
		if (std::find(dynamicStateEnable.begin(), dynamicStateEnable.end(), vk::DynamicState::eViewport) == dynamicStateEnable.end()) {
			dynamicStateEnable.push_back(vk::DynamicState::eViewport);
		}
		if (std::find(dynamicStateEnable.begin(), dynamicStateEnable.end(), vk::DynamicState::eScissor) == dynamicStateEnable.end()) {
			dynamicStateEnable.push_back(vk::DynamicState::eScissor);
		}

		vk::PipelineDynamicStateCreateInfo dynamicStateCI{
			.dynamicStateCount = (uint32_t)dynamicStateEnable.size(),
			.pDynamicStates = dynamicStateEnable.data(),
		};

		//we now use all of the info structs we have been writing into into this one to create the pipeline
		vk::GraphicsPipelineCreateInfo pipelineCI{
			.stageCount = (uint32_t)shaderStages.size(),
			.pStages = shaderStages.data(),
			.pVertexInputState = &pvertexInputCI,
			.pInputAssemblyState = &pinputAssemlyStateCI,
			.pViewportState = &viewportStateCI,
			.pRasterizationState = &prasterizationStateCI,
			.pMultisampleState = &multisamplerStateCI,
			.pDepthStencilState = &pDepthStencilStateCI,
			.pColorBlendState = &colorBlendingSCI,
			.pDynamicState = &dynamicStateCI,
			.layout = pipeline.layout.get(),
			.renderPass = pass,
			.subpass = subpass,
		};

		auto ret = device.createGraphicsPipelineUnique({}, pipelineCI);
		if (ret.result != vk::Result::eSuccess) {
			std::cerr << "error: could not compile pipeline!\n";
			exit(-1);
		}
		pipeline.pipeline = std::move(ret.value);

		return pipeline;
	}
#endif

	std::optional<vk::UniqueShaderModule> loadShaderModule(vk::Device device, std::filesystem::path filePath);

	vk::PipelineShaderStageCreateInfo makeShaderStageCreateInfo(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule);

#if defined(VULKANHELPER_IMPLEMENTATION)
	std::optional<vk::UniqueShaderModule> loadShaderModule(vk::Device device, std::filesystem::path filePath) {
		std::ifstream file{filePath, std::ios::ate | std::ios::binary};

		if (!file.is_open()) {
			return {};
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
		file.seekg(0);
		file.read((char *)buffer.data(), fileSize);
		file.close();

		vk::ShaderModuleCreateInfo createInfo = {};

		createInfo.codeSize = buffer.size() * sizeof(uint32_t);
		createInfo.pCode = buffer.data();

		vk::ShaderModule shaderModule;
		return std::move(device.createShaderModuleUnique(createInfo));
	}

	vk::PipelineShaderStageCreateInfo makeShaderStageCreateInfo(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule) {
		vk::PipelineShaderStageCreateInfo info{};

		info.stage = stage;
		info.module = shaderModule;
		info.pName = "main";
		return info;
	}

#endif

	vk::FenceCreateInfo makeDefaultFenceCI();

	vk::AttachmentDescription makeDefaultColorAttackmentDescription();

#if defined(VULKANHELPER_IMPLEMENTATION)
	vk::FenceCreateInfo makeDefaultFenceCI() {
		vk::FenceCreateInfo info{};
		info.flags |= vk::FenceCreateFlagBits::eSignaled;
		return info;
	}

	vk::AttachmentDescription makeDefaultColorAttackmentDescription() {
		return vk::AttachmentDescription{
			.format = vk::Format::eUndefined,
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::ePresentSrcKHR,
		};
	}
#endif

	template <typename T>
	class Pool {
	public:
		Pool() = default;
		Pool(std::function<T(void)> creator, std::function<void(T)> destroyer, std::function<void(T)> resetter) : creator{std::move(creator)}, destroyer{std::move(destroyer)}, resetter{std::move(resetter)} {}

		Pool(Pool &&other) {
			this->creator = std::move(other.creator);
			this->destroyer = std::move(other.destroyer);
			this->resetter = std::move(other.resetter);
			this->pool = std::move(other.pool);
			this->usedList = std::move(other.usedList);
		}

		Pool &operator=(Pool &&other) {
			if (&other == this)
				return *this;
			return *new (this) Pool(std::move(other));
		}

		~Pool() {
			for (auto &el : pool) {
				destroyer(el);
			}
			for (auto &el : usedList) {
				destroyer(el);
			}
			pool.clear();
		}

		void flush() {
			for (auto &el : usedList) {
				resetter(el);
			}
			pool.insert(pool.end(), usedList.begin(), usedList.end());
			usedList.clear();
		}

		T get() {
			if (pool.size() == 0) {
				pool.push_back(creator());
			}

			auto el = pool.back();
			pool.pop_back();
			usedList.push_back(el);
			return el;
		}

	private:
		std::function<T(void)> creator;
		std::function<void(T)> destroyer;
		std::function<void(T)> resetter;
		std::vector<T> pool;
		std::vector<T> usedList;
	};

	vk::UniqueCommandPool makeUniqueCommandPool(
		vk::Device device,
		uint32_t queueFamilyIndex,
		vk::CommandPoolCreateFlagBits flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

#if defined(VULKANHELPER_IMPLEMENTATION)
	vk::UniqueCommandPool makeUniqueCommandPool(
		vk::Device device,
		uint32_t queueFamilyIndex,
		vk::CommandPoolCreateFlagBits flags) {
		vk::CommandPoolCreateInfo cmdPoolCreateInfo = {};
		cmdPoolCreateInfo.flags = flags; // we can reset individual command buffers from this pool
		cmdPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

		return device.createCommandPoolUnique(cmdPoolCreateInfo);
	}
#endif
} // namespace vkh

namespace vkh_detail {
	static PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
	static PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;
} // namespace vkh_detail

#if defined(VULKANHELPER_IMPLEMENTATION)
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pMessenger) {
	return vkh_detail::pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const *pAllocator) {
	return vkh_detail::pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
#endif

namespace vkh {
	vk::UniqueInstance createInstance(const std::vector<const char *> &layers, const std::vector<const char *> &extensions);

	vk::UniqueDebugUtilsMessengerEXT createDebugMessenger(const vk::UniqueInstance &instance);

	vk::PhysicalDevice selectPhysicalDevice(const vk::UniqueInstance &instance, const std::function<std::size_t(vk::PhysicalDevice)> &rateDeviceSuitability);

    vk::UniqueDevice createLogicalDevice(vk::PhysicalDevice physicalDevice, const std::set<std::size_t> &queueIndices, const std::vector<const char *> &extensions);

	std::uint32_t findMemoryTypeIndex(vk::PhysicalDeviceMemoryProperties const &memoryProperties, uint32_t typeBits, vk::MemoryPropertyFlags requirementsMask);

#if defined(VULKANHELPER_IMPLEMENTATION)
	vk::UniqueInstance createInstance(const std::vector<const char *> &layers, const std::vector<const char *> &extensions) {
		vk::ApplicationInfo vulkanApplicationInfo{.apiVersion = VK_API_VERSION_1_1};
		return vk::createInstanceUnique({
			.pApplicationInfo = &vulkanApplicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		});
	}

	vk::UniqueDebugUtilsMessengerEXT createDebugMessenger(const vk::UniqueInstance &instance) {
		vkh_detail::pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
		if (!vkh_detail::pfnVkCreateDebugUtilsMessengerEXT)
			throw std::runtime_error("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
		vkh_detail::pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
		if (!vkh_detail::pfnVkDestroyDebugUtilsMessengerEXT)
			throw std::runtime_error("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");

		return instance->createDebugUtilsMessengerEXTUnique(vk::DebugUtilsMessengerCreateInfoEXT{
			.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
			.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
								  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) -> VkBool32 {
				if (pCallbackData->messageIdNumber == 648835635) {
					// UNASSIGNED-khronos-Validation-debug-build-warning-message
					return VK_FALSE;
				}
				if (pCallbackData->messageIdNumber == 767975156) {
					// UNASSIGNED-BestPractices-vkCreateInstance-specialuse-extension
					return VK_FALSE;
				}
				std::string message = fmt::format(
					"{}: {}:\n\tmessage name   = <{}>\n\tmessage number = {}\n\tmessage        = <{}>\n",
					vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)),
					vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)),
					pCallbackData->pMessageIdName,
					pCallbackData->messageIdNumber,
					pCallbackData->pMessage);
				if (0 < pCallbackData->queueLabelCount) {
					message += fmt::format("\tQueue Labels:\n");
					for (uint8_t i = 0; i < pCallbackData->queueLabelCount; i++)
						message += fmt::format("\t\tlabelName = <{}>\n", pCallbackData->pQueueLabels[i].pLabelName);
				}
				if (0 < pCallbackData->cmdBufLabelCount) {
					message += fmt::format("\tCommandBuffer Labels:\n");
					for (uint8_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
						message += fmt::format("\t\tlabelName = <{}>\n", pCallbackData->pCmdBufLabels[i].pLabelName);
				}
				if (0 < pCallbackData->objectCount) {
					message += fmt::format("\tObjects:\n");
					for (uint8_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
						message += fmt::format("\t\tlabelName = <{}>\nObject {}\n\t\t\tobjectType   = {}\n\t\t\tobjectHandle = {}\n",
											   pCallbackData->pCmdBufLabels[i].pLabelName, i,
											   vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)),
											   pCallbackData->pObjects[i].objectHandle);
						if (pCallbackData->pObjects[i].pObjectName)
							message += fmt::format("\t\t\tobjectName   = <{}>\n", pCallbackData->pObjects[i].pObjectName);
					}
				}
				MessageBox(nullptr, message.c_str(), "Vulkan Validation Error", MB_OK);
				return VK_TRUE;
			},
		});
	}

	vk::PhysicalDevice selectPhysicalDevice(vk::Instance instance, const std::function<std::size_t(vk::PhysicalDevice)> &rateDeviceSuitability) {
		auto devices = instance.enumeratePhysicalDevices();
		std::vector<std::size_t> devicesSuitability;
		devicesSuitability.reserve(devices.size());
		for (const auto &device : devices)
			devicesSuitability.push_back(rateDeviceSuitability(device));
		auto bestDeviceIter = std::max_element(devicesSuitability.begin(), devicesSuitability.end());
		if (*bestDeviceIter == 0)
			throw std::runtime_error("Failed to find a suitable physical device");
		return devices[std::distance(devicesSuitability.begin(), bestDeviceIter)];
	}

	vk::UniqueDevice createLogicalDevice(vk::PhysicalDevice physicalDevice, const std::set<std::size_t> &queueIndices, const std::vector<const char *> &extensions) {
		float queuePriority = 0.0f;
		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateinfos;
		for (auto index : queueIndices) {
			deviceQueueCreateinfos.push_back({
				.queueFamilyIndex = static_cast<std::uint32_t>(index),
				.queueCount = 1,
				.pQueuePriorities = &queuePriority,
			});
		}
		return physicalDevice.createDevice({
			.queueCreateInfoCount = static_cast<std::uint32_t>(deviceQueueCreateinfos.size()),
			.pQueueCreateInfos = deviceQueueCreateinfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		});
	}

	std::uint32_t findMemoryTypeIndex(vk::PhysicalDeviceMemoryProperties const &memoryProperties, uint32_t typeBits, vk::MemoryPropertyFlags requirementsMask) {
		std::uint32_t type_index = std::uint32_t(~0);
		for (std::uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			if (typeBits & 1 && (memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
				type_index = i;
				break;
			}
			typeBits >>= 1;
		}
		if (type_index == std::uint32_t(~0))
			throw std::runtime_error("Unable to find suitable memory type index");
		return type_index;
	}
#endif
} // namespace vkh
