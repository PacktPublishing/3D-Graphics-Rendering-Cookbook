//
#version 460

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec4 inShadowCoord;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBuffer
{
	mat4 mvp;
	mat4 model;
	mat4 lightMVP;
	vec4 cameraPos;
	vec4 lightPos;
	float meshScale;
} ubo;

layout (binding = 3) uniform sampler2D textureShadow;
layout (binding = 4) uniform sampler2D textureAlbedo;

float PCF(int kernelSize, vec2 shadowCoord, float depth)
{
	float size = 1.0 / float( textureSize(textureShadow, 0 ).x );
	float shadow = 0.0;
	int range = kernelSize / 2;
	for ( int v=-range; v<=range; v++ ) for ( int u=-range; u<=range; u++ )
		shadow += (depth >= texture( textureShadow, shadowCoord + size * vec2(u, v) ).r) ? 1.0 : 0.0;
	return shadow / (kernelSize * kernelSize);
}

float shadowFactor(vec4 shadowCoord)
{
	vec4 shadowCoords4 = shadowCoord / shadowCoord.w;

	if (shadowCoords4.z > -1.0 && shadowCoords4.z < 1.0)
	{
		float depthBias = -0.001;
		float shadowSample = PCF( 13, shadowCoords4.xy, shadowCoords4.z + depthBias );
		return mix(1.0, 0.3, shadowSample);
	}

	return 1.0; 
}

void main()
{
	outColor = vec4(shadowFactor(inShadowCoord) * texture(textureAlbedo, uv).rgb, 1.0);
}
