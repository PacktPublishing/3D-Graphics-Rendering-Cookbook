/**/
#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texScene;

void main()
{
	vec4 color = texture(texScene, uv);

	float luminance = dot(color, vec4(0.33, 0.34, 0.33, 0.0));

	outColor = luminance >= 1.0 ? color : vec4(vec3(0.0), 1.0);
}
