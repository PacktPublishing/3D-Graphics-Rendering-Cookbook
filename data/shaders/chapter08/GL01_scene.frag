//
#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
	mat4 light;
	vec4 cameraPos;
	vec4 lightAngles;
	vec4 lightPos;
};

struct PerVertex
{
	vec2 uv;
	vec4 shadowCoord;
	vec3 worldPos;
};

layout (location=0) in PerVertex vtx;

layout (location=0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D texture0;
layout (binding = 1) uniform sampler2D textureShadow;

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

float lightFactor(vec3 worldPos)
{
	vec3 dirLight = normalize(lightPos.xyz - worldPos);
	vec3 dirSpot  = normalize(-lightPos.xyz); // light is always looking at (0, 0, 0)

	float rho = dot(-dirLight, dirSpot);

	float outerAngle = lightAngles.x;
	float innerAngle = lightAngles.y;

	if (rho > outerAngle)
		return smoothstep(outerAngle, innerAngle, rho);

	return 0.0;
}

void main()
{
	vec3 albedo = texture(texture0, vtx.uv).xyz;

	out_FragColor = vec4( albedo * shadowFactor(vtx.shadowCoord) * lightFactor(vtx.worldPos), 1.0 );
};
