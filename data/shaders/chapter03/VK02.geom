//
#version 460

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

layout (location=0) in vec3 color[];
layout (location=1) in vec2 uvs[];
layout (location=0) out vec3 fragColor;
layout (location=1) out vec3 barycoords;
layout (location=2) out vec2 uv;

void main()
{
	const vec3 bc[3] = vec3[]
	(
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);
	for ( int i = 0; i < 3; i++ )
	{
		gl_Position = gl_in[i].gl_Position;
		fragColor = color[i];
		barycoords = bc[i];
		uv = uvs[i];

		EmitVertex();
	}

	EndPrimitive();
}
