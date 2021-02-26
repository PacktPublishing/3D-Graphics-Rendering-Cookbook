//
#version 460

layout(location = 0) in vec3 uvw;

layout(location = 0) out vec4 outColor;

// layout(binding = 5) readonly buffer MatBO    { uint   data[]; } mat_bo;

void main()
{
	outColor = vec4(uvw, 0.5);
}
