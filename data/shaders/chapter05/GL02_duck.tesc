//
#version 460 core

layout (vertices = 3) out;

layout (location=0) in vec2 uv_in[];
layout (location=1) in vec3 worldPos_in[];

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
	float tesselationScale;
};

in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex
{
	vec4 gl_Position;
} gl_out[];

struct vertex
{
	vec2 uv;
};

layout(location = 0) out vertex Out[];

float getTessLevel(float distance0, float distance1)
{
	const float distanceScale1 = 7.0;
	const float distanceScale2 = 10.0;
	const float avgDistance = (distance0 + distance1) / 2.0;

	if (avgDistance <= distanceScale1 * tesselationScale)
	{
		return 5.0;
	}
	else if (avgDistance <= distanceScale2 * tesselationScale)
	{
		return 3.0;
	}

	return 1.0;
}

void main()
{	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	Out[gl_InvocationID].uv = uv_in[gl_InvocationID];

	vec3 c = cameraPos.xyz;

	float eyeToVertexDistance0 = distance(c, worldPos_in[0]);
	float eyeToVertexDistance1 = distance(c, worldPos_in[1]);
	float eyeToVertexDistance2 = distance(c, worldPos_in[2]);

	// Calculate the tessellation levels
	gl_TessLevelOuter[0] = getTessLevel(eyeToVertexDistance1, eyeToVertexDistance2);
	gl_TessLevelOuter[1] = getTessLevel(eyeToVertexDistance2, eyeToVertexDistance0);
	gl_TessLevelOuter[2] = getTessLevel(eyeToVertexDistance0, eyeToVertexDistance1);
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
};
