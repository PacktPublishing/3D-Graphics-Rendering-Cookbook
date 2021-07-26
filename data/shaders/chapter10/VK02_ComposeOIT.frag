/**/
#version 460

layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UniformBuffer { uint width; uint height; } ubo;

struct TransparentFragment {
	vec4 color;
	float depth;
	uint next;
};

layout (binding = 1) buffer Heads { uint heads[] ; };

layout (binding = 2) buffer Lists { TransparentFragment fragments[]; };

layout (binding = 3) uniform sampler2D texScene;

void main()
{
#define MAX_FRAGMENTS 64
	TransparentFragment frags[64];

	int numFragments = 0;
	uint headIdx = heads[uint(gl_FragCoord.y) * ubo.width + uint(gl_FragCoord.x)]; // imageLoad(heads, ivec2(gl_FragCoord.xy)).r;
	uint idx = headIdx;

//	vec4 idxColor = unpackUnorm4x8(idx);

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
	vec4 color = texture(texScene, vec2(uv.x, 1.0 - uv.y));

	// traverse the array, and combine the colors using the alpha channel
	for (int i = 0; i < numFragments; i++)
	{
		color = mix( color, vec4(frags[i].color), clamp(float(frags[i].color.a), 0.0, 1.0) );
	}

	outColor = vec4(color.xyz, 1.0);
//	outColor = vec4(numFragments > 0 ? vec3(1, 0, 0) : color.xyz, 1.0);
//	outColor = vec4(uv, 0.0, 1.0);
//	outColor = vec4(idxColor.xyz, 1.0);
//	outColor = vec4(fragments[headIdx].color.xyz, 1.0);
}
