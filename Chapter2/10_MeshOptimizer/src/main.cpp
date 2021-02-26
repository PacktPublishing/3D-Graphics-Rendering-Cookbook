#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>
#include <meshoptimizer.h>

#include <stdio.h>
#include <stdlib.h>

#include <vector>

using glm::mat4;
using glm::vec3;

static const char* shaderCodeVertex = R"(
#version 460 core
layout(std140, binding = 0) uniform PerFrameData
{
	uniform mat4 MVP;
};
layout (location=0) in vec3 pos;
layout (location=0) out vec3 color;
void main()
{
	gl_Position = MVP * vec4(pos, 1.0);
	color = pos.xyz;
}
)";

static const char* shaderCodeGeometry = R"(
#version 460 core

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

layout (location=0) in vec3 color[];
layout (location=0) out vec3 colors;
layout (location=1) out vec3 barycoords;

void main()
{
	const vec3 bc[3] = vec3[]
	(
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);
	for ( int i = 0; i < 3; i++ )
	{
		gl_Position = gl_in[i].gl_Position;
		colors = color[i];
		barycoords = bc[i];
		EmitVertex();
	}
	EndPrimitive();
}
)";

static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 colors;
layout (location=1) in vec3 barycoords;
layout (location=0) out vec4 out_FragColor;
float edgeFactor(float thickness)
{
	vec3 a3 = smoothstep( vec3( 0.0 ), fwidth(barycoords) * thickness, barycoords);
	return min( min( a3.x, a3.y ), a3.z );
}
void main()
{
	out_FragColor = vec4( mix( vec3(0.0), colors, edgeFactor(1.0) ), 1.0 );
};
)";

struct PerFrameData
{
	mat4 mvp;
};

int main(void)
{
	glfwSetErrorCallback(
		[](int error, const char* description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}
	);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Simple example", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderVertex, 1, &shaderCodeVertex, nullptr);
	glCompileShader(shaderVertex);

	const GLuint shaderGeometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(shaderGeometry, 1, &shaderCodeGeometry, nullptr);
	glCompileShader(shaderGeometry);

	const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderFragment, 1, &shaderCodeFragment, nullptr);
	glCompileShader(shaderFragment);

	const GLuint program = glCreateProgram();
	glAttachShader(program, shaderVertex);
	glAttachShader(program, shaderGeometry);
	glAttachShader(program, shaderFragment);
	glLinkProgram(program);
	glUseProgram(program);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLsizeiptr kBufferSize = sizeof(PerFrameData);

	GLuint perFrameDataBuffer;
	glCreateBuffers(1, &perFrameDataBuffer);
	glNamedBufferStorage(perFrameDataBuffer, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer, 0, kBufferSize);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	GLuint meshData;
	glCreateBuffers(1, &meshData);

	const aiScene* scene = aiImportFile("data/rubber_duck/scene.gltf", aiProcess_Triangulate);

	if (!scene || !scene->HasMeshes())
	{
		printf("Unable to load data/rubber_duck/scene.gltf\n");
		exit(255);
	}

	const aiMesh* mesh = scene->mMeshes[0];
	std::vector<vec3> positions;
	for (unsigned i = 0; i != mesh->mNumVertices; i++)
	{
		const aiVector3D v = mesh->mVertices[i];
		positions.push_back(vec3(v.x, v.z, v.y));
	}
	std::vector<unsigned int> indices;
	for (unsigned i = 0; i != mesh->mNumFaces; i++)
	{
		for (unsigned j = 0; j != 3; j++)
			indices.push_back(mesh->mFaces[i].mIndices[j]);
	}
	aiReleaseImport(scene);

	std::vector<unsigned int> indicesLod;
	{
		std::vector<unsigned int> remap(indices.size());
		const size_t vertexCount = meshopt_generateVertexRemap(remap.data(), indices.data(), indices.size(), positions.data(), indices.size(), sizeof(vec3));

		std::vector<unsigned int> remappedIndices(indices.size());
		std::vector<vec3> remappedVertices(vertexCount);

		meshopt_remapIndexBuffer(remappedIndices.data(), indices.data(), indices.size(), remap.data());
		meshopt_remapVertexBuffer(remappedVertices.data(), positions.data(), positions.size(), sizeof(vec3), remap.data());

		meshopt_optimizeVertexCache(remappedIndices.data(), remappedIndices.data(), indices.size(), vertexCount);
		meshopt_optimizeOverdraw(remappedIndices.data(), remappedIndices.data(), indices.size(), glm::value_ptr(remappedVertices[0]), vertexCount, sizeof(vec3), 1.05f);
		meshopt_optimizeVertexFetch(remappedVertices.data(), remappedIndices.data(), indices.size(), remappedVertices.data(), vertexCount, sizeof(vec3));

		const float threshold = 0.2f;
		const size_t target_index_count = size_t(remappedIndices.size() * threshold);
		const float target_error = 1e-2f;

		indicesLod.resize(remappedIndices.size());
		indicesLod.resize(meshopt_simplify(&indicesLod[0], remappedIndices.data(), remappedIndices.size(), &remappedVertices[0].x, vertexCount, sizeof(vec3), target_index_count, target_error));

		indices = remappedIndices;
		positions = remappedVertices;
	}

	const size_t sizeIndices = sizeof(unsigned int) * indices.size();
	const size_t sizeIndicesLod = sizeof(unsigned int) * indicesLod.size();
	const size_t sizeVertices = sizeof(vec3) * positions.size();

	glNamedBufferStorage(meshData, sizeIndices + sizeIndicesLod + sizeVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(meshData, 0, sizeIndices, indices.data());
	glNamedBufferSubData(meshData, sizeIndices, sizeIndicesLod, indicesLod.data());
	glNamedBufferSubData(meshData, sizeIndices + sizeIndicesLod, sizeVertices, positions.data());

	glVertexArrayElementBuffer(vao, meshData);
	glVertexArrayVertexBuffer(vao, 0, meshData, sizeIndices + sizeIndicesLod, sizeof(vec3));
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);

	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		const float ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const mat4 m1 = glm::rotate(glm::translate(mat4(1.0f), vec3(-0.5f, -0.5f, -1.5f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 m2 = glm::rotate(glm::translate(mat4(1.0f), vec3(+0.5f, -0.5f, -1.5f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);

		const PerFrameData perFrameData1 = { .mvp = p * m1 };
		glNamedBufferSubData(perFrameDataBuffer, 0, kBufferSize, &perFrameData1);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned>(indices.size()), GL_UNSIGNED_INT, nullptr);

		const PerFrameData perFrameData2 = { .mvp = p * m2 };
		glNamedBufferSubData(perFrameDataBuffer, 0, kBufferSize, &perFrameData2);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned>(indicesLod.size()), GL_UNSIGNED_INT, (void*)sizeIndices);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &meshData);
	glDeleteBuffers(1, &perFrameDataBuffer);
	glDeleteProgram(program);
	glDeleteShader(shaderFragment);
	glDeleteShader(shaderVertex);
	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
