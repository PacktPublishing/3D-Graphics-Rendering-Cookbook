#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "shared/scene/VtxData.h"

#include <meshoptimizer.h>

std::vector<Mesh> g_meshes;

std::vector<uint32_t> g_indexData;
std::vector<float> g_vertexData;

uint32_t g_indexOffset = 0;
uint32_t g_vertexOffset = 0;

constexpr uint32_t g_numElementsToStore = 3 + 3 + 2;

float g_meshScale = 0.01f;

bool g_calculateLODs = false;

void processLods(std::vector<uint32_t>& indices, std::vector<float>& vertices, std::vector<std::vector<uint32_t>>& outLods)
{
	size_t verticesCountIn = vertices.size() / 2;
	size_t targetIndicesCount = indices.size();

	uint8_t LOD = 1;

	printf("\n   LOD0: %i indices", int(indices.size()));

	outLods.push_back(indices);

	while ( targetIndicesCount > 1024 && LOD < 8 )
	{
		targetIndicesCount = indices.size() / 2;

		bool sloppy = false;

		size_t numOptIndices = meshopt_simplify(
			indices.data(),
			indices.data(), (uint32_t)indices.size(),
			vertices.data(), verticesCountIn,
			sizeof( float ) * 3,
			targetIndicesCount, 0.02f );

		// cannot simplify further
		if (static_cast<size_t>(numOptIndices * 1.1f) > indices.size())
		{
			if (LOD > 1)
			{
				// try harder
				numOptIndices = meshopt_simplifySloppy(
					indices.data(),
					indices.data(), indices.size(),
					vertices.data(), verticesCountIn,
					sizeof(float) * 3,
					targetIndicesCount);
				sloppy = true;
				if (numOptIndices == indices.size()) break;
			}
			else
				break;
		}

		indices.resize(numOptIndices);

		meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), verticesCountIn);

		printf("\n   LOD%i: %i indices %s", int(LOD), int(numOptIndices), sloppy ? "[sloppy]" : "");

		LOD++;

		outLods.push_back(indices);
	}
}

Mesh convertAIMesh(const aiMesh* m)
{
	const bool hasTexCoords = m->HasTextureCoords(0);
	const uint32_t streamElementSize = static_cast<uint32_t>(g_numElementsToStore * sizeof(float));

	// Original data for LOD calculation
	std::vector<float> srcVertices;
	std::vector<uint32_t> srcIndices;

	std::vector<std::vector<uint32_t>> outLods;

	for (size_t i = 0; i != m->mNumVertices; i++)
	{
		const aiVector3D v = m->mVertices[i];
		const aiVector3D n = m->mNormals[i];
		const aiVector3D t = hasTexCoords ? m->mTextureCoords[0][i] : aiVector3D();

		if (g_calculateLODs)
		{
			srcVertices.push_back(v.x);
			srcVertices.push_back(v.y);
			srcVertices.push_back(v.z);
		}

		g_vertexData.push_back(v.x * g_meshScale);
		g_vertexData.push_back(v.y * g_meshScale);
		g_vertexData.push_back(v.z * g_meshScale);

		g_vertexData.push_back(t.x);
		g_vertexData.push_back(1.0f - t.y);

		g_vertexData.push_back(n.x);
		g_vertexData.push_back(n.y);
		g_vertexData.push_back(n.z);
	}

	Mesh result = {
		.streamCount = 1,
		.indexOffset = g_indexOffset,
		.vertexOffset = g_vertexOffset,
		.vertexCount = m->mNumVertices,
		.streamOffset = { g_vertexOffset * streamElementSize },
		.streamElementSize = { streamElementSize }
	};

	for (size_t i = 0; i != m->mNumFaces; i++)
	{
		if (m->mFaces[i].mNumIndices != 3)
			continue;
		for (unsigned j = 0; j != m->mFaces[i].mNumIndices; j++)
			srcIndices.push_back(m->mFaces[i].mIndices[j]);
	}

	if (!g_calculateLODs)
		outLods.push_back(srcIndices);
	else
		processLods(srcIndices, srcVertices, outLods);

	printf("\nCalculated LOD count: %u\n", (unsigned)outLods.size());

	uint32_t numIndices = 0;
	for (size_t l = 0 ; l < outLods.size() ; l++)
	{
		for (size_t i = 0 ; i < outLods[l].size() ; i++)
			g_indexData.push_back(outLods[l][i]);

		result.lodOffset[l] = numIndices;
		numIndices += (int)outLods[l].size();
	}

	result.lodOffset[outLods.size()] = numIndices;
	result.lodCount = (uint32_t)outLods.size();

	g_indexOffset  += numIndices;
	g_vertexOffset += m->mNumVertices;

	return result;
}

void loadFile(const char* fileName)
{
	printf("Loading '%s'...\n", fileName);

	const unsigned int flags = 0
		| aiProcess_JoinIdenticalVertices
		| aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_PreTransformVertices
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_FindInvalidData
		| aiProcess_FindInstances
		| aiProcess_OptimizeMeshes
	;

	const aiScene* scene = aiImportFile(fileName, flags);

	if (!scene || !scene->HasMeshes())
	{
		printf("Unable to load '%s'\n", fileName);
		exit(255);
	}

	g_meshes.reserve(scene->mNumMeshes);

	for (unsigned int i = 0; i != scene->mNumMeshes; i++)
	{
		printf("\nConverting meshes %u/%u...", i + 1, scene->mNumMeshes);
		fflush(stdout);

		g_meshes.push_back(convertAIMesh(scene->mMeshes[i]));
	}
}

inline void saveMeshesToFile(FILE* f)
{
	const MeshFileHeader header = {
		.magicValue = 0x12345678,
		.meshCount = (uint32_t)g_meshes.size(),
		.dataBlockStartOffset = (uint32_t )(sizeof(MeshFileHeader) + g_meshes.size() * sizeof(Mesh)),
		.indexDataSize = (uint32_t)(g_indexData.size() * sizeof(uint32_t)),
		.vertexDataSize = (uint32_t)(g_vertexData.size() * sizeof(float))
	};

	fwrite(&header, 1, sizeof(header), f);
	fwrite(g_meshes.data(), header.meshCount, sizeof(Mesh), f);

	fwrite(g_indexData.data(), 1, header.indexDataSize, f);
	fwrite(g_vertexData.data(), 1, header.vertexDataSize, f);
}

int main()
{
	loadFile("deps/src/bistro/Exterior/exterior.obj");

	FILE *f = fopen("data/meshes/test.meshes", "wb");
	saveMeshesToFile(f);
	fclose(f);

	std::vector<DrawData> grid;
	g_vertexOffset = 0;
	for (auto i = 0 ; i < g_meshes.size() ; i++)
	{
		grid.push_back(DrawData {
			.meshIndex = (uint32_t)i,
			.materialIndex = 0,
			.LOD = 0,
			.indexOffset = g_meshes[i].indexOffset,
			.vertexOffset = g_vertexOffset,
			.transformIndex = 0
		});
		g_vertexOffset += g_meshes[i].vertexCount;
	}

	f = fopen("data/meshes/test.meshes.drawdata", "wb");
	fwrite(grid.data(), grid.size(), sizeof(DrawData), f);
	fclose(f);

	return 0;
}
