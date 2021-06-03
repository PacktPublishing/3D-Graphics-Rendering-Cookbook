/**/

layout(location = 0) in  vec2 texCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSSAO;

const vec4 gaussFilter[11] = vec4[](
	vec4(-5.0, 0.0, 0.0,  3.0/133.0),
	vec4(-4.0, 0.0, 0.0,  6.0/133.0),
	vec4(-3.0, 0.0, 0.0, 10.0/133.0),
	vec4(-2.0, 0.0, 0.0, 15.0/133.0),
	vec4(-1.0, 0.0, 0.0, 20.0/133.0),
	vec4( 0.0, 0.0, 0.0, 25.0/133.0),
	vec4( 1.0, 0.0, 0.0, 20.0/133.0),
	vec4( 2.0, 0.0, 0.0, 15.0/133.0),
	vec4( 3.0, 0.0, 0.0, 10.0/133.0),
	vec4( 4.0, 0.0, 0.0,  6.0/133.0),
	vec4( 5.0, 0.0, 0.0,  3.0/133.0)
);

const float texScaler =  1.0 / 512.0;
const float texOffset = -0.5 / 512.0;
