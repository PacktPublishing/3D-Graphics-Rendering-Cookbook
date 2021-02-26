#include "shared/vkFramework/VulkanResources.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

#include <imgui/imgui.h>

#include <gli/gli.hpp>
#include <gli/texture2d.hpp>
#include <gli/load_ktx.hpp>

VulkanResources::~VulkanResources()
{
	for (auto& t: allTextures)
	{
		destroyVulkanImage(vkDev.device, t.image);
		vkDestroySampler(vkDev.device, t.sampler, nullptr);
	}

	for (auto& b: allBuffers)
	{
		if (b.ptr != nullptr)
			vkUnmapMemory(vkDev.device, b.memory);
		vkDestroyBuffer(vkDev.device, b.buffer, nullptr);
		vkFreeMemory(vkDev.device, b.memory, nullptr);
	}

	for (auto& fb: allFramebuffers)
		vkDestroyFramebuffer(vkDev.device, fb, nullptr);

	for (auto& rp: allRenderPasses)
		vkDestroyRenderPass(vkDev.device, rp, nullptr);

	for (auto& ds: allDSLayouts)
		vkDestroyDescriptorSetLayout(vkDev.device, ds, nullptr);

	for (auto& pl: allPipelineLayouts)
		vkDestroyPipelineLayout(vkDev.device, pl, nullptr);

	for (auto& p: allPipelines)
		vkDestroyPipeline(vkDev.device, p, nullptr);

	for (auto& dpool: allDPools)
		vkDestroyDescriptorPool(vkDev.device, dpool, nullptr);
}

VulkanTexture VulkanResources::loadCubeMap(const char* fileName, uint32_t mipLevels)
{
	VulkanTexture cubemap;

	uint32_t w = 0, h = 0;

	if (mipLevels > 1)
		createMIPCubeTextureImage(vkDev, fileName, mipLevels, cubemap.image.image, cubemap.image.imageMemory, &w, &h);
	else
		createCubeTextureImage(vkDev, fileName, cubemap.image.image, cubemap.image.imageMemory, &w, &h);

	createImageView(vkDev.device, cubemap.image.image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &cubemap.image.imageView, VK_IMAGE_VIEW_TYPE_CUBE, 6, mipLevels);
	createTextureSampler(vkDev.device, &cubemap.sampler);

	cubemap.width = w;
	cubemap.height = h;

	allTextures.push_back(cubemap);
	return cubemap;
}

VulkanTexture VulkanResources::loadKTX(const char* fileName)
{
	gli::texture gliTex = gli::load_ktx(fileName);
	glm::tvec3<uint32_t> extent(gliTex.extent(0));

	VulkanTexture ktx = {
		.width = extent.x,
		.height = extent.y,
		.depth = 4
	};

	if (!createTextureImageFromData(vkDev, ktx.image.image, ktx.image.imageMemory,
		(uint8_t*)gliTex.data(0, 0, 0), ktx.width, ktx.height, VK_FORMAT_R16G16_SFLOAT))
	{
		printf("ModelRenderer: failed to load BRDF LUT texture \n");
		exit(EXIT_FAILURE);
	}

	createImageView(vkDev.device, ktx.image.image, VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, &ktx.image.imageView);
	createTextureSampler(vkDev.device, &ktx.sampler);

	allTextures.push_back(ktx);

	return ktx;
}

