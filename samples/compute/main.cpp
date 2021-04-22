#define VULKANHELPER_IMPLEMENTATION
#include <vulkanhelper.hpp>

#include "../shared/load-shader.hpp"

#include <glm/glm.hpp>

int main() try {

	auto vulkanInstance = vkh::createInstance({}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME});
	auto debugMessenger = vkh::createDebugMessenger(vulkanInstance);

	std::vector<const char *> deviceExtensions{};
	std::uint32_t computeQueueFamilyIndex = 0;

	auto selectedPhysicalDevice = vkh::selectPhysicalDevice(vulkanInstance, [&](vk::PhysicalDevice device) -> std::size_t {
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
	auto computeQueue = logicalDevice.getQueue(computeQueueFamilyIndex, 0);

	glm::ivec2 dim{512, 512};
	std::vector<std::uint32_t> localBuffer;
	localBuffer.resize(dim.x * dim.y);
	std::uint32_t localBufferByteCount = static_cast<std::uint32_t>(localBuffer.size() * sizeof(localBuffer[0]));

	auto deviceBuffer = logicalDevice.createBuffer({.size = localBufferByteCount, .usage = vk::BufferUsageFlagBits::eStorageBuffer});
	auto deviceBufferMemoryRequirements = logicalDevice.getBufferMemoryRequirements(deviceBuffer);
	auto deviceBufferMemoryTypeIndex = vkh::findMemoryTypeIndex(
		selectedPhysicalDevice.getMemoryProperties(),
		deviceBufferMemoryRequirements.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	auto deviceBufferMemory = logicalDevice.allocateMemory({
		.allocationSize = deviceBufferMemoryRequirements.size,
		.memoryTypeIndex = deviceBufferMemoryTypeIndex,
	});
	logicalDevice.bindBufferMemory(deviceBuffer, deviceBufferMemory, 0);

	glslang::InitializeProcess();
	auto computeSpv = loadGlslShaderToSpv("samples/compute/main.comp");
	glslang::FinalizeProcess();

	vkh::DescriptorSetLayoutCache layoutCache{ logicalDevice };
	vkh::GeneralDescriptorSetAllocator descSetAllocator{ logicalDevice };

	vk::DescriptorSet mainDescriptorSet = 
		vkh::DescriptorSetBuilder(&descSetAllocator, &layoutCache)
		.addBufferBinding(
			vk::DescriptorSetLayoutBinding{
					.binding = 0, 
					.descriptorType = vk::DescriptorType::eStorageBuffer, 
					.descriptorCount = 1, 
					.stageFlags = vk::ShaderStageFlagBits::eCompute
			},
			vk::DescriptorBufferInfo{.buffer = deviceBuffer, .range= localBufferByteCount}
		)
		.build();

	vkh::ComputePipelineBuilder pipelineBuilder(logicalDevice);
	pipelineBuilder.setShaderStage(&computeSpv);
	pipelineBuilder.reflectSPVForDescriptors(layoutCache);
	auto computePipeline = pipelineBuilder.build();

	auto commandPool = logicalDevice.createCommandPoolUnique({.queueFamilyIndex = computeQueueFamilyIndex});
	auto commandBuffer = logicalDevice.allocateCommandBuffers({.commandPool = *commandPool, .commandBufferCount = 1}).front();

	commandBuffer.begin(vk::CommandBufferBeginInfo{});
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline.pipeline.get());
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, computePipeline.layout.get(), 0, mainDescriptorSet, nullptr);
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

	logicalDevice.freeCommandBuffers(*commandPool, commandBuffer);

	auto vertexbufferDeviceDataPtr = static_cast<std::uint8_t *>(logicalDevice.mapMemory(deviceBufferMemory, 0, deviceBufferMemoryRequirements.size));
	std::memcpy(localBuffer.data(), vertexbufferDeviceDataPtr, localBufferByteCount);
	logicalDevice.unmapMemory(deviceBufferMemory);

	auto savePPM = [](const std::filesystem::path &filepath, const std::vector<std::uint32_t> buffer, glm::ivec2 dim) {
		std::ofstream output_file(filepath, std::ios::binary);
		if (output_file.is_open()) {
			output_file << "P6\n"
						<< dim.x << " " << dim.y << "\n255\n";
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
