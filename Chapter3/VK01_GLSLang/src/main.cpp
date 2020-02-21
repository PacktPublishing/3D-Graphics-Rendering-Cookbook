#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "shared/Utils.h"
#include "shared/UtilsVulkan.h"

void saveSPIRVBinaryFile(const char* filename, unsigned int* code, size_t size)
{
	FILE* f = fopen(filename, "w");

	if (!f)
		return;

	fwrite(code, sizeof(uint32_t), size, f);
	fclose(f);
}

void testShaderCompilation(const char* sourceFilename, const char* destFilename)
{
	ShaderModule shaderModule;

	if (compileShaderFile(sourceFilename, shaderModule) < 1)
		return;

	saveSPIRVBinaryFile(destFilename, shaderModule.SPIRV.data(), shaderModule.SPIRV.size());
}

/*
This program should give the same result as the following commands:

	glslangValidator -V110 --target-env spirv1.3 VK01.vert -o VK01.vert.bin
	glslangValidator -V110 --target-env spirv1.3 VK01.frag -o VK01.frag.bin
*/
int main()
{
	glslang_initialize_process();

	testShaderCompilation("data/shaders/chapter03/VK01.vert", "VK01.vert.bin");
	testShaderCompilation("data/shaders/chapter03/VK01.frag", "VK01.frag.bin");

	glslang_finalize_process();

	return 0;
}
