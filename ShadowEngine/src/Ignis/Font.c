#include "Font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "Ignis.h"

int ignisCreateFont(IgnisFont* font, const char* path, float size)
{
	return ignisCreateFontConfig(font, path, size, IGNIS_FONT_FIRST_CHAR, IGNIS_FONT_NUM_CHARS, IGNIS_FONT_BITMAP_WIDTH, IGNIS_FONT_BITMAP_HEIGHT);
}

int ignisCreateFontConfig(IgnisFont* font, const char* path, float size, int first, int num, int bitmap_width, int bitmap_height)
{
	if (!font) return 0;

	font->first_char = first;
	font->num_chars = num;

	/* load char data */
	font->char_data = (stbtt_bakedchar*)malloc(sizeof(stbtt_bakedchar) * font->num_chars);
	if (!font->char_data)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to allocate memory for char data");
	}

	GLubyte* bitmap = (GLubyte*)malloc(sizeof(GLubyte) * bitmap_width * bitmap_height);
	if (!bitmap)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to allocate memory for bitmap");
		ignisDeleteFont(font);
		return 0;
	}

	char* buffer = ignisReadFile(path, NULL);
	if (!buffer)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to read file: %s", path);
		ignisDeleteFont(font);
		free(bitmap);
		return 0;
	}

	stbtt_BakeFontBitmap(buffer, 0, size, bitmap, bitmap_width, bitmap_height, font->first_char, font->num_chars, font->char_data);

	free(buffer);

	IgnisTextureConfig config = IGNIS_DEFAULT_CONFIG;
	config.internal_format = GL_R8;
	config.format = GL_RED;

	if (!ignisGenerateTexture2D(&font->texture, bitmap_width, bitmap_height, bitmap, &config))
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to create texture");
		ignisDeleteFont(font);
		return 0;
	}

	free(bitmap);
	return 1;
}

void ignisDeleteFont(IgnisFont* font)
{
	if (font->char_data)
		free(font->char_data);

	ignisDeleteTexture2D(&font->texture);
}

void ignisBindFont(IgnisFont* font, GLuint slot)
{
	if (font)
		ignisBindTexture2D(&font->texture, slot);
	else
		ignisBindTexture2D(NULL, slot);
}

int ignisFontLoadCharQuad(IgnisFont* font, char c, float* x, float* y, float* vertices, size_t offset)
{
	if (c >= font->first_char && c < font->first_char + font->num_chars)
	{
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(font->char_data, font->texture.width, font->texture.height, c - font->first_char, x, y, &q, 1);

		float quad[] =
		{
			q.x0, q.y0, q.s0, q.t0,
			q.x0, q.y1, q.s0, q.t1,
			q.x1, q.y1, q.s1, q.t1,
			q.x1, q.y0, q.s1, q.t0
		};

		memcpy(vertices + offset, quad, sizeof(quad));

		return 1;
	}

	return 0;
}

float ignisFontGetTextWidth(IgnisFont* font, const char* text)
{
	float x = 0.0f;
	float y = 0.0f;
	for (size_t i = 0; i < strlen(text); i++)
	{
		if (text[i] >= font->first_char && text[i] < font->first_char + font->num_chars)
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(font->char_data, font->texture.width, font->texture.height, text[i] - font->first_char, &x, &y, &q, 1);
		}
	}
	return x;
}

float ignisFontGetTextHeight(IgnisFont* font, const char* text, float* y_offset)
{
	float x = 0.0f;
	float y = 0.0f;

	float height = 0.0f;

	for (size_t i = 0; i < strlen(text); i++)
	{
		if (text[i] >= font->first_char && text[i] < font->first_char + font->num_chars)
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(font->char_data, font->texture.width, font->texture.height, text[i] - font->first_char, &x, &y, &q, 1);

			if (height < y - q.y0) height = y - q.y0;
			if (y_offset && *y_offset > y - q.y1) *y_offset = y - q.y1;
		}
	}
	return height;
}
