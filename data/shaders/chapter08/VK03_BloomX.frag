/**/
#version 460

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

const vec4 gaussFilter[11] = vec4[11](
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

void main()
{
	const float texScaler =  2.0 / 256.0;
	const float texOffset = -0.5 / 256.0;

	vec4 Color = vec4( 0.0, 0.0, 0.0, 0.0 );
	for ( int i = 0; i < 11; i++ )
	{
		vec2 Coord = vec2( texCoord.x + gaussFilter[i].x * texScaler + texOffset,
			texCoord.y + gaussFilter[i].y * texScaler + texOffset );

		Color += texture( texSampler, Coord ) * gaussFilter[i].w;
	}

	Color.a = 1.0; // *= 2.0;

	outColor = vec4(Color.xyz, 1.0);
}
