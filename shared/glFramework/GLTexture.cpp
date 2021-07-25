#include "shared/glFramework/GLTexture.h"
#include "shared/Bitmap.h"
#include "shared/UtilsCubemap.h"

#include <glad/gl.h>
#include <assert.h>
#include <stdio.h>
#include <string>

#include <stb/stb_image.h>
#include <gli/gli.hpp>
#include <gli/texture2d.hpp>
#include <gli/load_ktx.hpp>

int getNumMipMapLevels2D(int w, int h)
{
	int levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}

GLTexture::GLTexture(GLenum type, int width, int height, GLenum internalFormat)
	: type_(type)
{
	glCreateTextures(type, 1, &handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(handle_, getNumMipMapLevels2D(width, height), internalFormat, width, height);
}

/// Draw a checkerboard on a pre-allocated square RGB image.
uint8_t* genDefaultCheckerboardImage(int* width, int* height)
{
	const int w = 128;
	const int h = 128;

	uint8_t* imgData = (uint8_t*)malloc(w * h * 3); // stbi_load() uses malloc(), so this is safe

	assert(imgData && w > 0 && h > 0);
	assert(w == h);

	if (!imgData || w <= 0 || h <= 0) return nullptr;
	if (w != h) return nullptr;

	for (int i = 0; i < w * h; i++)
	{
		const int row = i / w;
		const int col = i % w;
		imgData[i * 3 + 0] = imgData[i * 3 + 1] = imgData[i * 3 + 2] = 0xFF * ((row + col) % 2);
	}

	if (width) *width = w;
	if (height) *height = h;

	return imgData;
}

GLTexture::GLTexture(GLenum type, const char* fileName)
	: type_(type)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCreateTextures(type, 1, &handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const char* ext = strrchr(fileName, '.');

	const bool isKTX = ext && !strcmp(ext, ".ktx");

	switch (type)
	{
	case GL_TEXTURE_2D:
	{
		int w = 0;
		int h = 0;
		int numMipmaps = 0;
		if (isKTX)
		{
			gli::texture gliTex = gli::load_ktx(fileName);
			gli::gl GL(gli::gl::PROFILE_KTX);
			gli::gl::format const format = GL.translate(gliTex.format(), gliTex.swizzles());
			glm::tvec3<GLsizei> extent(gliTex.extent(0));
			w = extent.x;
			h = extent.y;
			numMipmaps = getNumMipMapLevels2D(w, h);
			glTextureStorage2D(handle_, numMipmaps, format.Internal, w, h);
			glTextureSubImage2D(handle_, 0, 0, 0, w, h, format.External, format.Type, gliTex.data(0, 0, 0));
		}
		else
		{
			uint8_t* img = stbi_load(fileName, &w, &h, nullptr, STBI_rgb_alpha);

			// Note(Anton): replaced assert(img) with a fallback image to prevent crashes with missing files or bad (eg very long) paths.
			if (!img)
			{
				fprintf(stderr, "WARNING: could not load image `%s`, using a fallback.\n", fileName);
				img = genDefaultCheckerboardImage(&w, &h);
				if (!img)
				{
					fprintf(stderr, "FATAL ERROR: out of memory allocating image for fallback texture\n");
					exit(EXIT_FAILURE);
				}
			}

			numMipmaps = getNumMipMapLevels2D(w, h);
			glTextureStorage2D(handle_, numMipmaps, GL_RGBA8, w, h);
			glTextureSubImage2D(handle_, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
			stbi_image_free((void*)img);
		}
		glGenerateTextureMipmap(handle_);
		glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps-1);
		glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(handle_, GL_TEXTURE_MAX_ANISOTROPY , 16);
		break;
	}
	case GL_TEXTURE_CUBE_MAP:
	{
		int w, h, comp;
		const float* img = stbi_loadf(fileName, &w, &h, &comp, 3);
		assert(img);
		Bitmap in(w, h, comp, eBitmapFormat_Float, img);
		const bool isEquirectangular = w == 2 * h;
		Bitmap out = isEquirectangular ? convertEquirectangularMapToVerticalCross(in) : in;
		stbi_image_free((void*)img);
		Bitmap cubemap = convertVerticalCrossToCubeMapFaces(out);

		const int numMipmaps = getNumMipMapLevels2D(cubemap.w_, cubemap.h_);

		glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(handle_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(handle_, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps-1);
		glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glTextureStorage2D(handle_, numMipmaps, GL_RGB32F, cubemap.w_, cubemap.h_);
		const uint8_t* data = cubemap.data_.data();

		for (unsigned i = 0; i != 6; ++i)
		{
			glTextureSubImage3D(handle_, 0, 0, 0, i, cubemap.w_, cubemap.h_, 1, GL_RGB, GL_FLOAT, data);
			data += cubemap.w_ * cubemap.h_ * cubemap.comp_ * Bitmap::getBytesPerComponent(cubemap.fmt_);
		}

		glGenerateTextureMipmap(handle_);
		break;
	}
	default:
		assert(false);
	}

	handleBindless_ = glGetTextureHandleARB(handle_);
	glMakeTextureHandleResidentARB(handleBindless_);
}

GLTexture::GLTexture(int w, int h, const void* img)
	: type_(GL_TEXTURE_2D)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCreateTextures(type_, 1, &handle_);
	int numMipmaps = getNumMipMapLevels2D(w, h);
	glTextureStorage2D(handle_, numMipmaps, GL_RGBA8, w, h);
	glTextureSubImage2D(handle_, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glGenerateTextureMipmap(handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_ANISOTROPY, 16);
	handleBindless_ = glGetTextureHandleARB(handle_);
	glMakeTextureHandleResidentARB(handleBindless_);
}

GLTexture::GLTexture(GLTexture&& other)
: type_(other.type_)
, handle_(other.handle_)
, handleBindless_(other.handleBindless_)
{
	other.type_ = 0;
	other.handle_ = 0;
	other.handleBindless_ = 0;
}

GLTexture::~GLTexture()
{
	if (handleBindless_)
		glMakeTextureHandleNonResidentARB(handleBindless_);
	glDeleteTextures(1, &handle_);
}
