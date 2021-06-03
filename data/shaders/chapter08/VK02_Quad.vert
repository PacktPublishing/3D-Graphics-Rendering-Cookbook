//
#version 460

layout(location = 0) out vec2 texCoord;

vec2 positions[4] = vec2[](
    vec2(1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0)
);

int indices[6] = int[] ( 0, 1, 2, 0, 2, 3 );

vec2 texcoords[4] = vec2[](
	vec2(1.0f, 1.0f),
	vec2(1.0f, 0.0f),
	vec2(0.0f, 0.0f),
	vec2(0.0f, 1.0f)
);

void main()
{
	int idx = indices[gl_VertexIndex];
	gl_Position = vec4(positions[idx], 0.0, 1.0);
	texCoord  = texcoords[idx];
}
