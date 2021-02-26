//
#version 460

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 outColor;

layout(binding = 3) uniform sampler2D texSampler;

void main()
{
//	outColor = /*vec4(uv, vv, 1.0);*/ color * texture(texSampler, uv); // vec4( uv, vv/*0.0*//*cc*/, 1.0 );
//	outColor = vec4( uv, 0, 1.0 ); //color; // /*vec4(1,0,0,1); // */vec4( uv, 0, 1.0 );

//	float vv = texture(texSampler, uv).x;
//	outColor = vec4(uv, vv, 0.5);
//	outColor = color * texture(texSampler, vec2(uv.x, 1.0 - uv.y)); // .wxyz;
	outColor = color * texture(texSampler, uv); // .wxyz;
}
