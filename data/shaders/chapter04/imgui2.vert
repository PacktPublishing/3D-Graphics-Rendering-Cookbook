// glsl_shader.vert, compiled with:
// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
//layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

layout(binding = 0) uniform  UniformBuffer { mat4   inMtx; } ubo;

void main()
{
	Out.Color = aColor;
	Out.UV = aUV;

	gl_Position = ubo.inMtx * vec4(aPos.x, aPos.y, 0.0, 1.0);
}
