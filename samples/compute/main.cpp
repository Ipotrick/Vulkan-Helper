#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

#include "../shared/load-shader.hpp"

#include <glm/glm.hpp>

int main() try {

	auto vulkanInstance = vkh::createInstance({}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME});
	auto debugMessenger = vkh::createDebugMessenger(*vulkanInstance);

	std::vector<const char *> deviceExtensions{};
	std::uint32_t computeQueueFamilyIndex = 0;

	auto selectedPhysicalDevice = vkh::selectPhysicalDevice(*vulkanInstance, [&](vk::PhysicalDevice device) -> std::size_t {
		std::size_t score = 0;

		auto queueFamilyProperties = device.getQueueFamilyProperties();
		for (std::uint32_t i = 0; i < queueFamilyProperties.size(); ++i) {
			if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) {
				computeQueueFamilyIndex = i, score += 1;
				break;
			}
		}

		return score;
	});

	auto selectedPhysicalDeviceProperties = selectedPhysicalDevice.getProperties();
	fmt::print("Selected Physical Device: {}\n", selectedPhysicalDeviceProperties.deviceName);

	auto logicalDevice = vkh::createLogicalDevice(selectedPhysicalDevice, {computeQueueFamilyIndex}, deviceExtensions);
	auto computeQueue = logicalDevice->getQueue(computeQueueFamilyIndex, 0);

    glm::ivec2 dim{512, 512};
	std::vector<std::uint32_t> localBuffer;
	localBuffer.resize(dim.x * dim.y);
    std::uint32_t localBufferByteCount = static_cast<std::uint32_t>(localBuffer.size() * sizeof(localBuffer[0]));

	auto deviceBuffer = logicalDevice->createBuffer({.size = localBufferByteCount, .usage = vk::BufferUsageFlagBits::eStorageBuffer});
	auto deviceBufferMemoryRequirements = logicalDevice->getBufferMemoryRequirements(deviceBuffer);
	auto deviceBufferMemoryTypeIndex = vkh::findMemoryTypeIndex(
		selectedPhysicalDevice.getMemoryProperties(),
		deviceBufferMemoryRequirements.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	auto deviceBufferMemory = logicalDevice->allocateMemory({
		.allocationSize = deviceBufferMemoryRequirements.size,
		.memoryTypeIndex = deviceBufferMemoryTypeIndex,
	});
	logicalDevice->bindBufferMemory(deviceBuffer, deviceBufferMemory, 0);

	vk::DescriptorSetLayoutBinding descriptorSetLayoutBindings[] = {
		{0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute, 0},
	};
	auto descriptorSetLayout = logicalDevice->createDescriptorSetLayout({.bindingCount = 1, .pBindings = descriptorSetLayoutBindings});
	auto computePipelineLayout = logicalDevice->createPipelineLayout({.setLayoutCount = 1, .pSetLayouts = &descriptorSetLayout});

	glslang::InitializeProcess();
	auto computeSpv = loadGlslShaderToSpv("samples/compute/main.comp");
	glslang::FinalizeProcess();
	auto computeShaderModule = logicalDevice->createShaderModule({.codeSize = computeSpv.size() * sizeof(computeSpv[0]), .pCode = computeSpv.data()});

	vk::ComputePipelineCreateInfo computePipelineCreateInfo{
		.stage{
			.stage = vk::ShaderStageFlagBits::eCompute,
			.module = computeShaderModule,
			.pName = "main",
		},
		.layout = computePipelineLayout,
	};

	auto [computePipelineCreationResult, computePipeline] = logicalDevice->createComputePipeline(nullptr, computePipelineCreateInfo);
	if (computePipelineCreationResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create compute pipeline");

	logicalDevice->destroyShaderModule(computeShaderModule);

	vk::DescriptorPoolSize descriptorPoolSize{.type = vk::DescriptorType::eStorageBuffer, .descriptorCount = 1};
	auto descriptorPool = logicalDevice->createDescriptorPool({
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &descriptorPoolSize,
	});
	auto descriptorSets = logicalDevice->allocateDescriptorSets({.descriptorPool = descriptorPool, .descriptorSetCount = 1, .pSetLayouts = &descriptorSetLayout});
	vk::DescriptorBufferInfo descriptorBufferInfo(deviceBuffer, 0, localBufferByteCount);
	logicalDevice->updateDescriptorSets(
		{
			vk::WriteDescriptorSet{
				.dstSet = descriptorSets[0],
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pBufferInfo = &descriptorBufferInfo,
			},
		},
		nullptr);

    
    auto commandPool = logicalDevice->createCommandPoolUnique({.queueFamilyIndex = computeQueueFamilyIndex});
    auto commandBuffer = logicalDevice->allocateCommandBuffers({.commandPool = *commandPool, .commandBufferCount = 1}).front();

    commandBuffer.begin(vk::CommandBufferBeginInfo{});
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, computePipelineLayout, 0, descriptorSets[0], nullptr);
    commandBuffer.dispatch(dim.x, dim.y, 1);
    commandBuffer.end();
    
    double duration = 0.0;

    for (int i = 0; i < 10; ++i) {
        fmt::print("starting compute shader... ");
        auto t0 = std::chrono::high_resolution_clock::now();
        computeQueue.submit({{
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
        }});
        computeQueue.waitIdle();
        auto t1 = std::chrono::high_resolution_clock::now();
        double thisDur = std::chrono::duration<double>(t1 - t0).count();
        duration += thisDur;
        fmt::print("Finished! took {}s\n", thisDur);
    }
    duration /= 10;
    fmt::print("Average duration {}s ({} fps)\n", duration, 1.0 / duration);

    logicalDevice->freeCommandBuffers(*commandPool, commandBuffer);

	auto vertexbufferDeviceDataPtr = static_cast<std::uint8_t *>(logicalDevice->mapMemory(deviceBufferMemory, 0, deviceBufferMemoryRequirements.size));
	std::memcpy(localBuffer.data(), vertexbufferDeviceDataPtr, localBufferByteCount);
	logicalDevice->unmapMemory(deviceBufferMemory);

    auto savePPM = [](const std::filesystem::path &filepath, const std::vector<std::uint32_t> buffer, glm::ivec2 dim) {
        std::ofstream output_file(filepath, std::ios::binary);
        if (output_file.is_open()) {
            output_file << "P6\n" << dim.x << " " << dim.y << "\n255\n";
            for (const auto &p : buffer) {
                std::uint8_t r = (p >> 0x18) & 0xff;
                std::uint8_t g = (p >> 0x10) & 0xff;
                std::uint8_t b = (p >> 0x08) & 0xff;
                output_file << r << g << b;
            }
        }
    };

    fmt::print("Saving image... ");
    savePPM("build/compute.ppm", localBuffer, dim);
    fmt::print("Finished\n");

    std::cin.get();

} catch (const vk::SystemError &e) {
	fmt::print("vk::SystemError: {}", e.what());
    std::cin.get();
} catch (const std::exception &e) {
	fmt::print("std::exception: {}", e.what());
    std::cin.get();
} catch (...) {
	fmt::print("Unknown exception: no details available");
    std::cin.get();
}
