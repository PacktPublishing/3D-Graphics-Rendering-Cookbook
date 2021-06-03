#version 460

#extension GL_EXT_nonuniform_qualifier : require

layout (binding = 1) uniform sampler2D textures[];

layout (location = 0) in vec2 in_uv;
layout (location = 1) flat in uint in_texIndex;

layout (location = 0) out vec4 outFragColor;

const uint depthTextureMask = 0xFFFF;

void main() 
{
	uint tex = in_texIndex & depthTextureMask;
	uint texType = (in_texIndex >> 16) & depthTextureMask;

	vec4 value = texture(textures[nonuniformEXT(tex)], in_uv);

	outFragColor = (texType == 0) ? vec4(value.xyz, 1.0) : vec4(value.rrr, 1.0);
}
