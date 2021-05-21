#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "shared/Bitmap.h"
#include "shared/glFramework/GLFWApp.h"
#include "shared/glFramework/GLShader.h"
#include "shared/glFramework/GLTexture.h"
#include "shared/UtilsMath.h"
#include "shared/Camera.h"
#include "shared/scene/VtxData.h"
#include "shared/UtilsCubemap.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

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

CameraPositioner_FirstPerson positioner( vec3(0.0f, 6.0f, 11.0f), vec3(0.0f, 4.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
Camera camera(positioner);

class GLMeshPVP final
{
public:
	GLMeshPVP(const uint32_t* indices, uint32_t indicesSize, const float* vertexData, uint32_t verticesSize)
	: numIndices_(indicesSize / sizeof(uint32_t))
	, bufferIndices_(indicesSize, indices, 0)
	, bufferVertices_(verticesSize, vertexData, 0)
	{
		glCreateVertexArrays(1, &vao_);
		glVertexArrayElementBuffer(vao_, bufferIndices_.getHandle());
	}

	void draw() const
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferVertices_.getHandle());
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices_), GL_UNSIGNED_INT, nullptr);
	}

	~GLMeshPVP()
	{
		glDeleteVertexArrays(1, &vao_);
	}
private:
	GLuint vao_;
	uint32_t numIndices_;

	GLBuffer bufferIndices_;
	GLBuffer bufferVertices_;
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

	GLShader shaderVertex("data/shaders/chapter06/GL01_PBR.vert");
	GLShader shaderFragment("data/shaders/chapter06/GL01_PBR.frag");
	GLProgram program(shaderVertex, shaderFragment);

	const aiScene* scene = aiImportFile("deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf", aiProcess_Triangulate);

	if (!scene || !scene->HasMeshes())
	{
		printf("Unable to load deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf\n");
		exit(255);
	}

	struct VertexData
	{
		vec3 pos;
		vec3 n;
		vec2 tc;
	};

	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	{
		const aiMesh* mesh = scene->mMeshes[0];
		for (unsigned i = 0; i != mesh->mNumVertices; i++)
		{
			const aiVector3D v = mesh->mVertices[i];
			const aiVector3D n = mesh->mNormals[i];
			const aiVector3D t = mesh->mTextureCoords[0][i];
			vertices.push_back({ .pos = vec3(v.x, v.y, v.z), .n = vec3(n.x, n.y, n.z), .tc = vec2(t.x, 1.0f - t.y) });
		}
		for (unsigned i = 0; i != mesh->mNumFaces; i++)
		{
			for (unsigned j = 0; j != 3; j++)
				indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
		aiReleaseImport(scene);
	}

	const size_t kSizeIndices = sizeof(uint32_t) * indices.size();
	const size_t kSizeVertices = sizeof(VertexData) * vertices.size();

	GLMeshPVP mesh(indices.data(), (uint32_t)kSizeIndices, (float*)vertices.data(), (uint32_t)kSizeVertices);

	GLTexture texAO(GL_TEXTURE_2D, "deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_AO.jpg");
	GLTexture texEmissive(GL_TEXTURE_2D, "deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_emissive.jpg");
	GLTexture texAlbedo(GL_TEXTURE_2D, "deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_albedo.jpg");
	GLTexture texMeR(GL_TEXTURE_2D, "deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_metalRoughness.jpg");
	GLTexture texNormal(GL_TEXTURE_2D, "deps/src/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_normal.jpg");

	const GLuint textures[] = { texAO.getHandle(), texEmissive.getHandle(), texAlbedo.getHandle(), texMeR.getHandle(), texNormal.getHandle() };

	glBindTextures(0, sizeof(textures)/sizeof(GLuint), textures);

	// cube map
	GLTexture envMap(GL_TEXTURE_CUBE_MAP, "data/piazza_bologni_1k.hdr");
	GLTexture envMapIrradiance(GL_TEXTURE_CUBE_MAP, "data/piazza_bologni_1k_irradiance.hdr");
	const GLuint envMaps[] = { envMap.getHandle(), envMapIrradiance.getHandle() };
	glBindTextures(5, 2, envMaps);

	// BRDF LUT
	GLTexture brdfLUT(GL_TEXTURE_2D, "data/brdfLUT.ktx");
	glBindTextureUnit(7, brdfLUT.getHandle());

	// model matrices
	const mat4 m(1.0f);
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

	positioner.maxSpeed_ = 5.0f;

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
			else
				positioner.movement_.fastSpeed_ = false;
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

		const mat4 scale = glm::scale(mat4(1.0f), vec3(5.0f));
		const mat4 rot = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		const mat4 pos = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f));
		const mat4 m = glm::rotate(scale * rot * pos, (float)glfwGetTime() * 0.1f, vec3(0.0f, 0.0f, 1.0f));
		glNamedBufferSubData(modelMatrices.getHandle(), 0, sizeof(mat4), value_ptr(m));

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		program.useProgram();
		mesh.draw();

		glEnable(GL_BLEND);
		progGrid.useProgram();
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);

		app.swapBuffers();
	}

	return 0;
}
