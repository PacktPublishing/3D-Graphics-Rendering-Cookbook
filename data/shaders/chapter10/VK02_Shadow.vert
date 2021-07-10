//
#version 460

layout(location = 0) out vec3 uvw;
layout(location = 1) out vec3 v_worldNormal;
layout(location = 2) out vec4 v_worldPos;
layout(location = 3) out flat uint matIdx;

layout(location = 4) out vec4 v_shadowCoord;

#include <data/shaders/chapter07/VK01.h>
#include <data/shaders/chapter07/VK01_VertCommon.h>

layout(binding = 6) readonly buffer ShadowBO  { mat4 lightProj; mat4 lightView; } shadow_bo;

// Vulkan's Z is in 0..1, but we did "(gl_Position.z + gl_Position.w) / 2.0" in VK02_Depth.vert
const mat4 scaleBias = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0);

void main()
{
	DrawData dd = drawDataBuffer.data[gl_BaseInstance];

	uint refIdx = dd.indexOffset + gl_VertexIndex;
	ImDrawVert v = sbo.data[ibo.data[refIdx] + dd.vertexOffset];

	mat4 model = transformBuffer.data[gl_BaseInstance];

	v_worldPos    = model * vec4(v.x, v.y, v.z, 1.0);
	v_worldNormal = transpose(inverse(mat3(model))) * vec3(v.nx, v.ny, v.nz);

	// assign shader outputs
	gl_Position = ubo.proj * ubo.view * v_worldPos;
	matIdx = dd.material;
	uvw = vec3(v.u, v.v, 1.0);

	// shadow coordinates
	const mat4 lightMVP = shadow_bo.lightProj * shadow_bo.lightView;
	v_shadowCoord = scaleBias * lightMVP * vec4(v_worldPos.xyz, 1.0);
}
