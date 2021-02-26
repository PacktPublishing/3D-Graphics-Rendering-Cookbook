#include "shared/vkFramework/GuiRenderer.h"
#include "shared/scene/Scene.h"

static constexpr uint32_t ImGuiVtxBufferSize = 512 * 1024 * sizeof(ImDrawVert);
static constexpr uint32_t ImGuiIdxBufferSize = 512 * 1024 * sizeof(uint32_t);

static void addImGuiItem(uint32_t width, uint32_t height, VkCommandBuffer commandBuffer, const ImDrawCmd* pcmd, ImVec2 clipOff, ImVec2 clipScale, int idxOffset, int vtxOffset,
	VkPipelineLayout pipelineLayout)
{
	if (pcmd->UserCallback)
		return;

	// Project scissor/clipping rectangles into framebuffer space
	ImVec4 clipRect;
	clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
	clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
	clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
	clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

	if (clipRect.x < width && clipRect.y < height && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
	{
		if (clipRect.x < 0.0f) clipRect.x = 0.0f;
		if (clipRect.y < 0.0f) clipRect.y = 0.0f;
		// Apply scissor/clipping rectangle
		const VkRect2D scissor = {
			.offset = { .x = (int32_t)(clipRect.x), .y = (int32_t)(clipRect.y) },
			.extent = { .width = (uint32_t)(clipRect.z - clipRect.x), .height = (uint32_t)(clipRect.w - clipRect.y) }
		};
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		uint32_t texture = (uint32_t)(intptr_t)pcmd->TextureId;
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(uint32_t), (const void*)&texture);

		vkCmdDraw(commandBuffer, pcmd->ElemCount, 1, pcmd->IdxOffset + idxOffset, pcmd->VtxOffset + vtxOffset);
	}
}

void GuiRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer, size_t currentImage, VkFramebuffer fb, VkRenderPass rp)
{
	beginRenderPass((rp != VK_NULL_HANDLE) ? rp : renderPass_.handle, (fb != VK_NULL_HANDLE) ? fb : framebuffer_, commandBuffer, currentImage);

	const ImDrawData* drawData = ImGui::GetDrawData();
	ImVec2 clipOff   = drawData->DisplayPos;
	ImVec2 clipScale = drawData->FramebufferScale;

	int vtxOffset = 0, idxOffset = 0;

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];

		for (int cmd = 0; cmd < cmdList->CmdBuffer.Size ; cmd++)
		{
			const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd];
			addImGuiItem(ctx_.vkDev.framebufferWidth, ctx_.vkDev.framebufferHeight, commandBuffer, pcmd, clipOff, clipScale, idxOffset, vtxOffset, pipelineLayout_);
		}
		idxOffset += cmdList->IdxBuffer.Size;
		vtxOffset += cmdList->VtxBuffer.Size;
	}

	vkCmdEndRenderPass(commandBuffer);
}

void GuiRenderer::updateBuffers(size_t currentImage)
{
	const ImDrawData* drawData = ImGui::GetDrawData();

	const float L = drawData->DisplayPos.x;
	const float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
	const float T = drawData->DisplayPos.y;
	const float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

	const mat4 inMtx = glm::ortho(L, R, T, B);
	updateUniformBuffer(currentImage, 0, sizeof(mat4), glm::value_ptr(inMtx));

	void* data = nullptr;
	vkMapMemory(ctx_.vkDev.device, storages_[currentImage].memory, 0, storages_[currentImage].size, 0, &data);

	ImDrawVert* vtx = (ImDrawVert*)data;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		memcpy(vtx, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
		vtx += cmdList->VtxBuffer.Size;
	}

	uint32_t* idx = (uint32_t*)((uint8_t*)data + ImGuiVtxBufferSize);
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		const uint16_t* src = (const uint16_t*)cmdList->IdxBuffer.Data;

		for (int j = 0; j < cmdList->IdxBuffer.Size; j++)
			*idx++ = (uint32_t)*src++;
	}

	vkUnmapMemory(ctx_.vkDev.device, storages_[currentImage].memory);
}

