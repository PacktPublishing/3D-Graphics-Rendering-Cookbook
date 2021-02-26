#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

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
	uniform int isWireframe;
};
layout (location=0) in vec3 pos;
layout (location=0) out vec3 color;
void main()
{
	gl_Position = MVP * vec4(pos, 1.0);
	color = isWireframe > 0 ? vec3(0.0f) : pos.xyz;
}
)";

static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 color;
layout (location=0) out vec4 out_FragColor;
void main()
{
	out_FragColor = vec4(color, 1.0);
};
)";

struct PerFrameData
{
	mat4 mvp;
	int isWireframe;
};

int main( void )
{
	glfwSetErrorCallback(
		[]( int error, const char* description )
		{
			fprintf( stderr, "Error: %s\n", description );
		}
	);

	if ( !glfwInit() )
		exit( EXIT_FAILURE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	GLFWwindow* window = glfwCreateWindow( 1024, 768, "Simple example", nullptr, nullptr );
	if ( !window )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback(
		window,
		[]( GLFWwindow* window, int key, int scancode, int action, int mods )
		{
			if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
				glfwSetWindowShouldClose( window, GLFW_TRUE );
		}
	);

	glfwMakeContextCurrent( window );
	gladLoadGL( glfwGetProcAddress );
	glfwSwapInterval( 1 );

	const GLuint shaderVertex = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( shaderVertex, 1, &shaderCodeVertex, nullptr );
	glCompileShader( shaderVertex );

	const GLuint shaderFragment = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( shaderFragment, 1, &shaderCodeFragment, nullptr );
	glCompileShader( shaderFragment );

	const GLuint program = glCreateProgram();
	glAttachShader( program, shaderVertex );
	glAttachShader( program, shaderFragment );
	glLinkProgram( program );

	GLuint vao;
	glCreateVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	const GLsizeiptr kBufferSize = sizeof( PerFrameData );

	GLuint perFrameDataBuffer;
	glCreateBuffers( 1, &perFrameDataBuffer );
	glNamedBufferStorage( perFrameDataBuffer, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT );
	glBindBufferRange( GL_UNIFORM_BUFFER, 0, perFrameDataBuffer, 0, kBufferSize );

	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_POLYGON_OFFSET_LINE );
	glPolygonOffset( -1.0f, -1.0f );

	GLuint meshData;
	glCreateBuffers( 1, &meshData );

	const aiScene* scene = aiImportFile( "data/rubber_duck/scene.gltf", aiProcess_Triangulate );

	if ( !scene || !scene->HasMeshes() )
	{
		printf( "Unable to load data/rubber_duck/scene.gltf\n" );
		exit( 255 );
	}

	const aiMesh* mesh = scene->mMeshes[0];
	std::vector<vec3> positions;
	for ( unsigned int i = 0; i != mesh->mNumFaces; i++ )
	{
		const aiFace& face = mesh->mFaces[i];
		const unsigned int idx[3] = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
		for ( int j = 0; j != 3; j++ )
		{
			const aiVector3D v = mesh->mVertices[idx[j]];
			positions.push_back( vec3( v.x, v.z, v.y ) );
		}
	}

	aiReleaseImport( scene );

	glNamedBufferStorage( meshData, sizeof( vec3 ) * positions.size(), positions.data(), 0 );

	glVertexArrayVertexBuffer( vao, 0, meshData, 0, sizeof( vec3 ) );
	glEnableVertexArrayAttrib( vao, 0 );
	glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
	glVertexArrayAttribBinding( vao, 0, 0 );

	const int numVertices = static_cast<int>(positions.size());

	while ( !glfwWindowShouldClose( window ) )
	{
		int width, height;
		glfwGetFramebufferSize( window, &width, &height );
		const float ratio = width / (float)height;

		glViewport( 0, 0, width, height );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		const mat4 m = glm::rotate( glm::translate( mat4( 1.0f ), vec3( 0.0f, -0.5f, -1.5f ) ), (float)glfwGetTime(), vec3( 0.0f, 1.0f, 0.0f ) );
		const mat4 p = glm::perspective( 45.0f, ratio, 0.1f, 1000.0f );

		PerFrameData perFrameData = { .mvp = p * m, .isWireframe = false };

		glUseProgram( program );
		glNamedBufferSubData( perFrameDataBuffer, 0, kBufferSize, &perFrameData );

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDrawArrays( GL_TRIANGLES, 0, numVertices );

		perFrameData.isWireframe = true;
		glNamedBufferSubData( perFrameDataBuffer, 0, kBufferSize, &perFrameData );

		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDrawArrays( GL_TRIANGLES, 0, numVertices );

		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glDeleteBuffers( 1, &meshData );
	glDeleteBuffers( 1, &perFrameDataBuffer );
	glDeleteProgram( program );
	glDeleteShader( shaderFragment );
	glDeleteShader( shaderVertex );
	glDeleteVertexArrays( 1, &vao );

	glfwDestroyWindow( window );
	glfwTerminate();

	return 0;
}
