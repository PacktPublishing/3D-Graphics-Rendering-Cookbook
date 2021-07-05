#include "shared/scene/Material.h"

#include <unordered_map>
#include "shared/Utils.h"

void saveStringList(FILE* f, const std::vector<std::string>& lines)
{
	uint32_t sz = (uint32_t)lines.size();
	fwrite(&sz, sizeof(uint32_t), 1, f);
	for (const auto& s: lines)
	{
		sz = (uint32_t)s.length();
		fwrite(&sz, sizeof(uint32_t), 1, f);
		fwrite(s.c_str(), sz + 1, 1, f);
	}
}

void loadStringList(FILE* f, std::vector<std::string>& lines)
{
	{
		uint32_t sz = 0;
		fread(&sz, sizeof(uint32_t), 1, f);
		lines.resize(sz);
	}
	std::vector<char> inBytes;
	for (auto& s: lines)
	{
		uint32_t sz = 0;
		fread(&sz, sizeof(uint32_t), 1, f);
		inBytes.resize(sz + 1);
		fread(inBytes.data(), sz + 1, 1, f);
		s = std::string(inBytes.data());
	}
}

void saveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files)
{
	FILE* f = fopen(fileName, "wb");
	if (!f)
		return;

	uint32_t sz = (uint32_t)materials.size();
	fwrite(&sz, 1, sizeof(uint32_t), f);
	fwrite(materials.data(), sizeof(MaterialDescription), sz, f);
	saveStringList(f, files);
	fclose(f);
}

void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files)
{
	FILE* f = fopen(fileName, "rb");
	if (!f) {
		printf("Cannot load file %s\nPlease run SceneConverter tool from Chapter7\n", fileName);
		exit(255);
	}

	uint32_t sz;
	fread(&sz, 1, sizeof(uint32_t), f);
	materials.resize(sz);
	fread(materials.data(), sizeof(MaterialDescription), materials.size(), f);
	loadStringList(f, files);
	fclose(f);
}

void mergeMaterialLists(
	const std::vector< std::vector<MaterialDescription>* >& oldMaterials,
	const std::vector< std::vector<std::string>* >& oldTextures,
	std::vector<MaterialDescription>& allMaterials,
	std::vector<std::string>& newTextures
)
{
	// map texture names to indices in newTexturesList (calculated as we fill the newTexturesList)
	std::unordered_map<std::string, int> newTextureNames;
	std::unordered_map<int, int> materialToTextureList; // direct MaterialDescription usage as a key is impossible, so we use its index in the allMaterials array

	// Create combined material list [no hashing of materials, just straightforward merging of all lists]
	int midx = 0;
	for (const std::vector<MaterialDescription>* ml: oldMaterials) {
		for (const MaterialDescription& m: *ml) {
			allMaterials.push_back(m);
			materialToTextureList[allMaterials.size() - 1] = midx;
		}

		midx++;
	}

	// Create one combined texture list
	for (const std::vector<std::string>* tl: oldTextures)
		for (const std::string& file: *tl) {
			newTextureNames[file] = addUnique(newTextures, file); // addUnique() is in SceneConverter/MaterialConv.inl
		}

	// Lambda to replace textureID by a new "version" (from global list)
	auto replaceTexture = [&materialToTextureList, &oldTextures, &newTextureNames](int m, uint64_t* textureID) {
		if (*textureID < INVALID_TEXTURE) {
			auto listIdx = materialToTextureList[m];
			auto texList = oldTextures[listIdx];
			const std::string& texFile = (*texList)[*textureID];
			*textureID = (uint64_t)(newTextureNames[texFile]);
		}
	};

	for (size_t i = 0 ; i < allMaterials.size() ; i++)
	{
		auto& m = allMaterials[i];
		replaceTexture(i, &m.ambientOcclusionMap_);
		replaceTexture(i, &m.emissiveMap_);
		replaceTexture(i, &m.albedoMap_);
		replaceTexture(i, &m.metallicRoughnessMap_);
		replaceTexture(i, &m.normalMap_);
	}
}