GuiRenderer::GuiRenderer(VulkanRenderContext& ctx, const std::vector<VulkanTexture>& textures, RenderPass renderPass):
	Renderer(ctx)
{
	ImGui::CreateContext();

	allTextures.push_back(ctx.resources.createFontTexture("data/OpenSans-Light.ttf"));
	for (auto t: textures)
		allTextures.push_back(t);

	// Buffer allocation
	const size_t imgCount = ctx.vkDev.swapchainImages.size();

	descriptorSets_.resize(imgCount);
	storages_.resize(imgCount);
	uniforms_.resize(imgCount);

	VkDeviceSize bufferSize = ImGuiVtxBufferSize + ImGuiIdxBufferSize;

	DescriptorSetInfo dsInfo = {
		.buffers = {
			uniformBufferAttachment(VulkanBuffer{},                  0,       sizeof(mat4), VK_SHADER_STAGE_VERTEX_BIT),
			storageBufferAttachment(VulkanBuffer{},                  0, ImGuiVtxBufferSize, VK_SHADER_STAGE_VERTEX_BIT),
			storageBufferAttachment(VulkanBuffer{}, ImGuiVtxBufferSize, ImGuiIdxBufferSize, VK_SHADER_STAGE_VERTEX_BIT)
		},
		.textureArrays = { fsTextureArrayAttachment(allTextures) }
	};
	descriptorSetLayout_ = ctx.resources.addDescriptorSetLayout(dsInfo);
	descriptorPool_ = ctx.resources.addDescriptorPool(dsInfo, imgCount);

	for(size_t i = 0 ; i < imgCount ; i++)
	{
		uniforms_[i] = ctx.resources.addUniformBuffer(sizeof(mat4));
		storages_[i] = ctx.resources.addStorageBuffer(bufferSize);

		dsInfo.buffers[0].buffer = uniforms_[i];
		dsInfo.buffers[1].buffer = storages_[i];
		dsInfo.buffers[2].buffer = storages_[i];

		descriptorSets_[i] = ctx.resources.addDescriptorSet(descriptorPool_, descriptorSetLayout_);
		ctx.resources.updateDescriptorSet(descriptorSets_[i], dsInfo);
	}

	const PipelineInfo pInfo = initRenderPass(PipelineInfo { .useDepth = false, .dynamicScissorState = true },
		{}, renderPass, ctx.screenRenderPass_NoDepth);

	initPipeline({ "data/shaders/chapter07/VK02_ImGui.vert", "data/shaders/chapter07/VK02_ImGui.frag" }, pInfo, 0, sizeof(uint32_t));
}

GuiRenderer::~GuiRenderer()
{
	ImGui::DestroyContext();
}

void imguiTextureWindow(const char* Title, uint32_t texId)
{
	ImGui::Begin(Title, nullptr);

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		ImGui::Image((void*)(intptr_t)texId, ImVec2(vMax.x - vMin.x, vMax.y - vMin.y));

	ImGui::End();
}

int renderSceneTree(const Scene& scene, int node)
{
	int selected = -1;
	std::string name = getNodeName(scene, node);
	std::string label = name.empty() ? (std::string("Node") + std::to_string(node)) : name;

	const int flags = (scene.hierarchy_[node].firstChild_ < 0) ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : 0;

	const bool opened = ImGui::TreeNodeEx(&scene.hierarchy_[node], flags, "%s", label.c_str());

	ImGui::PushID(node);

	if (ImGui::IsItemClicked(0))
	{
		printf("Selected node: %d (%s)\n", node, label.c_str());
		selected = node;
	}

	if (opened)
	{
		for (int ch = scene.hierarchy_[node].firstChild_; ch != -1; ch = scene.hierarchy_[ch].nextSibling_)
		{
			int subNode = renderSceneTree(scene, ch);
			if (subNode > -1)
				selected = subNode;
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
	return selected;
}
