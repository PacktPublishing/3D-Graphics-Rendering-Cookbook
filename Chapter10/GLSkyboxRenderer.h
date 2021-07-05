#pragma once

#include "shared/glFramework/GLFWApp.h"

class GLSkyboxRenderer
{
public:
	GLSkyboxRenderer(
		const char* envMap = "data/immenstadter_horn_2k.hdr",
		const char* envMapIrradiance = "data/immenstadter_horn_2k_irradiance.hdr")
	: envMap_(GL_TEXTURE_CUBE_MAP, envMap)
	, envMapIrradiance_(GL_TEXTURE_CUBE_MAP, envMapIrradiance)
	{
		glCreateVertexArrays(1, &dummyVAO_);
		const GLuint pbrTextures[] = { envMap_.getHandle(), envMapIrradiance_.getHandle(), brdfLUT_.getHandle() };
		// binding points for data/shaders/PBR.sp
		glBindTextures(5, 3, pbrTextures);
	}
	~GLSkyboxRenderer()
	{
		glDeleteVertexArrays(1, &dummyVAO_);
	}
	void draw()
	{
		progCube_.useProgram();
		glBindTextureUnit(1, envMap_.getHandle());
		glDepthMask(false);
		glBindVertexArray(dummyVAO_);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);
	}

private:
	// https://hdrihaven.com/hdri/?h=immenstadter_horn
	GLTexture envMap_;
	GLTexture envMapIrradiance_;
	GLTexture brdfLUT_ = { GL_TEXTURE_2D, "data/brdfLUT.ktx" };
	GLShader shdCubeVertex_ = GLShader{ "data/shaders/chapter08/GL03_cube.vert" };
	GLShader shdCubeFragment_ = GLShader{ "data/shaders/chapter08/GL03_cube.frag" };
	GLProgram progCube_ = GLProgram{ shdCubeVertex_, shdCubeFragment_ };
	GLuint dummyVAO_;
};
