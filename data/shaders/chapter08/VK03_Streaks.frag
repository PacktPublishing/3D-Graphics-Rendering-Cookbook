/**/
#version 460

layout(location = 0) in  vec2 texCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texColor;
layout(binding = 1) uniform sampler2D texRotationPattern;

void main()
{
	const float texOffset = 1.0 / 256.0;
	const int numStreaks = 4;

	int streakSamples = 7;
	float attenuation = 0.94;
	int iteration = 1;

	vec4 color = vec4( 0.0 );
	float b = pow( float(streakSamples), float(iteration) );
	vec2 streakDirection = texture( texRotationPattern, texCoord ).xy;

	for ( int k = 0; k < numStreaks; k++ )
	{
		vec4 cOut = vec4( 0.0 );

		for ( int s = 0; s < streakSamples; s++)
		{
			float weight = pow(attenuation, b * float(s));

			// Streak direction is a 2D vector in image space
			vec2 texCoordSample = texCoord + (streakDirection * b * float(s) * texOffset);

			// Scale and accumulate
			cOut += clamp(weight, 0.0, 1.0) * texture( texColor, texCoordSample ) / 4.0;
		}

		color = max( color, cOut );

		// rotate streak 90 degrees
		streakDirection = vec2( -streakDirection.y, streakDirection.x );
	}

	outColor = vec4(color.rgb, 1.0);
}
