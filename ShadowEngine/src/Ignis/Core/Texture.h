#ifndef IGNIS_TEXTURE_H
#define IGNIS_TEXTURE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../glad/glad.h"

typedef struct
{
	GLint internal_format;
	GLenum format;

	GLint min_filter;
	GLint mag_filter;

	GLint wrap_s;
	GLint wrap_t;
} IgnisTextureConfig;

#define IGNIS_DEFAULT_CONFIG { GL_RGBA8, GL_RGBA, GL_LINEAR, GL_NEAREST, GL_REPEAT, GL_REPEAT }

GLuint ignisGenerateTexture(GLuint target, int width, int height, void* pixels, IgnisTextureConfig config);

typedef struct
{
	GLuint name;

	int width;
	int height;

	GLuint rows;
	GLuint columns;

	IgnisTextureConfig config;
} IgnisTexture2D;

int ignisCreateTexture2DEmpty(IgnisTexture2D* texture, int width, int height, IgnisTextureConfig* config);
int ignisCreateTexture2DRaw(IgnisTexture2D* texture, int width, int height, void* pixels, IgnisTextureConfig* config);
int ignisCreateTexture2D(IgnisTexture2D* texture, const char* path, GLuint rows, GLuint columns, int flip_on_load, IgnisTextureConfig* config);

void ignisDeleteTexture2D(IgnisTexture2D* texture);

void ignisBindTexture2D(IgnisTexture2D* texture, GLuint slot);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_TEXTURE_H */