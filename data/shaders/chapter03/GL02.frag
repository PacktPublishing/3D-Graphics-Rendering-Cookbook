//
#version 460 core

layout (location=0) in vec2 uvs;
layout (location=1) in vec3 barycoords;

layout (location=0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D texture0;

float edgeFactor(float thickness)
{
	vec3 a3 = smoothstep( vec3( 0.0 ), fwidth(barycoords) * thickness, barycoords);
	return min( min( a3.x, a3.y ), a3.z );
}

void main()
{
	vec4 color = texture(texture0, uvs);
	out_FragColor = mix( color * vec4(0.8), color, edgeFactor(1.0) );
};
