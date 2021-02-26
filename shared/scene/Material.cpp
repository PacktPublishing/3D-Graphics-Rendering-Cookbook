#include "shared/scene/Material.h"

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
	fwrite(materials.data(), sz, sizeof(MaterialDescription), f);
	saveStringList(f, files);
	fclose(f);
}

void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files)
{
	FILE* f = fopen(fileName, "rb");
	if (!f)
		return;

	uint32_t sz;
	fread(&sz, 1, sizeof(uint32_t), f);
	materials.resize(sz);
	fread(materials.data(), materials.size(), sizeof(MaterialDescription), f);
	loadStringList(f, files);
	fclose(f);
}
