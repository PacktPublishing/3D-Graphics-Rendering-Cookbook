/**/
#version 460

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBuffer { float exposure; float maxWhite; float bloomStrength; float adaptationSpeed; } ubo;

layout(binding = 1) uniform sampler2D texScene;
layout(binding = 2) uniform sampler2D texLuminance;
layout(binding = 3) uniform sampler2D texBloom;

// Extended Reinhard tone mapping operator
vec3 Reinhard2(vec3 x)
{
	return (x * (1.0 + x / (ubo.maxWhite * ubo.maxWhite))) / (1.0 + x);
}

void main()
{
	vec3 color = texture(texScene, uv).rgb;
	vec3 bloom = texture(texBloom, vec2(uv.x, 1.0 - uv.y)).rgb;
	float avgLuminance = texture(texLuminance, vec2(0.5, 0.5)).x;

	float midGray = 0.5;

	color *= ubo.exposure * midGray / (avgLuminance + 0.001);
	color = Reinhard2(color);
	outColor = vec4(color + ubo.bloomStrength * bloom, 1.0);
}
