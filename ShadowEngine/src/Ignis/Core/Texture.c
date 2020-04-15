#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../Ignis.h"

int ignisCreateTextureEmpty(IgnisTexture* texture, GLuint target, int width, int height, IgnisTextureConfig* config)
{
	return ignisCreateTextureRaw(texture, target, width, height, NULL, config);
}

int ignisCreateTextureRaw(IgnisTexture* texture, GLuint target, int width, int height, void* pixels, IgnisTextureConfig* configptr)
{
	IgnisTextureConfig config = IGNIS_DEFAULT_CONFIG;

	if (configptr)
		config = *configptr;

	if (texture)
	{
		texture->name = ignisGenerateTexture(target, width, height, pixels, config);
		texture->target = target;
		texture->width = width;
		texture->height = height;
		texture->rows = 1;
		texture->columns = 1;
		texture->slot = 0;
		texture->config = config;

		return texture->name;
	}

	return 0;
}

int ignisCreateTexture(IgnisTexture* texture, GLuint target, const char* path, GLuint rows, GLuint columns, int flip_on_load, IgnisTextureConfig* configptr)
{
	IgnisTextureConfig config = IGNIS_DEFAULT_CONFIG;

	if (configptr)
		config = *configptr;

	if (texture)
	{
		stbi_set_flip_vertically_on_load(flip_on_load);

		int bpp = 0;

		unsigned char* pixels = stbi_load(path, &texture->width, &texture->height, &bpp, 4);

		/* check if bpp and format matches */
		if (bpp == 4)
		{
			if (config.format != GL_RGBA || config.internal_format != GL_RGBA8)
				_ignisErrorCallback(IGNIS_WARN, "[Texture] Format mismatch for %s", path);
		}
		else if (bpp == 3)
		{
			if (config.format != GL_RGB || config.internal_format != GL_RGB8)
				_ignisErrorCallback(IGNIS_WARN, "[Texture] Format mismatch for %s", path);
		}

		if (pixels)
		{
			texture->name = ignisGenerateTexture(target, texture->width, texture->height, pixels, config);
			stbi_image_free(pixels);
		}
		else
		{
			_ignisErrorCallback(IGNIS_ERROR, "[Texture] Failed to load texture(%s): %s", path, stbi_failure_reason());
			texture->name = 0;
		}

		texture->target = target;
		texture->rows = rows;
		texture->columns = columns;
		texture->slot = 0;
		texture->config = config;

		return texture->name;
	}

	return 0;
}

void ignisDestroyTexture(IgnisTexture* texture)
{
	glDeleteTextures(1, &texture->name);
}

GLuint ignisGenerateTexture(GLuint target, int width, int height, void* pixels, IgnisTextureConfig config)
{
	GLuint name;
	glGenTextures(1, &name);
	glBindTexture(target, name);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, config.wrap_s);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, config.wrap_t);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, config.min_filter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, config.mag_filter);

	switch (target)
	{
	case GL_TEXTURE_1D:
	case GL_PROXY_TEXTURE_1D:
		glTexImage1D(target, 0, config.internal_format, width, 0, config.format, GL_UNSIGNED_BYTE, pixels);
		break;
	case GL_TEXTURE_2D:
	case GL_PROXY_TEXTURE_2D:
	case GL_TEXTURE_1D_ARRAY:
	case GL_PROXY_TEXTURE_1D_ARRAY:
	case GL_TEXTURE_RECTANGLE:
	case GL_PROXY_TEXTURE_RECTANGLE:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
	case GL_PROXY_TEXTURE_CUBE_MAP:
		glTexImage2D(target, 0, config.internal_format, width, height, 0, config.format, GL_UNSIGNED_BYTE, pixels);
		break;
	default:
		_ignisErrorCallback(IGNIS_ERROR, "[Texture] Unsupported target (%d)", target);
		glDeleteTextures(1, &name);
		return 0;
	}
	glGenerateMipmap(target);

	return name;
}

void ignisBindTexture(IgnisTexture* texture, GLuint slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(texture->target, texture->name);
	texture->slot = slot;
}

void ignisUnbindTexture(IgnisTexture* texture)
{
	glActiveTexture(GL_TEXTURE0 + texture->slot);
	glBindTexture(texture->target, 0);
	texture->slot = 0;
}
