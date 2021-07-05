//

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	mat4 light;
	vec4 cameraPos;
	vec4 frustumPlanes[6];
	vec4 frustumCorners[8];
	uint numShapesToCull;
};
