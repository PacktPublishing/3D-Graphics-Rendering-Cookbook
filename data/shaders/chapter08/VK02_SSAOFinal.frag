/**/
#version 460

layout(location = 0) in  vec2 texCoord1;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBuffer
{
	float scale;
	float bias;
	float zNear;
	float zFar;
	float radius;
	float attScale;
	float distScale;
} params;

layout(binding = 1) uniform sampler2D texScene;
layout(binding = 2) uniform sampler2D texSSAO;

void main()
{
	vec2 uv = vec2(texCoord1.x, 1.0 - texCoord1.y);

	vec4 color = texture(texScene, uv);
	float ssao = clamp( texture(texSSAO,  uv).r + params.bias, 0.0, 1.0 );

	outColor = vec4(
		mix(color, color * ssao, params.scale).rgb,
		1.0
	);
}
