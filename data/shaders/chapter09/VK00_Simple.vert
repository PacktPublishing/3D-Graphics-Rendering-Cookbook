//
#version 460

layout(location = 0) out vec3 v_worldNormal;
layout(location = 1) out vec4 v_worldPos;

#include <data/shaders/chapter07/VK01.h>
#include <data/shaders/chapter07/VK01_VertCommon.h>

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

const uint indices[36] = uint[36](
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

const vec3 normals[6] = vec3[6](
	vec3( 0, 0, 1),
	vec3( 1, 0, 0),
	vec3( 0, 0,-1),
	vec3(-1, 0,-1),
	vec3( 0,-1, 0),
	vec3( 0, 1, 0)
);

void main()
{
	uint vertexIndex = gl_VertexIndex % 36;

	uint vidx = indices[vertexIndex];
	uint faceIndex = vertexIndex / 6;

	vec3 position = pos[vidx];
	vec3 normal = normals[faceIndex];

	mat4 model = transformBuffer.data[gl_BaseInstance];

	v_worldPos   = model * vec4(position, 1.0);
	v_worldNormal = transpose(inverse(mat3(model))) * normal;

	// Flip Y axis (Vulkan compatibility)
	v_worldPos.y = -v_worldPos.y;

	gl_Position = ubo.proj * ubo.view * v_worldPos;
}
