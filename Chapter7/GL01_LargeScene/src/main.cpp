#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "shared/glFramework/GLFWApp.h"
#include "shared/glFramework/GLShader.h"
#include "shared/glFramework/GLSceneData.h"
#include "shared/UtilsMath.h"
#include "shared/Camera.h"

#include "shared/scene/VtxData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

const GLuint kBufferIndex_PerFrameUniforms = 0;
const GLuint kBufferIndex_ModelMatrices = 1;
const GLuint kBufferIndex_Materials = 2;

struct PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
};

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
} mouseState;

CameraPositioner_FirstPerson positioner( vec3(-10.0f, 3.0f, 3.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
Camera camera(positioner);

struct DrawElementsIndirectCommand
{
	GLuint count_;
	GLuint instanceCount_;
	GLuint firstIndex_;
	GLuint baseVertex_;
	GLuint baseInstance_;
};

class GLMesh final
{
public:
	explicit GLMesh(const GLSceneData& data)
		: numIndices_(data.header_.indexDataSize / sizeof(uint32_t))
		, bufferIndices_(data.header_.indexDataSize, data.meshData_.indexData_.data(), 0)
		, bufferVertices_(data.header_.vertexDataSize, data.meshData_.vertexData_.data(), 0)
		, bufferMaterials_(sizeof(MaterialDescription) * data.materials_.size(), data.materials_.data(), 0)
		, bufferIndirect_(sizeof(DrawElementsIndirectCommand) * data.shapes_.size() + sizeof(GLsizei), nullptr, GL_DYNAMIC_STORAGE_BIT)
		, bufferModelMatrices_(sizeof(glm::mat4) * data.shapes_.size(), nullptr, GL_DYNAMIC_STORAGE_BIT)
	{
		glCreateVertexArrays(1, &vao_);
		glVertexArrayElementBuffer(vao_, bufferIndices_.getHandle());
		glVertexArrayVertexBuffer(vao_, 0, bufferVertices_.getHandle(), 0, sizeof(vec3) + sizeof(vec3) + sizeof(vec2));
		// position
		glEnableVertexArrayAttrib(vao_, 0);
		glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao_, 0, 0);
		// uv
		glEnableVertexArrayAttrib(vao_, 1);
		glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3));
		glVertexArrayAttribBinding(vao_, 1, 0);
		// normal
		glEnableVertexArrayAttrib(vao_, 2);
		glVertexArrayAttribFormat(vao_, 2, 3, GL_FLOAT, GL_TRUE, sizeof(vec3) + sizeof(vec2));
		glVertexArrayAttribBinding(vao_, 2, 0);

		std::vector<uint8_t> drawCommands;

		drawCommands.resize(sizeof(DrawElementsIndirectCommand) * data.shapes_.size() + sizeof(GLsizei));

		// store the number of draw commands in the very beginning of the buffer
		const GLsizei numCommands = (GLsizei)data.shapes_.size();
		memcpy(drawCommands.data(), &numCommands, sizeof(numCommands));

		DrawElementsIndirectCommand* cmd = std::launder(
			reinterpret_cast<DrawElementsIndirectCommand*>(drawCommands.data() + sizeof(GLsizei))
		);

		// prepare indirect commands buffer
		for (size_t i = 0; i != data.shapes_.size(); i++)
		{
			const uint32_t meshIdx = data.shapes_[i].meshIndex;
			const uint32_t lod = data.shapes_[i].LOD;
			*cmd++ = {
				.count_ = data.meshData_.meshes_[meshIdx].getLODIndicesCount(lod),
				.instanceCount_ = 1,
				.firstIndex_ = data.shapes_[i].indexOffset,
				.baseVertex_ = data.shapes_[i].vertexOffset,
				.baseInstance_ = data.shapes_[i].materialIndex
			};
		}

		glNamedBufferSubData(bufferIndirect_.getHandle(), 0, drawCommands.size(), drawCommands.data());

		std::vector<glm::mat4> matrices(data.shapes_.size());
		size_t i = 0;
		for (const auto& c : data.shapes_)
			matrices[i++] = data.scene_.globalTransform_[c.transformIndex];

		glNamedBufferSubData(bufferModelMatrices_.getHandle(), 0, matrices.size() * sizeof(mat4), matrices.data());
	}

	void draw(const GLSceneData& data) const
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_Materials, bufferMaterials_.getHandle());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_ModelMatrices, bufferModelMatrices_.getHandle());
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferIndirect_.getHandle());
		glBindBuffer(GL_PARAMETER_BUFFER, bufferIndirect_.getHandle());
		glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(GLsizei), 0, (GLsizei)data.shapes_.size(), 0);
	}

	~GLMesh()
	{
		glDeleteVertexArrays(1, &vao_);
	}

	GLMesh(const GLMesh&) = delete;
	GLMesh(GLMesh&&) = default;

