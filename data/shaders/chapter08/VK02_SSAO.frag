/**/
#version 460

layout(location = 0) in vec2 texCoord1;

layout(location = 0) out vec4 outColor;

const vec3 offsets[8] = vec3[8] 
(
	vec3(-0.5, -0.5, -0.5),
	vec3( 0.5, -0.5, -0.5),
	vec3(-0.5,  0.5, -0.5),
	vec3( 0.5,  0.5, -0.5),
	vec3(-0.5, -0.5,  0.5),
	vec3( 0.5, -0.5,  0.5),
	vec3(-0.5,  0.5,  0.5),
	vec3( 0.5,  0.5,  0.5)
);

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

layout(binding = 1) uniform sampler2D texDepth;
layout(binding = 2) uniform sampler2D texRotation;

// based on http://steps3d.narod.ru/tutorials/ssao-tutorial.html
void main()
{
	vec2 uv = vec2(texCoord1.x, 1.0 - texCoord1.y);

	const float radius    = params.radius;
	const float attScale  = params.attScale;
	const float distScale = params.distScale;
    
	const float zFarMulzNear   = params.zFar * params.zNear;
	const float zFarMinuszNear = params.zFar - params.zNear;
    
	float size = 1.0 / 512.0; // float(textureSize(texDepth, 0 ).x);

	// get Z in eye space
	float Z     = zFarMulzNear / ( texture( texDepth, uv ).x * zFarMinuszNear - params.zFar );
	float att   = 0.0;
	vec3  plane = 2.0 * texture( texRotation, uv * size / 4.0 ).xyz - vec3( 1.0 );
  
	for ( int i = 0; i < 8; i++ )
	{
		vec3  rSample = reflect( offsets[i], plane );
		float zSample = texture( texDepth, uv + radius*rSample.xy / Z ).x;

		zSample = zFarMulzNear / ( zSample * zFarMinuszNear - params.zFar );
        
		float dist = max( zSample - Z, 0.0 ) / distScale;
		float occl = 15.0 * max( dist * (2.0 - dist), 0.0 );
        
		att += 1.0 / ( 1.0 + occl * occl );
	}
    
	att = clamp( att * att / 64.0 + 0.45, 0.0, 1.0 ) * attScale;
	outColor = vec4 ( vec3(att), 1.0 );
}
