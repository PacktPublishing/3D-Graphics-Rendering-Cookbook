/**/
#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 out_FragColor;

layout(binding = 0) uniform sampler2D texScene;

struct TransparentFragment
{
	vec4 color;
	float depth;
	uint next;
};

layout (binding = 0, r32ui) uniform uimage2D heads;

layout (std430, binding = 3) buffer Lists
{
    TransparentFragment fragments[];
};

void main()
{
#define MAX_FRAGMENTS 64
	TransparentFragment frags[64];

	int numFragments = 0;
	uint idx = imageLoad(heads, ivec2(gl_FragCoord.xy)).r;

	// copy the linked list for this fragment into an array
	while (idx != 0xFFFFFFFF && numFragments < MAX_FRAGMENTS)
	{
		frags[numFragments] = fragments[idx];
		numFragments++;
		idx = fragments[idx].next;
	}

	// sort the array by depth using insertion sort (largest to smallest)
	for (int i = 1; i < numFragments; i++) {
		TransparentFragment toInsert = frags[i];
		uint j = i;
		while (j > 0 && toInsert.depth > frags[j-1].depth) {
			frags[j] = frags[j-1];
			j--;
		}
		frags[j] = toInsert;
	}

	// get the color of the closest non-transparent object from the frame buffer
	vec4 color = texture(texScene, uv);

	// traverse the array, and combine the colors using the alpha channel
	for (int i = 0; i < numFragments; i++)
	{
		color = mix( color, vec4(frags[i].color), clamp(float(frags[i].color.a), 0.0, 1.0) );
	}

	out_FragColor = color;
}
