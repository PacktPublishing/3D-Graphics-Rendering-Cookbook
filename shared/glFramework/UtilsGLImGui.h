#pragma once

#include "shared/glFramework/GLShader.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

const GLchar* shaderCodeImGuiVertex = R"(
	#version 460 core
	layout (location = 0) in vec2 Position;
	layout (location = 1) in vec2 UV;
	layout (location = 2) in vec4 Color;
	layout(std140, binding = 7) uniform PerFrameData
	{
		uniform mat4 MVP;
	};
	out vec2 Frag_UV;
	out vec4 Frag_Color;
	void main()
	{
		Frag_UV = UV;
		Frag_Color = Color;
		gl_Position = MVP * vec4(Position.xy,0,1);
	}
)";

const GLchar* shaderCodeImGuiFragment = R"(
	#version 460 core
	in vec2 Frag_UV;
	in	vec4 Frag_Color;
	layout (binding = 0) uniform sampler2D Texture;
	layout (location = 0) out vec4 out_FragColor;
	void main()
	{
		out_FragColor = Frag_Color * texture(Texture, Frag_UV.st);
	}
)";

class ImGuiGLRenderer
{
public:
	ImGuiGLRenderer()
	{
		glCreateVertexArrays(1, &vao_);

		glVertexArrayElementBuffer(vao_, elements_.getHandle());
		glVertexArrayVertexBuffer(vao_, 0, vertices_.getHandle(), 0, sizeof(ImDrawVert));

		glEnableVertexArrayAttrib(vao_, 0);
		glEnableVertexArrayAttrib(vao_, 1);
		glEnableVertexArrayAttrib(vao_, 2);

		glVertexArrayAttribFormat(vao_, 0, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, pos));
		glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, uv));
		glVertexArrayAttribFormat(vao_, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, IM_OFFSETOF(ImDrawVert, col));

		glVertexArrayAttribBinding(vao_, 0, 0);
		glVertexArrayAttribBinding(vao_, 1, 0);
		glVertexArrayAttribBinding(vao_, 2, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, 7, perFrameDataBuffer_.getHandle());

		defaultInitImGui();
	}
	~ImGuiGLRenderer()
	{
		glDeleteVertexArrays(1, &vao_);
		glDeleteTextures(1, &texture_);
	}
	void render(int width, int height, const ImDrawData* draw_data)
	{
		if (!draw_data)
			return;

		glBindTextures(0, 1, &texture_);
		glBindVertexArray(vao_);
		program_.useProgram();

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);

		const float L = draw_data->DisplayPos.x;
		const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		const float T = draw_data->DisplayPos.y;
		const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		const glm::mat4 orthoProjection = glm::ortho(L, R, B, T);

		glNamedBufferSubData(perFrameDataBuffer_.getHandle(), 0, sizeof(glm::mat4), glm::value_ptr(orthoProjection));

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			glNamedBufferSubData(vertices_.getHandle(), 0, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), cmd_list->VtxBuffer.Data);
			glNamedBufferSubData(elements_.getHandle(), 0, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), cmd_list->IdxBuffer.Data);

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				const ImVec4 cr = pcmd->ClipRect;
				glScissor((int)cr.x, (int)(height - cr.w), (int)(cr.z - cr.x), (int)(cr.w - cr.y));
				glBindTextureUnit(0, (GLuint)(intptr_t)pcmd->TextureId);
				glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT,
					(void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
			}
		}

		glScissor(0, 0, width, height);
		glDisable(GL_SCISSOR_TEST);
	}

private:
	void defaultInitImGui()
	{
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

		// Build texture atlas
		ImFontConfig cfg = ImFontConfig();
		cfg.FontDataOwnedByAtlas = false;
		cfg.RasterizerMultiply = 1.5f;
		cfg.SizePixels = 768.0f / 32.0f;
		cfg.PixelSnapH = true;
		cfg.OversampleH = 4;
		cfg.OversampleV = 4;
		ImFont* Font = io.Fonts->AddFontFromFileTTF("data/OpenSans-Light.ttf", cfg.SizePixels, &cfg);

		unsigned char* pixels = nullptr;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		glCreateTextures(GL_TEXTURE_2D, 1, &texture_);
		glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(texture_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(texture_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(texture_, 1, GL_RGBA8, width, height);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(texture_, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		io.Fonts->TexID = (ImTextureID)(intptr_t)texture_;
		io.FontDefault = Font;
		io.DisplayFramebufferScale = ImVec2(1, 1);
	}

private:
	GLuint texture_ = 0;
	GLuint vao_ = 0;
	GLBuffer vertices_ = GLBuffer(128 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT);
	GLBuffer elements_ = GLBuffer(256 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT);
	GLShader vertex_ = GLShader(GL_VERTEX_SHADER, shaderCodeImGuiVertex);
	GLShader fragment_ = GLShader(GL_FRAGMENT_SHADER, shaderCodeImGuiFragment);
	GLProgram program_ = GLProgram(vertex_, fragment_);
	GLBuffer perFrameDataBuffer_ = GLBuffer(sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
};

void imguiTextureWindowGL(const char* title, uint32_t texId)
{
	ImGui::Begin(title, nullptr);

	const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	const ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	ImGui::Image(
		(void*)(intptr_t)texId,
		ImVec2(vMax.x - vMin.x, vMax.y - vMin.y),
		ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)
	);

	ImGui::End();
}
