#pragma once

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Include/ResourceLimits.h>

static constexpr TBuiltInResource getResource() {
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
static std::vector<std::uint32_t> glslToSpv(vk::ShaderStageFlagBits shader_stage, const std::string &glsl_code) {
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
	TBuiltInResource resource = getResource();
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
static std::string textFileToString(const std::filesystem::path &filepath) {
	std::ifstream sourceFile(filepath);
	std::string sourceString;
	if (sourceFile.is_open()) {
		sourceFile.seekg(0, std::ios::end);
		sourceString.reserve(sourceFile.tellg());
		sourceFile.seekg(0, std::ios::beg);
		sourceString.assign(
			std::istreambuf_iterator<char>(sourceFile),
			std::istreambuf_iterator<char>());
		sourceFile.close();
	} else {
		auto message = fmt::format("Failed to open shader file {} in {}", filepath.string(), std::filesystem::current_path().string());
		throw std::runtime_error(message.c_str());
	}
	return sourceString;
}
static std::vector<std::uint32_t> loadGlslShaderToSpv(const std::filesystem::path &filepath) {
	auto sourceString = textFileToString(filepath);
	vk::ShaderStageFlagBits shaderStage;

	if (filepath.extension() == ".vert") {
		shaderStage = vk::ShaderStageFlagBits::eVertex;
	} else if (filepath.extension() == ".frag") {
		shaderStage = vk::ShaderStageFlagBits::eFragment;
	} else {
		throw std::runtime_error(fmt::format("Attempting to load a currently unsupported shader file stage '{}'", filepath.extension().string()).c_str());
	}

	return glslToSpv(shaderStage, sourceString);
}
