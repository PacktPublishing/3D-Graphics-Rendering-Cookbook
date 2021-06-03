//

#extension GL_EXT_shader_explicit_arithmetic_types_int64: enable

struct ImDrawVert   { float x, y, z; float u, v; float nx, ny, nz; };
struct DrawData {
	uint mesh;
	uint material;
	uint lod;
	uint indexOffset;
	uint vertexOffset;
	uint transformIndex;
};

#include <data/shaders/chapter07/MaterialData.h>
