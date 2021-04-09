#pragma once

#include <cinttypes>
#include <vector>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace vkh {
	struct VertexDescription {
		std::vector<vk::VertexInputBindingDescription> bindings;
		std::vector<vk::VertexInputAttributeDescription> attributes;
		vk::PipelineVertexInputStateCreateFlags flags{};

		vk::PipelineVertexInputStateCreateInfo makePipelineVertexInputStateCreateInfo() const
		{
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
		VertexDiscriptionBuilder& beginBinding(uint32_t stride, vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex);
		VertexDiscriptionBuilder& setAttribute(vk::Format format);
		VertexDiscriptionBuilder& stageCreateFlags(vk::PipelineVertexInputStateCreateFlags flags);
		VertexDescription build();
	private:
		uint32_t stride{ 0 };
		uint32_t offset{ 0 };
		uint32_t location{ 0 };
		vk::PipelineVertexInputStateCreateFlags flags{};
		std::vector<vk::VertexInputBindingDescription> bindings;
		std::vector<vk::VertexInputAttributeDescription> attributes;
	};

#ifdef VULKANHELPER_IMPLEMENTATION
    VertexDiscriptionBuilder& VertexDiscriptionBuilder::beginBinding(uint32_t stride, vk::VertexInputRate inputRate)
    {
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

    VertexDiscriptionBuilder& VertexDiscriptionBuilder::setAttribute(vk::Format format)
    {
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

    VertexDiscriptionBuilder& VertexDiscriptionBuilder::stageCreateFlags(vk::PipelineVertexInputStateCreateFlags flags)
    {
        this->flags = flags;
        return *this;
    }

    VertexDescription VertexDiscriptionBuilder::build()
    {
        assert(bindings.size() > 0);
        return VertexDescription{
            bindings,
            attributes,
            flags
        };
    }
#endif
}

