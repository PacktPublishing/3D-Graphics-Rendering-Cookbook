#include "shared/vkFramework/MultiRenderer.h"

#include <stb/stb_image.h>

uint8_t* genDefaultCheckerboardImage(int* width, int* height);

VKSceneData::VKSceneData(VulkanRenderContext& ctx,
	const char* meshFile,
	const char* sceneFile,
	const char* materialFile,
	VulkanTexture envMap,
	VulkanTexture irradianceMap,
	bool asyncLoad)
: ctx(ctx)
, envMapIrradiance_(irradianceMap)
, envMap_(envMap)
{
	brdfLUT_ = ctx.resources.loadKTX("data/brdfLUT.ktx");

	loadMaterials(materialFile, materials_, textureFiles_);

	std::vector<VulkanTexture> textures;
	for (const auto& f: textureFiles_) {
		auto t = asyncLoad ? ctx.resources.addSolidRGBATexture() : ctx.resources.loadTexture2D(f.c_str());
		textures.push_back(t);
#if 0
		if (t.image.image != nullptr)
			setVkImageName(ctx.vkDev, t.image.image, f.c_str());
#endif
	}

	if (asyncLoad)
	{
		loadedFiles_.reserve(textureFiles_.size());

		taskflow_.for_each_index(0u, (uint32_t)textureFiles_.size(), 1u, [this](int idx)
			{
				int w, h;
				const uint8_t* img = stbi_load(this->textureFiles_[idx].c_str(), &w, &h, nullptr, STBI_rgb_alpha);
				if (!img)
					img = genDefaultCheckerboardImage(&w, &h);
				std::lock_guard lock(loadedFilesMutex_);
				loadedFiles_.emplace_back(LoadedImageData { idx, w, h, img });
			}
		);

		executor_.run(taskflow_);
	}

	allMaterialTextures = fsTextureArrayAttachment(textures);

	const uint32_t materialsSize = static_cast<uint32_t>(sizeof(MaterialDescription) * materials_.size());
	material_ = ctx.resources.addStorageBuffer(materialsSize);
	uploadBufferData(ctx.vkDev, material_.memory, 0, materials_.data(), materialsSize);

	loadMeshes(meshFile);
	loadScene(sceneFile);
}

void VKSceneData::loadMeshes(const char* meshFile)
{
	MeshFileHeader header = loadMeshData(meshFile, meshData_);

	const uint32_t indexBufferSize = header.indexDataSize;
	uint32_t vertexBufferSize = header.vertexDataSize;

	const uint32_t offsetAlignment = getVulkanBufferAlignment(ctx.vkDev);
	if ((vertexBufferSize & (offsetAlignment - 1)) != 0)
	{
		const size_t numFloats = (offsetAlignment - (vertexBufferSize & (offsetAlignment - 1))) / sizeof(float);
		for (size_t i = 0; i != numFloats; i++)
			meshData_.vertexData_.push_back(0);
		vertexBufferSize = (vertexBufferSize + offsetAlignment) & ~(offsetAlignment - 1);
	}

	VulkanBuffer storage = ctx.resources.addStorageBuffer(vertexBufferSize + indexBufferSize);
	uploadBufferData(ctx.vkDev, storage.memory, 0, meshData_.vertexData_.data(), vertexBufferSize);
	uploadBufferData(ctx.vkDev, storage.memory, vertexBufferSize, meshData_.indexData_.data(), indexBufferSize);

	vertexBuffer_ = BufferAttachment { .dInfo = { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT }, .buffer = storage, .offset = 0, .size = vertexBufferSize };
	indexBuffer_  = BufferAttachment { .dInfo = { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT }, .buffer = storage, .offset = vertexBufferSize, .size = indexBufferSize };
}

void VKSceneData::loadScene(const char* sceneFile)
{
	::loadScene(sceneFile, scene_);

	// prepare draw data buffer
	for (const auto& c : scene_.meshes_)
	{
		auto material = scene_.materialForNode_.find(c.first);
		if (material == scene_.materialForNode_.end())
			continue;

		shapes_.push_back(
			DrawData{
				.meshIndex = c.second,
				.materialIndex = material->second,
				.LOD = 0,
				.indexOffset = meshData_.meshes_[c.second].indexOffset,
				.vertexOffset = meshData_.meshes_[c.second].vertexOffset,
				.transformIndex = c.first
			});
	}

	shapeTransforms_.resize(shapes_.size());
	transforms_ = ctx.resources.addStorageBuffer(shapes_.size() * sizeof(glm::mat4));

	recalculateAllTransforms();
	uploadGlobalTransforms();
}

void VKSceneData::updateMaterial(int matIdx)
{
	uploadBufferData(ctx.vkDev, material_.memory, matIdx * sizeof(MaterialDescription), materials_.data() + matIdx, sizeof(MaterialDescription));
}

void VKSceneData::convertGlobalToShapeTransforms()
{
	// fill the shapeTransforms_ array from globalTransforms_
	size_t i = 0;
	for (const auto& c : shapes_)
		shapeTransforms_[i++] = scene_.globalTransform_[c.transformIndex];
}

void VKSceneData::recalculateAllTransforms()
{
	// force recalculation of global transformations
	markAsChanged(scene_, 0);
	recalculateGlobalTransforms(scene_);
}

void VKSceneData::uploadGlobalTransforms()
{
	convertGlobalToShapeTransforms();
 	uploadBufferData(ctx.vkDev, transforms_.memory, 0, shapeTransforms_.data(), transforms_.size);
}

