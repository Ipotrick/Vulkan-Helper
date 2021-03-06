#version 400

#extension GL_ARB_separate_shader_objects: enable
#extension GL_ARB_shading_language_420pack: enable

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outColor;

void main() {
	outColor = inColor;
	gl_Position = vec4(inPosition, 0.5, 1);
}
