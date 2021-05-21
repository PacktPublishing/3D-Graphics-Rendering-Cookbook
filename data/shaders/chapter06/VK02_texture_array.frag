#version 460

#extension GL_EXT_nonuniform_qualifier : require

layout (binding = 2) uniform sampler2D textures[];

layout (location = 0) in vec2 in_uv;
layout (location = 1) flat in uint in_texIndex;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = texture(textures[nonuniformEXT(in_texIndex)], in_uv);
}
