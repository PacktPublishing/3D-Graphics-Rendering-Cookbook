#include "shared/scene/VtxData.h"

#include <algorithm>
#include <stdio.h>

MeshFileHeader loadMeshData(const char* meshFile, std::vector<Mesh>& meshes, std::vector<uint32_t>& indexData, std::vector<float>& vertexData)
{
	MeshFileHeader header;

	FILE* f = fopen(meshFile, "rb");

	if (fread(&header, 1, sizeof(header), f) != sizeof(header))
	{
		printf("Unable to read mesh file header\n");
		exit(255);
	}

	meshes.resize(header.meshCount);
	if (fread(meshes.data(), sizeof(Mesh), header.meshCount, f) != header.meshCount)
	{
		printf("Could not read mesh descriptors\n");
		exit(255);
	}

	indexData.resize(header.indexDataSize / sizeof(uint32_t));
	vertexData.resize(header.vertexDataSize / sizeof(float));

	if ((fread(indexData.data(), 1, header.indexDataSize, f) != header.indexDataSize) ||
		(fread(vertexData.data(), 1, header.vertexDataSize, f) != header.vertexDataSize))
	{
		printf("Unable to read index/vertex data\n");
		exit(255);
	}

	fclose(f);

	return header;
}

void saveBoundingBoxes(const char* fileName, const std::vector<BoundingBox>& boxes)
{
	FILE* f = fopen(fileName, "wb");

	if (!f)
	{
		printf("Error opening bounding boxes file for writing\n");
		exit(255);
	}

	uint32_t sz = (uint32_t)boxes.size();
	fwrite(&sz, 1, sizeof(sz), f);
	fwrite(boxes.data(), sz, sizeof(BoundingBox), f);

	fclose(f);
}

void loadBoundingBoxes(const char* fileName, std::vector<BoundingBox>& boxes)
{
	FILE* f = fopen(fileName, "rb");

	if (!f)
	{
		printf("Error opening bounding boxes file\n");
		exit(255);
	}

	uint32_t sz;
	fread(&sz, 1, sizeof(sz), f);

	// TODO: check file size, divide by bounding box size
	boxes.resize(sz);
	fread(boxes.data(), sz, sizeof(BoundingBox), f);

	fclose(f);
}

BoundingBox calculateBoundingBox(const float* vertices, uint32_t vertexCount, uint32_t numElementsPerVertex)
{
	BoundingBox box;
	for (int i = 0 ; i < 3 ; i++) {
		box.min[i] = +std::numeric_limits<float>::max();
		box.max[i] = -std::numeric_limits<float>::max();
	}

	for (size_t i = 0 ; i < vertexCount ; i++)
		for (size_t k = 0 ; k < 3 ; k++) {
			box.min[k] = std::min(box.min[k], vertices[i * numElementsPerVertex + k]);
			box.max[k] = std::max(box.max[k], vertices[i * numElementsPerVertex + k]);
		}

	return box;
}
