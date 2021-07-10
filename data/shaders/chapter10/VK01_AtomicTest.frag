//
#version 460 core

//layout (early_fragment_tests) in;

layout (set = 0, binding = 0) buffer Atomic { uint count; };

struct node { uint idx; float xx, yy; };

layout (set = 0, binding = 1) buffer Values { node value[]; };

layout (set = 0, binding = 2) uniform UniformBuffer { float width; float height; } ubo;

layout(location = 0) out vec4 outColor;

void main()
{
	const uint maxPixels = uint(ubo.width) * uint(ubo.height);

	// Check LinkedListSBO is full
	if (count < maxPixels && gl_HelperInvocation == false)
	{
		uint idx = atomicAdd(count, 1);

		// Exchange new head index and previous head index
		value[idx].idx = idx;
		value[idx].xx  = gl_FragCoord.x;
		value[idx].yy  = gl_FragCoord.y;
	}

	discard;
	outColor = vec4(0.0);	
}
