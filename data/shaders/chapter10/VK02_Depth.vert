//
#version 460

layout(location = 0) out vec3 uvw;
layout(location = 1) out vec3 v_worldNormal;
layout(location = 2) out vec4 v_worldPos;
layout(location = 3) out flat uint matIdx;

#include <data/shaders/chapter07/VK01.h>
#include <data/shaders/chapter07/VK01_VertCommon.h>

void main()
{
	DrawData dd = drawDataBuffer.data[gl_BaseInstance];

	uint refIdx = dd.indexOffset + gl_VertexIndex;
	ImDrawVert v = sbo.data[ibo.data[refIdx] + dd.vertexOffset];

	mat4 model = transformBuffer.data[gl_BaseInstance];

	v_worldPos   = model * vec4(v.x, v.y, v.z, 1.0);
	v_worldNormal = transpose(inverse(mat3(model))) * vec3(v.nx, v.ny, v.nz);

	v_worldPos.y = -v_worldPos.y;

	/* Assign shader outputs */
	gl_Position = ubo.proj * ubo.view * v_worldPos;
	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
	matIdx = dd.material;
	uvw = vec3(v.u, v.v, 1.0);
}
