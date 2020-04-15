#include "Font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "Ignis.h"

int ignisLoadFont(IgnisFont* font, const char* path, float size)
{
	return ignisLoadFontConfig(font, path, size, IGNIS_FONT_FIRST_CHAR, IGNIS_FONT_NUM_CHARS, IGNIS_FONT_BITMAP_WIDTH, IGNIS_FONT_BITMAP_HEIGHT);
}

int ignisLoadFontConfig(IgnisFont* font, const char* path, float size, int first, int num, int bm_w, int bm_h)
{
	if (!font) return 0;

	font->first_char = first;
	font->num_chars = num;

	font->bitmap_width = bm_w;
	font->bitmap_height = bm_h;

	/* load char data */
	font->char_data = (stbtt_bakedchar*)malloc(sizeof(stbtt_bakedchar) * font->num_chars);
	if (!font->char_data)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to allocate memory for char data");
	}

	GLubyte* bitmap = (GLubyte*)malloc(sizeof(GLubyte) * font->bitmap_width * font->bitmap_height);
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

	stbtt_BakeFontBitmap(buffer, 0, size, bitmap, font->bitmap_width, font->bitmap_height, font->first_char, font->num_chars, font->char_data); // no guarantee this fits!

	free(buffer);

	IgnisTextureConfig config = IGNIS_DEFAULT_CONFIG;
	config.internal_format = GL_R8;
	config.format = GL_RED;

	if (!ignisCreateTextureRaw(&font->texture, GL_TEXTURE_2D, font->bitmap_width, font->bitmap_height, bitmap, &config))
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

	ignisDestroyTexture(&font->texture);
}

void ignisBindFont(IgnisFont* font)
{
	ignisBindTexture(&font->texture, 0);
}

void ignisUnbindFont(IgnisFont* font)
{
	ignisUnbindTexture(&font->texture);
}

int ignisLoadCharQuad(IgnisFont* font, char c, float* x, float* y, float* vertices, size_t offset)
{
	if (c >= font->first_char && c < font->first_char + font->num_chars)
	{
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(font->char_data, font->bitmap_width, font->bitmap_height, c - font->first_char, x, y, &q, 1);

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
			stbtt_GetBakedQuad(font->char_data, font->bitmap_width, font->bitmap_height, text[i] - font->first_char, &x, &y, &q, 1);
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
			stbtt_GetBakedQuad(font->char_data, font->bitmap_width, font->bitmap_height, text[i] - font->first_char, &x, &y, &q, 1);

			if (height < y - q.y0) height = y - q.y0;
			if (y_offset && *y_offset > y - q.y1) *y_offset = y - q.y1;
		}
	}
	return height;
}
