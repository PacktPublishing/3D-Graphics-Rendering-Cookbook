#include "shared/vkFramework/MultiRenderer.h"

VKSceneData::VKSceneData(VulkanRenderContext& ctx,
	const char* meshFile,
	const char* sceneFile,
	const char* materialFile,
	VulkanTexture envMap,
	VulkanTexture irradienceMap)
: ctx(ctx)
, envMapIrradience_(irradienceMap)
, envMap_(envMap)
{
	brdfLUT_ = ctx.resources.loadKTX("data/brdfLUT.ktx");

	std::vector<std::string> textureFiles;
	loadMaterials(materialFile, materials_, textureFiles);

	std::vector<VulkanTexture> textures;
	for (const auto& f: textureFiles)
		textures.push_back(ctx.resources.loadTexture2D(f.c_str()));

	allMaterialTextures = fsTextureArrayAttachment(textures);

	const uint32_t materialsSize = static_cast<uint32_t>(sizeof(MaterialDescription) * materials_.size());
	material_ = ctx.resources.addStorageBuffer(materialsSize);
	uploadBufferData(ctx.vkDev, material_.memory, 0, materials_.data(), materialsSize);

	loadMeshes(meshFile);
	loadScene(sceneFile);
}

void VKSceneData::loadMeshes(const char* meshFile)
{
	std::vector<uint32_t> indexData;
	std::vector<float> vertexData;
	MeshFileHeader header = loadMeshData(meshFile, meshes_, indexData, vertexData);

	uint32_t indexBufferSize = header.indexDataSize;
	uint32_t vertexBufferSize = header.vertexDataSize;

	const uint32_t offsetAlignment = getVulkanBufferAlignment(ctx.vkDev);
	if ((vertexBufferSize & (offsetAlignment - 1)) != 0)
	{
		const size_t numFloats = (offsetAlignment - (vertexBufferSize & (offsetAlignment - 1))) / sizeof(float);
		for (size_t i = 0; i != numFloats; i++)
			vertexData.push_back(0);
		vertexBufferSize = (vertexBufferSize + offsetAlignment) & ~(offsetAlignment - 1);
	}

	VulkanBuffer storage = ctx.resources.addStorageBuffer(vertexBufferSize + indexBufferSize);
	uploadBufferData(ctx.vkDev, storage.memory, 0, vertexData.data(), vertexBufferSize);
	uploadBufferData(ctx.vkDev, storage.memory, vertexBufferSize, indexData.data(), indexBufferSize);

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
				.indexOffset = meshes_[c.second].indexOffset,
				.vertexOffset = meshes_[c.second].vertexOffset,
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
	RenderPass screenRenderPass)
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
	const uint32_t uniformBufferSize = sizeof(mat4) + sizeof(vec4);

	std::vector<TextureAttachment> textureAttachments;
	if (sceneData_.envMap_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.envMap_));
	if (sceneData_.envMapIrradience_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.envMapIrradience_));
	if (sceneData_.brdfLUT_.width)
		textureAttachments.push_back(fsTextureAttachment(sceneData_.brdfLUT_));

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
	updateUniformBuffer((uint32_t)imageIndex, 0, sizeof(glm::mat4), glm::value_ptr(proj_ * view_ * model_));
	updateUniformBuffer((uint32_t)imageIndex, sizeof(glm::mat4), 4 * sizeof(float), glm::value_ptr(cameraPos_));
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
			.vertexCount = sceneData_.meshes_[j].getLODIndicesCount(lod),
			.instanceCount = visibility ? (visibility[i] ? 1u : 0u) : 1u,
			.firstVertex = 0,
			.firstInstance = i
		};
	}
	vkUnmapMemory(ctx_.vkDev.device, indirect_[currentImage].memory);
}