MultiRenderer::MultiRenderer(
	VulkanRenderContext& ctx,
	VKSceneData& sceneData,
	const char* vertShaderFile,
	const char* fragShaderFile,
	const std::vector<VulkanTexture>& outputs,
	RenderPass screenRenderPass,
	const std::vector<BufferAttachment>& auxBuffers,
	const std::vector<TextureAttachment>& auxTextures)
: Renderer(ctx)
, sceneData_(sceneData)
{
	const PipelineInfo pInfo = initRenderPass(PipelineInfo {}, outputs, screenRenderPass, ctx.screenRenderPass);

	const uint32_t indirectDataSize = (uint32_t)sceneData_.shapes_.size() * sizeof(VkDrawIndirectCommand);

	const size_t imgCount = ctx.vkDev.swapchainImages.size();
	uniforms_.resize(imgCount);
	shape_.resize(imgCount);
	indirect_.resize(imgCount);

	descriptorSets_.resize(imgCount);

	const uint32_t shapesSize = (uint32_t)sceneData_.shapes_.size() * sizeof(DrawData);
	const uint32_t uniformBufferSize = sizeof(ubo_);

	std::vector<TextureAttachment> textureAttachments;
	if (sceneData_.envMap_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.envMap_));
	if (sceneData_.envMapIrradiance_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.envMapIrradiance_));
	if (sceneData_.brdfLUT_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.brdfLUT_));

	for (const auto& t: auxTextures)
		textureAttachments.push_back(t);

	DescriptorSetInfo dsInfo = {
		.buffers = {
			uniformBufferAttachment(VulkanBuffer {},         0, uniformBufferSize, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
			sceneData_.vertexBuffer_,
			sceneData_.indexBuffer_,
			storageBufferAttachment(VulkanBuffer {},         0, shapesSize, VK_SHADER_STAGE_VERTEX_BIT),
			storageBufferAttachment(sceneData_.material_,    0, (uint32_t)sceneData_.material_.size, VK_SHADER_STAGE_FRAGMENT_BIT),
			storageBufferAttachment(sceneData_.transforms_,  0, (uint32_t)sceneData_.transforms_.size, VK_SHADER_STAGE_VERTEX_BIT),
		},
		.textures = textureAttachments,
		.textureArrays = { sceneData_.allMaterialTextures }
	};

	for (const auto& b: auxBuffers)
		dsInfo.buffers.push_back(b);

	descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
	descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, (uint32_t)imgCount);

	for (size_t i = 0; i != imgCount; i++)
	{
		uniforms_[i] = ctx.resources.addUniformBuffer(uniformBufferSize);
		indirect_[i] = ctx.resources.addIndirectBuffer(indirectDataSize);
		updateIndirectBuffers(i);

		shape_[i] = ctx.resources.addStorageBuffer(shapesSize);
		uploadBufferData(ctx.vkDev, shape_[i].memory, 0, sceneData_.shapes_.data(), shapesSize);

		dsInfo.buffers[0].buffer = uniforms_[i];
		dsInfo.buffers[3].buffer = shape_[i];

		descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
		ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
	}

	initPipeline({ vertShaderFile, fragShaderFile }, pInfo);
}

void MultiRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	/* For CountKHR (Vulkan 1.1) we may use indirect rendering with GPU-based object counter */
	/// vkCmdDrawIndirectCountKHR(commandBuffer, indirectBuffers_[currentImage], 0, countBuffers_[currentImage], 0, shapes.size(), sizeof(VkDrawIndirectCommand));
	/* For Vulkan 1.0 vkCmdDrawIndirect is enough */
	vkCmdDrawIndirect(commandBuffer, indirect_[currentImage].buffer, 0, (uint32_t)sceneData_.shapes_.size(), sizeof(VkDrawIndirectCommand));

	vkCmdEndRenderPass(commandBuffer);
}

void MultiRenderer::updateBuffers(size_t imageIndex)
{
	updateUniformBuffer((uint32_t)imageIndex, 0, sizeof(ubo_), &ubo_);
}

void MultiRenderer::updateIndirectBuffers(size_t currentImage, bool* visibility)
{
	VkDrawIndirectCommand* data = nullptr;
	vkMapMemory(ctx_.vkDev.device, indirect_[currentImage].memory, 0, sizeof(VkDrawIndirectCommand), 0, (void**)&data);

	const uint32_t size = (uint32_t)sceneData_.shapes_.size();

	for (uint32_t i = 0; i != size; i++)
	{
		const uint32_t j = sceneData_.shapes_[i].meshIndex;

		const uint32_t lod = sceneData_.shapes_[i].LOD;
		data[i] = {
			.vertexCount = sceneData_.meshData_.meshes_[j].getLODIndicesCount(lod),
			.instanceCount = visibility ? (visibility[i] ? 1u : 0u) : 1u,
			.firstVertex = 0,
			.firstInstance = i
		};
	}
	vkUnmapMemory(ctx_.vkDev.device, indirect_[currentImage].memory);
}

bool MultiRenderer::checkLoadedTextures()
{
	VKSceneData::LoadedImageData data;

	{
		std::lock_guard lock(sceneData_.loadedFilesMutex_);

		if (sceneData_.loadedFiles_.empty())
			return false;

		data = sceneData_.loadedFiles_.back();

		sceneData_.loadedFiles_.pop_back();
	}

	this->updateTexture(data.index_, ctx_.resources.addRGBATexture(data.w_, data.h_, const_cast<uint8_t*>(data.img_)));

	stbi_image_free((void*)data.img_);

	return true;
}