VulkanTexture VulkanResources::loadTexture2D(const char* filename)
{
	VulkanTexture tex;
	if (!createTextureImage(vkDev, filename, tex.image.image, tex.image.imageMemory, &tex.width, &tex.height))
	{
		printf("Cannot load %s 2D texture file\n", filename);
		exit(EXIT_FAILURE);
	}

	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	transitionImageLayout(vkDev, tex.image.image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	if (!createImageView(vkDev.device, tex.image.image, format, VK_IMAGE_ASPECT_COLOR_BIT, &tex.image.imageView))
	{
		printf("Cannot create image view for 2d texture (%s)\n", filename);
		exit(EXIT_FAILURE);
	}

	createTextureSampler(vkDev.device, &tex.sampler);
	allTextures.push_back(tex);
	return tex;
}

VulkanTexture VulkanResources::addColorTexture(int texWidth, int texHeight, VkFormat colorFormat, VkFilter minFilter, VkFilter maxFilter)
{
	const uint32_t w = (texWidth  > 0) ? texWidth  : vkDev.framebufferWidth;
	const uint32_t h = (texHeight > 0) ? texHeight : vkDev.framebufferHeight;

	VulkanTexture res =
	{
		.width = w,
		.height = h,
		.depth = 1,
		.format = colorFormat
	};

	if (!createOffscreenImage(vkDev,
		res.image.image, res.image.imageMemory,
		w, h, colorFormat, 1, 0))
	{
		printf("Cannot create color texture\n");
		exit(EXIT_FAILURE);
	}

	createImageView(vkDev.device, res.image.image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, &res.image.imageView);
	createTextureSampler(vkDev.device, &res.sampler, minFilter, maxFilter);

	transitionImageLayout(vkDev, res.image.image, colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	allTextures.push_back(res);
	return res;
}

VulkanTexture VulkanResources::addDepthTexture(int texWidth, int texHeight, VkImageLayout layout)
{
	const uint32_t w = (texWidth  > 0) ? texWidth  : vkDev.framebufferWidth;
	const uint32_t h = (texHeight > 0) ? texHeight : vkDev.framebufferHeight;

	const VkFormat depthFormat = findDepthFormat(vkDev.physicalDevice);

	VulkanTexture depth = {
		.width = w,
		.height = h,
		.depth = 1,
		.format = depthFormat
	};

	if(!createImage(vkDev.device, vkDev.physicalDevice, w, h, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth.image.image, depth.image.imageMemory))
	{
		printf("Cannot create depth texture\n");
		exit(EXIT_FAILURE);
	}

	createImageView(vkDev.device, depth.image.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &depth.image.imageView);
	transitionImageLayout(vkDev, depth.image.image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, layout/*VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL*/);

	if (!createDepthSampler(vkDev.device, &depth.sampler))
	{
		printf("Cannot create a depth sampler");
		exit(EXIT_FAILURE);
	}

	allTextures.push_back(depth);
	return depth;
}

VulkanBuffer VulkanResources::addBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool createMapping)
{
	VulkanBuffer buffer = { .buffer = VK_NULL_HANDLE, .size = 0, .memory = VK_NULL_HANDLE, .ptr = nullptr };

	if (!createSharedBuffer(vkDev, size, usage, properties, buffer.buffer, buffer.memory))
	{
		printf("Cannot allocate buffer\n");
		exit(EXIT_FAILURE);
	} else {
		buffer.size = size;
		allBuffers.push_back(buffer);
	}

	if (createMapping)
		vkMapMemory(vkDev.device, buffer.memory, 0, VK_WHOLE_SIZE, 0, &buffer.ptr);

	return buffer;
}

VulkanBuffer VulkanResources::addVertexBuffer(uint32_t indexBufferSize, const void* indexData, uint32_t vertexBufferSize, const void* vertexData)
{
	VulkanBuffer result;
	result.size = allocateVertexBuffer(vkDev, &result.buffer, &result.memory, vertexBufferSize, vertexData, indexBufferSize, indexData);
	allBuffers.push_back(result);
	return result;
}

VkDescriptorPool VulkanResources::addDescriptorPool(const DescriptorSetInfo& dsInfo, uint32_t dSetCount)
{
	uint32_t uniformBufferCount = 0;
	uint32_t storageBufferCount = 0;
	uint32_t samplerCount = static_cast<uint32_t>(dsInfo.textures.size());

	for(const auto& ta : dsInfo.textureArrays)
		samplerCount += static_cast<uint32_t>(ta.textures.size());

	for(const auto& b: dsInfo.buffers)
	{
		if (b.dInfo.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			uniformBufferCount++;
		if (b.dInfo.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			storageBufferCount++;
	}

	std::vector<VkDescriptorPoolSize> poolSizes;

	/* printf("Allocating pool[%d | %d | %d]\n", (int)uniformBufferCount, (int)storageBufferCount, (int)samplerCount); */

	if (uniformBufferCount)
		poolSizes.push_back(VkDescriptorPoolSize{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = dSetCount * uniformBufferCount });

	if (storageBufferCount)
		poolSizes.push_back(VkDescriptorPoolSize{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = dSetCount * storageBufferCount });

	if (samplerCount)
		poolSizes.push_back(VkDescriptorPoolSize{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = dSetCount * samplerCount });

	const VkDescriptorPoolCreateInfo poolInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = static_cast<uint32_t>(dSetCount),
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes = poolSizes.empty() ? nullptr : poolSizes.data()
	};

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	if (vkCreateDescriptorPool(vkDev.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		printf("Cannot allocate descriptor pool\n");
		exit(EXIT_FAILURE);
	}

	allDPools.push_back(descriptorPool);
	return descriptorPool;
}

VkPipeline VulkanResources::addComputePipeline(const char* shaderFile, VkPipelineLayout pipelineLayout)
{
	ShaderModule s;
	if (createShaderModule(vkDev.device, &s, shaderFile) == VK_NOT_READY)
	{
		printf("Unable to compile shader\n");
		exit(EXIT_FAILURE);
	}

	VkPipeline pipeline;
	VkResult res = createComputePipeline(vkDev.device, s.shaderModule, pipelineLayout, &pipeline);
	if (res != VK_SUCCESS)
	{
		printf("Cannot create compute pipeline (%d / %d)\n", res, res);
		exit(EXIT_FAILURE);
	}

	vkDestroyShaderModule(vkDev.device, s.shaderModule, nullptr);

	allPipelines.push_back(pipeline);
	return pipeline;
}

VkPipeline VulkanResources::addPipeline(VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
	const std::vector<const char*>& shaderFiles,
	const PipelineInfo& ppInfo)
{
	VkPipeline pipeline;

	if (!createGraphicsPipeline(vkDev, renderPass, pipelineLayout, shaderFiles,
		&pipeline, ppInfo.topology, ppInfo.useDepth, ppInfo.useBlending, ppInfo.dynamicScissorState, ppInfo.width, ppInfo.height, ppInfo.patchControlPoints))
	{
		printf("Cannot create graphics pipeline\n");
		exit(EXIT_FAILURE);
	}

	allPipelines.push_back(pipeline);
	return pipeline;
}

VkDescriptorSetLayout VulkanResources::addDescriptorSetLayout(const DescriptorSetInfo& dsInfo)
{
	VkDescriptorSetLayout descriptorSetLayout;

	uint32_t bindingIdx = 0;

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	for (const auto& b: dsInfo.buffers) {
		bindings.push_back(descriptorSetLayoutBinding(bindingIdx++, b.dInfo.type, b.dInfo.shaderStageFlags));
	}

	for (const auto& i: dsInfo.textures) {
		bindings.push_back(descriptorSetLayoutBinding(bindingIdx++, i.dInfo.type /*VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER*/, i.dInfo.shaderStageFlags));
	}

	for (const auto& t: dsInfo.textureArrays) {
		bindings.push_back(descriptorSetLayoutBinding(bindingIdx++, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, t.dInfo.shaderStageFlags, static_cast<uint32_t>(t.textures.size())));
	}

	const VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.size() > 0 ? bindings.data() : nullptr
	};

	if (vkCreateDescriptorSetLayout(vkDev.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		printf("Failed to create descriptor set layout\n");
		exit(EXIT_FAILURE);
	}

	allDSLayouts.push_back(descriptorSetLayout);
	return descriptorSetLayout;
}

VkDescriptorSet VulkanResources::addDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout dsLayout)
{
	VkDescriptorSet descriptorSet;

	const VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &dsLayout
	};

	if (vkAllocateDescriptorSets(vkDev.device, &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		printf("Cannot allocate descriptor set\n");
		exit(EXIT_FAILURE);
	}

	return descriptorSet;
}

/*
	This routine counts all textures in all texture arrays (if any of them are present),
	creates a list of DescriptorWrite operations with required buffer/image info structures
	and calls the vkUpdateDescriptorSets()
*/
void VulkanResources::updateDescriptorSet(VkDescriptorSet ds, const DescriptorSetInfo& dsInfo)
{
	uint32_t bindingIdx = 0;
	std::vector<VkWriteDescriptorSet> descriptorWrites;

	std::vector<VkDescriptorBufferInfo> bufferDescriptors(dsInfo.buffers.size());
	std::vector<VkDescriptorImageInfo>  imageDescriptors(dsInfo.textures.size());
	std::vector<VkDescriptorImageInfo>  imageArrayDescriptors;

	for (size_t i = 0 ; i < dsInfo.buffers.size() ; i++)
	{
		BufferAttachment b = dsInfo.buffers[i];
			
		bufferDescriptors[i] = VkDescriptorBufferInfo {
			.buffer = b.buffer.buffer,
			.offset = b.offset,
			.range  = (b.size > 0) ? b.size : VK_WHOLE_SIZE
		};

		descriptorWrites.push_back(bufferWriteDescriptorSet(ds, &bufferDescriptors[i], bindingIdx++, b.dInfo.type));
	}

	for(size_t i = 0 ; i < dsInfo.textures.size() ; i++)
	{
		VulkanTexture t = dsInfo.textures[i].texture;

		imageDescriptors[i] = VkDescriptorImageInfo {
			.sampler = t.sampler,
			.imageView = t.image.imageView,
			.imageLayout = /* t.texture.layout */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		descriptorWrites.push_back(imageWriteDescriptorSet(ds, &imageDescriptors[i], bindingIdx++));
	}

	uint32_t taOffset = 0;
	std::vector<uint32_t> taOffsets(dsInfo.textureArrays.size());
	for (size_t ta = 0 ; ta < dsInfo.textureArrays.size() ; ta++)
	{
		taOffsets[ta] = taOffset;

		for (size_t j = 0 ; j < dsInfo.textureArrays[ta].textures.size() ; j++)
		{
			VulkanTexture t = dsInfo.textureArrays[ta].textures[j];

			VkDescriptorImageInfo imageInfo = {
				.sampler = t.sampler,
				.imageView = t.image.imageView,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};

			imageArrayDescriptors.push_back(imageInfo); // item 'taOffsets[ta] + j'
		}

		taOffset += static_cast<uint32_t>(dsInfo.textureArrays[ta].textures.size());
	}

	for (size_t ta = 0 ; ta < dsInfo.textureArrays.size() ; ta++)
	{
		VkWriteDescriptorSet writeSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = ds,
			.dstBinding = bindingIdx++,
			.dstArrayElement = 0,
			.descriptorCount = static_cast<uint32_t>(dsInfo.textureArrays[ta].textures.size()),
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = imageArrayDescriptors.data() + taOffsets[ta]
		};

		descriptorWrites.push_back(writeSet);
	}

	vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

VkFramebuffer VulkanResources::addFramebuffer(RenderPass renderPass, const std::vector<VulkanTexture>& images)
{
	VkFramebuffer framebuffer;

	std::vector<VkImageView> attachments;
	for (const auto& i: images)
		attachments.push_back(i.image.imageView);

	VkFramebufferCreateInfo fbInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = renderPass.handle,
		.attachmentCount = (uint32_t)attachments.size(),
		.pAttachments = attachments.data(),
		.width = images[0].width,
		.height = images[0].height,
		.layers = 1
	};

	if (vkCreateFramebuffer(vkDev.device, &fbInfo, nullptr, &framebuffer) != VK_SUCCESS)
	{
		printf("Unable to create offscreen framebuffer\n");
		exit(EXIT_FAILURE);
	}

	allFramebuffers.push_back(framebuffer);
	return framebuffer;
}

std::vector<VkFramebuffer> VulkanResources::addFramebuffers(VkRenderPass renderPass, VkImageView depthView)
{
	std::vector<VkFramebuffer> framebuffers;
	createColorAndDepthFramebuffers(vkDev, renderPass, depthView, framebuffers);
	for (auto f : framebuffers)
		allFramebuffers.push_back(f);
	return framebuffers;
}

RenderPass VulkanResources::addFullScreenPass(bool useDepth, const RenderPassCreateInfo& ci)
{
	RenderPass result(vkDev, useDepth, ci);
	allRenderPasses.push_back(result.handle);
	return result;
}

RenderPass VulkanResources::addRenderPass(const std::vector<VulkanTexture>& outputs, const RenderPassCreateInfo& ci, bool useDepth, const std::vector<VkSubpassDependency>& extDeps)
{
	VkRenderPass renderPass;

	if (outputs.empty())
	{
		printf("Empty list of output attachments for RenderPass\n");
		exit(EXIT_FAILURE);
	}

	if (outputs.size() == 1)
	{
		printf("Creating color-only render pass\n");
		if (!createColorOnlyRenderPass(vkDev, &renderPass, ci, outputs[0].format))
		{
			printf("Unable to create offscreen color-only render pass\n");
			exit(EXIT_FAILURE);
		}
	} else
	{
		printf("Creating color/depth render pass\n");
		// TODO: update create...RenderPass to support multiple color attachments
		if (!createColorAndDepthRenderPass(vkDev, useDepth && (outputs.size() > 1), &renderPass, ci, outputs[0].format, extDeps))
		{
			printf("Unable to create offscreen render pass\n");
			exit(EXIT_FAILURE);
		}
	}

	allRenderPasses.push_back(renderPass);
	RenderPass rp;
	rp.info = ci;
	rp.handle = renderPass;
	return rp;
}

RenderPass VulkanResources::addDepthRenderPass(const std::vector<VulkanTexture>& outputs, const RenderPassCreateInfo& ci)
{
	VkRenderPass renderPass;

	if (!createDepthOnlyRenderPass(vkDev, &renderPass, ci))
	{
		printf("Unable to create offscreen render pass\n");
		exit(EXIT_FAILURE);
	}

	allRenderPasses.push_back(renderPass);
	RenderPass rp;
	rp.info = ci;
	rp.handle = renderPass;
	return rp;
//	return RenderPass { .info = ci, .handle = renderPass };
}

VkPipelineLayout VulkanResources::addPipelineLayout(VkDescriptorSetLayout dsLayout, uint32_t vtxConstSize, uint32_t fragConstSize)
{
	VkPipelineLayout pipelineLayout;
	if (!createPipelineLayoutWithConstants(vkDev.device, dsLayout, &pipelineLayout, vtxConstSize, fragConstSize))
	{
		printf("Cannot create pipeline layout\n");
		exit(EXIT_FAILURE);
	}

	allPipelineLayouts.push_back(pipelineLayout);
	return pipelineLayout;
}

VulkanTexture VulkanResources::createFontTexture(const char* fontFile)
{
	ImGuiIO& io = ImGui::GetIO();
	VulkanTexture res = { .image = { .image = VK_NULL_HANDLE } };

	// Build texture atlas
	ImFontConfig cfg = ImFontConfig();
	cfg.FontDataOwnedByAtlas = false;
	cfg.RasterizerMultiply = 1.5f;
	cfg.SizePixels = 768.0f / 32.0f;
	cfg.PixelSnapH = true;
	cfg.OversampleH = 4;
	cfg.OversampleV = 4;
	ImFont* Font = io.Fonts->AddFontFromFileTTF(fontFile, cfg.SizePixels, &cfg);

	unsigned char* pixels = nullptr;
	int texWidth = 1, texHeight = 1;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &texWidth, &texHeight);

	if (!pixels || !createTextureImageFromData(vkDev, res.image.image, res.image.imageMemory, pixels, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM))
	{
		printf("Failed to load texture\n");
		return res;
	}

	createImageView(vkDev.device, res.image.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &res.image.imageView);
	createTextureSampler(vkDev.device, &res.sampler);

	/* This is not strictly necessary, a font can be any texture */
	io.Fonts->TexID = (ImTextureID)0;
	io.FontDefault = Font;
	io.DisplayFramebufferScale = ImVec2(1, 1);

	allTextures.push_back(res);
	return res;
}

/* Helper mesh-related functions */
std::pair<BufferAttachment, BufferAttachment> VulkanResources::makeMeshBuffers(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
{
	const uint32_t indexBufferSize = uint32_t(indices.size() * sizeof(int));
	const uint32_t vertexBufferSize = uint32_t(vertices.size() * sizeof(float));

	VulkanBuffer storageBuffer = addVertexBuffer(indexBufferSize, indices.data(), vertexBufferSize, vertices.data());

	BufferAttachment vertexBufferAttachment { .dInfo = { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT }, .buffer = storageBuffer, .offset = 0, .size = vertexBufferSize };
	BufferAttachment indexBufferAttachment  { .dInfo = { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT }, .buffer = storageBuffer, .offset = vertexBufferSize, .size = indexBufferSize };

	return { vertexBufferAttachment, indexBufferAttachment };
}

std::pair<BufferAttachment, BufferAttachment> VulkanResources::loadMeshToBuffer(const char* filename, bool useTextureCoordinates, bool useNormals,
	std::vector<float>& vertices,
	std::vector<unsigned int>& indices)
{
	const aiScene* scene = aiImportFile( filename, aiProcess_Triangulate );

	if ( !scene || !scene->HasMeshes() )
	{
		printf( "Unable to load %s\n", filename );
		VulkanBuffer nullBuffer{ VK_NULL_HANDLE, 0, VK_NULL_HANDLE };

		return std::pair { BufferAttachment { DescriptorInfo {} , nullBuffer } , BufferAttachment { DescriptorInfo {}, nullBuffer } };
	}

	const aiMesh* mesh = scene->mMeshes[0];

	for (unsigned i = 0; i != mesh->mNumVertices; i++)
	{
		const aiVector3D v = mesh->mVertices[i];
		const aiVector3D t = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i] : aiVector3D();
		const aiVector3D n = mesh->mNormals ? mesh->mNormals[i] : aiVector3D();

		vertices.push_back(v.x);
		vertices.push_back(v.y);
		vertices.push_back(v.z);

		if (useTextureCoordinates)
		{
			vertices.push_back(t.x);
			vertices.push_back(t.y);
		}

		if (useNormals)
		{
			vertices.push_back(n.x);
			vertices.push_back(n.y);
			vertices.push_back(n.z);
		}
	}

	for ( unsigned i = 0; i != mesh->mNumFaces; i++ )
	{
		for ( unsigned j = 0; j != 3; j++ )
			indices.push_back( mesh->mFaces[i].mIndices[j] );
	}
	aiReleaseImport( scene );

	const uint32_t vertexBufferSize = static_cast<uint32_t>(sizeof(float) * vertices.size());
	const uint32_t indexBufferSize  = static_cast<uint32_t>(sizeof(unsigned int) * indices.size());

	return makeMeshBuffers(vertices, indices);
}

std::pair<BufferAttachment, BufferAttachment> VulkanResources::createPlaneBuffer_XZ()
{
	return makeMeshBuffers(
		std::vector<float> {
			-1, 0, -1,   0, 0,  0, 1, 0,
			+1, 0, -1,   1, 0,  0, 1, 0,
			+1, 0, +1,   1, 1,  0, 1, 0,
			-1, 0, +1,   0, 1,  0, 1, 0
		},
		std::vector<unsigned int> { 0u, 1u, 2u, 0u, 3u, 2u });
}

std::pair<BufferAttachment, BufferAttachment> VulkanResources::createPlaneBuffer_XY()
{
	return makeMeshBuffers(
		std::vector<float> {
			-1, -1, 0, 0, 0, 0, 0, 1,
			+1, -1, 0, 1, 0, 0, 0, 1,
			+1, +1, 0, 1, 1, 0, 0, 1,
			-1, +1, 0, 0, 1, 0, 0, 1
		},
		std::vector<unsigned int> { 0u, 1u, 2u, 0u, 3u, 2u });
}
