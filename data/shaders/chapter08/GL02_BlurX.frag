/**/
#version 460 core

layout(location = 0) in  vec2 uv;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSSAO;

const vec2 gaussFilter[11] = vec2[](
	vec2(-5.0,  3.0/133.0),
	vec2(-4.0,  6.0/133.0),
	vec2(-3.0, 10.0/133.0),
	vec2(-2.0, 15.0/133.0),
	vec2(-1.0, 20.0/133.0),
	vec2( 0.0, 25.0/133.0),
	vec2( 1.0, 20.0/133.0),
	vec2( 2.0, 15.0/133.0),
	vec2( 3.0, 10.0/133.0),
	vec2( 4.0,  6.0/133.0),
	vec2( 5.0,  3.0/133.0)
);

void main()
{
	vec3 color = vec3(0.0);

	float scale = 1.0 / textureSize(texSSAO, 0).x;

	for ( int i = 0; i < 11; i++ )
	{
		vec2 coord = vec2(uv.x + gaussFilter[i].x * scale, uv.y);
		color += textureLod(texSSAO, coord, 0).rgb * gaussFilter[i].y;
	}

	outColor = vec4(color, 1.0);
}
