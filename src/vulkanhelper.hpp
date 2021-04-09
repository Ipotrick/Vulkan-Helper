#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#ifdef VULKANHELPER_IMPLEMENTATION
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <iostream>
#endif 

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



    vk::PipelineRasterizationStateCreateInfo makeDefaultRasterisationStateCreateInfo(vk::PolygonMode polygonMode);

    vk::PipelineMultisampleStateCreateInfo makeDefaultMultisampleStateCreateInfo();

    vk::PipelineColorBlendAttachmentState makeDefaultColorBlendSAttachmentState();

    struct Pipeline {
        vk::UniquePipeline pipeline;
        vk::UniquePipelineLayout layout;
    };

    class PipelineBuilder {
    public:
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

        Pipeline build(vk::Device device, vk::RenderPass pass, uint32_t subpass = 0);
    };

#ifdef VULKANHELPER_IMPLEMENTATION
    vk::PipelineRasterizationStateCreateInfo makeDefaultRasterisationStateCreateInfo(vk::PolygonMode polygonMode)
    {
        return vk::PipelineRasterizationStateCreateInfo{
            .polygonMode = polygonMode,
            .frontFace = vk::FrontFace::eClockwise,
            .lineWidth = 1.0f,
        };
    }

    vk::PipelineMultisampleStateCreateInfo makeDefaultMultisampleStateCreateInfo()
    {
        return vk::PipelineMultisampleStateCreateInfo{ .minSampleShading = 1.0f };
    }

    vk::PipelineColorBlendAttachmentState makeDefaultColorBlendSAttachmentState()
    {
        return vk::PipelineColorBlendAttachmentState{
        .colorWriteMask =
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA,
        };
    }

    Pipeline PipelineBuilder::build(vk::Device device, vk::RenderPass pass, uint32_t subpass)
    {
        if (!vertexInput) {
            std::cout << "error: vertexInput was not specified in pipeline builder!\n";
            exit(-1);
        }

        // set state create infos:
        vk::PipelineVertexInputStateCreateInfo    pvertexInputCI = vertexInput.value();
        vk::PipelineColorBlendAttachmentState     pcolorBlendAttachmentCI = colorBlendAttachment.value_or(vkh::makeDefaultColorBlendSAttachmentState());
        vk::PipelineInputAssemblyStateCreateInfo  pinputAssemlyStateCI = inputAssembly.value_or(vk::PipelineInputAssemblyStateCreateInfo{ .topology = vk::PrimitiveTopology::eTriangleList });
        vk::PipelineRasterizationStateCreateInfo  prasterizationStateCI = rasterization.value_or(vkh::makeDefaultRasterisationStateCreateInfo(vk::PolygonMode::eFill));
        vk::PipelineMultisampleStateCreateInfo    multisamplerStateCI = multisampling.value_or(vkh::makeDefaultMultisampleStateCreateInfo());
        vk::PipelineDepthStencilStateCreateInfo   pDepthStencilStateCI = depthStencil.value_or(vk::PipelineDepthStencilStateCreateInfo{});
        vk::Viewport                              pviewport = viewport.value_or(vk::Viewport{ .width = 1,.height = 1 });
        vk::Rect2D                                pscissor = scissor.value_or(vk::Rect2D{ .extent = {static_cast<uint32_t>(pviewport.width), static_cast<uint32_t>(pviewport.height)} });

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
}

