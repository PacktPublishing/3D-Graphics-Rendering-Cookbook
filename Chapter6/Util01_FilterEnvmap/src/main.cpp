#include <imgui/imgui.h>
#include "shared/UtilsCubemap.h"
#include "shared/vkFramework/VulkanApp.h"

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int numPoints = 1024;

void process_cubemap(const char* filename, const char* outFilename)
{
	int w, h, comp;
	const float* img = stbi_loadf(filename, &w, &h, &comp, 3);

	if (!img)
	{
		printf("Failed to load [%s] texture\n", filename); fflush(stdout);
		return;
	}

	const int dstW = 256;
	const int dstH = 128;

	std::vector<vec3> out(dstW * dstH);

	convolveDiffuse((vec3*)img, w, h, dstW, dstH, out.data(), numPoints);

	stbi_image_free((void*)img);
	stbi_write_hdr(outFilename, dstW, dstH, 3, (float*)out.data());
}

int main()
{
	process_cubemap("data/piazza_bologni_1k.hdr", "data/piazza_bologni_1k_irradiance.hdr");

	return 0;
}
