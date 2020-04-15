#ifndef IGNIS_FONT_H
#define IGNIS_FONT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Core/Texture.h"

#include "stb_truetype.h"

typedef struct
{
	int first_char;
	int num_chars;

	int bitmap_width;
	int bitmap_height;

	stbtt_bakedchar* char_data;

	IgnisTexture texture;
} IgnisFont;

int ignisLoadFont(IgnisFont* font, const char* path, float size);
int ignisLoadFontConfig(IgnisFont* font, const char* path, float size, int first, int num, int bm_w, int bm_h);
void ignisDeleteFont(IgnisFont* font);

void ignisBindFont(IgnisFont* font);
void ignisUnbindFont(IgnisFont* font);

int ignisLoadCharQuad(IgnisFont* font, char c, float* x, float* y, float* vertices, size_t offset);

float ignisFontGetTextWidth(IgnisFont* font, const char* text);
float ignisFontGetTextHeight(IgnisFont* font, const char* text, float* y_offset);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_FONT_H */