#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

#include "shared/Utils.h"
#include "shared/UtilsMath.h"

constexpr const uint32_t kMaxLODs = 8;
constexpr const uint32_t kMaxStreams = 8;

// All offsets are relative to the beginning of the data block (excluding headers with Mesh list)
struct Mesh final
{
	/* Number of LODs in this mesh. Strictly less than MAX_LODS, last LOD offset is used as a marker only */
	uint32_t lodCount = 1;

	/* Number of vertex data streams */
	uint32_t streamCount = 0;

	/* The total count of all previous vertices in this mesh file */
	uint32_t indexOffset = 0;

	uint32_t vertexOffset = 0;

	/* Vertex count (for all LODs) */
	uint32_t vertexCount = 0;

	/* Offsets to LOD data. Last offset is used as a marker to calculate the size */
	uint32_t lodOffset[kMaxLODs] = { 0 };

	inline uint32_t getLODIndicesCount(uint32_t lod) const { return lodOffset[lod + 1] - lodOffset[lod]; }

	/* All the data "pointers" for all the streams */
	uint32_t streamOffset[kMaxStreams] = { 0 };

	/* Information about stream element (size pretty much defines everything else, the "semantics" is defined by the shader) */
	uint32_t streamElementSize[kMaxStreams] = { 0 };

	/* We could have included the streamStride[] array here to allow interleaved storage of attributes.
 	   For this book we assume tightly-packed (non-interleaved) vertex attribute streams */

	/* Additional information, like mesh name, can be added here */
};

struct MeshFileHeader
{
	/* Unique 64-bit value to check integrity of the file */
	uint32_t magicValue;

	/* Number of mesh descriptors following this header */
	uint32_t meshCount;

	/* The offset to combined mesh data (this is the base from which the offsets in individual meshes start) */
	uint32_t dataBlockStartOffset;

	/* How much space index data takes */
	uint32_t indexDataSize;

	/* How much space vertex data takes */
	uint32_t vertexDataSize;

	/* According to your needs, you may add additional metadata fields */
};

struct DrawData
{
	uint32_t meshIndex;
	uint32_t materialIndex;
	uint32_t LOD;
	uint32_t indexOffset;
	uint32_t vertexOffset;
	uint32_t transformIndex;
};

struct MeshData
{
	std::vector<uint32_t> indexData_;
	std::vector<float> vertexData_;
	std::vector<Mesh> meshes_;
	std::vector<BoundingBox> boxes_;
};

static_assert(sizeof(DrawData) == sizeof(uint32_t) * 6);
static_assert(sizeof(BoundingBox) == sizeof(float) * 6);

MeshFileHeader loadMeshData(const char* meshFile, MeshData& out);
void saveMeshData(const char* fileName, const MeshData& m);

void recalculateBoundingBoxes(MeshData& m);

// Combine a list of meshes to a single mesh container
MeshFileHeader mergeMeshData(MeshData& m, const std::vector<MeshData*> md);
