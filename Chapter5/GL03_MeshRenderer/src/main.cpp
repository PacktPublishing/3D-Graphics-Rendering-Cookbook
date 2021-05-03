#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "shared/glFramework/GLFWApp.h"
#include "shared/glFramework/GLShader.h"
#include "shared/UtilsMath.h"
#include "shared/Camera.h"

#include "shared/scene/VtxData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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

CameraPositioner_FirstPerson positioner( vec3(-31.5f, 7.5f, -9.5f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
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
	GLMesh(const MeshFileHeader& header, const Mesh* meshes, const uint32_t* indices, const float* vertexData)
		: numIndices_(header.indexDataSize / sizeof(uint32_t))
		, bufferIndices_(header.indexDataSize, indices, 0)
		, bufferVertices_(header.vertexDataSize, vertexData, 0)
		, bufferIndirect_(sizeof(DrawElementsIndirectCommand)* header.meshCount + sizeof(GLsizei), nullptr, GL_DYNAMIC_STORAGE_BIT)
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

		const GLsizei numCommands = (GLsizei)header.meshCount;

		drawCommands.resize(sizeof(DrawElementsIndirectCommand) * numCommands + sizeof(GLsizei));

		// store the number of draw commands in the very beginning of the buffer
		memcpy(drawCommands.data(), &numCommands, sizeof(numCommands));

		DrawElementsIndirectCommand* cmd = std::launder(
			reinterpret_cast<DrawElementsIndirectCommand*>(drawCommands.data() + sizeof(GLsizei))
		);

		// prepare indirect commands buffer
		for (uint32_t i = 0; i != numCommands; i++)
		{
			*cmd++ = {
				.count_ = meshes[i].getLODIndicesCount(0),
				.instanceCount_ = 1,
				.firstIndex_ = meshes[i].indexOffset,
				.baseVertex_ = meshes[i].vertexOffset,
				.baseInstance_ = 0
			};
		}

		glNamedBufferSubData(bufferIndirect_.getHandle(), 0, drawCommands.size(), drawCommands.data());
		glBindVertexArray(vao_);
	}

	void draw(const MeshFileHeader& header) const
	{
		glBindVertexArray(vao_);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferIndirect_.getHandle());
		glBindBuffer(GL_PARAMETER_BUFFER, bufferIndirect_.getHandle());
		glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(GLsizei), 0, (GLsizei)header.meshCount, 0);
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
	GLBuffer bufferIndirect_;
};

int main(void)
{
	GLApp app;

	GLShader shdGridVertex("data/shaders/chapter05/GL01_grid.vert");
	GLShader shdGridFragment("data/shaders/chapter05/GL01_grid.frag");
	GLProgram progGrid(shdGridVertex, shdGridFragment);

	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

	GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer.getHandle(), 0, kUniformBufferSize);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	GLShader shaderVertex  ("data/shaders/chapter05/GL03_mesh_inst.vert");
	GLShader shaderGeometry("data/shaders/chapter05/GL03_mesh_inst.geom");
	GLShader shaderFragment("data/shaders/chapter05/GL03_mesh_inst.frag");
	GLProgram program(shaderVertex, shaderGeometry, shaderFragment);

	MeshData meshData;
	const MeshFileHeader header = loadMeshData("data/meshes/test.meshes", meshData);

	GLMesh mesh(header, meshData.meshes_.data(), meshData.indexData_.data(), meshData.vertexData_.data());

	// model matrices
	const mat4 m(glm::scale(mat4(1.0f), vec3(2.0f)));
	GLBuffer modelMatrices(sizeof(mat4), value_ptr(m), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices.getHandle());

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

		const mat4 p = glm::perspective(45.0f, ratio, 0.5f, 5000.0f);
		const mat4 view = camera.getViewMatrix();

		const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = glm::vec4(camera.getPosition(), 1.0f) };
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		progGrid.useProgram();
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		program.useProgram();
		mesh.draw(header);

		app.swapBuffers();
	}

	return 0;
}