private:
	GLuint vao_;
	uint32_t numIndices_;

	GLBuffer bufferIndices_;
	GLBuffer bufferVertices_;
	GLBuffer bufferMaterials_;

	GLBuffer bufferIndirect_;

	GLBuffer bufferModelMatrices_;
};

int main(void)
{
	GLApp app;

	GLShader shdGridVertex("data/shaders/chapter05/GL01_grid.vert");
	GLShader shdGridFragment("data/shaders/chapter05/GL01_grid.frag");
	GLProgram progGrid(shdGridVertex, shdGridFragment);

	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

	GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, kBufferIndex_PerFrameUniforms, perFrameDataBuffer.getHandle(), 0, kUniformBufferSize);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	GLShader shaderVertex  ("data/shaders/chapter07/GL01_mesh.vert");
	GLShader shaderFragment("data/shaders/chapter07/GL01_mesh.frag");
	GLProgram program(shaderVertex, shaderFragment);

	GLSceneData sceneData1("data/meshes/test.meshes", "data/meshes/test.scene", "data/meshes/test.materials");
	GLSceneData sceneData2("data/meshes/test2.meshes", "data/meshes/test2.scene", "data/meshes/test2.materials");

	GLMesh mesh1(sceneData1);
	GLMesh mesh2(sceneData2);

	glfwSetCursorPosCallback(
		app.getWindow(),
		[](auto* window, double x, double y)
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			mouseState.pos.x = static_cast<float>(x / width);
			mouseState.pos.y = static_cast<float>(y / height);
		}
	);

	glfwSetMouseButtonCallback(
		app.getWindow(),
		[](auto* window, int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				mouseState.pressedLeft = action == GLFW_PRESS;
		}
	);

	glfwSetKeyCallback(
		app.getWindow(),
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			const bool pressed = action != GLFW_RELEASE;
			if (key == GLFW_KEY_ESCAPE && pressed)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			if (key == GLFW_KEY_W)
				positioner.movement_.forward_ = pressed;
			if (key == GLFW_KEY_S)
				positioner.movement_.backward_ = pressed;
			if (key == GLFW_KEY_A)
				positioner.movement_.left_ = pressed;
			if (key == GLFW_KEY_D)
				positioner.movement_.right_ = pressed;
			if (key == GLFW_KEY_1)
				positioner.movement_.up_ = pressed;
			if (key == GLFW_KEY_2)
				positioner.movement_.down_ = pressed;
			if (mods & GLFW_MOD_SHIFT)
				positioner.movement_.fastSpeed_ = pressed;
			if (key == GLFW_KEY_SPACE)
				positioner.setUpVector(vec3(0.0f, 1.0f, 0.0f));
		}
	);

	positioner.maxSpeed_ = 1.0f;

	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;

	while (!glfwWindowShouldClose(app.getWindow()))
	{
		positioner.update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;

		int width, height;
		glfwGetFramebufferSize(app.getWindow(), &width, &height);
		const float ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
		const mat4 view = camera.getViewMatrix();

		const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = glm::vec4(camera.getPosition(), 1.0f) };
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

		glDisable(GL_BLEND);
		program.useProgram();
		mesh1.draw(sceneData1);
		mesh2.draw(sceneData2);

		glEnable(GL_BLEND);
		progGrid.useProgram();
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);

		app.swapBuffers();
	}

	return 0;
}
