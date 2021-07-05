//
#version 460 core

const vec2 pos[4] = vec2[4](
	vec2(-1.0,-1.0),
	vec2( 1.0,-1.0),
	vec2( 1.0, 1.0),
	vec2(-1.0, 1.0)
);

const int indices[6] = int[6]( 0, 1, 2, 2, 3, 0 );

void main()
{
	gl_Position = vec4(pos[indices[gl_VertexIndex]], 0.5, 1.0);
}
