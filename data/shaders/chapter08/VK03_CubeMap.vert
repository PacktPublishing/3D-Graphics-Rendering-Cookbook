//
#version 460 core

layout (location=0) out vec3 dir;

layout(binding = 0) uniform UniformBuffer
{
	mat4 view;
	mat4 proj;
} ubo;

const vec3 pos[8] = vec3[8](
	vec3(-1.0,-1.0, 1.0),
	vec3( 1.0,-1.0, 1.0),
	vec3( 1.0, 1.0, 1.0),
	vec3(-1.0, 1.0, 1.0),

	vec3(-1.0,-1.0,-1.0),
	vec3( 1.0,-1.0,-1.0),
	vec3( 1.0, 1.0,-1.0),
	vec3(-1.0, 1.0,-1.0)
);

const int indices[36] = int[36](
	// front
	0, 1, 2, 2, 3, 0,
	// right
	1, 5, 6, 6, 2, 1,
	// back
	7, 6, 5, 5, 4, 7,
	// left
	4, 0, 3, 3, 7, 4,
	// bottom
	4, 5, 1, 1, 0, 4,
	// top
	3, 2, 6, 6, 7, 3
);

void main()
{
	float cubeSize = 100.0;
	int idx = indices[gl_VertexIndex];
	gl_Position = ubo.view * ubo.proj * vec4(cubeSize * pos[idx], 1.0);
	dir = vec3(pos[idx].x, -pos[idx].y, pos[idx].z);
}
