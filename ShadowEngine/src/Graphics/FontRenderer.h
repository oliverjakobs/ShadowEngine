#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Ignis/Ignis.h"

void FontRendererInit(const char* vert, const char* frag);
void FontRendererDestroy();

void FontRendererBindFont(IgnisFont* font, IgnisColorRGBA color);

void FontRendererStart(const float* mat_proj);
void FontRendererFlush();

void FontRendererRenderText(float x, float y, const char* text);
void FontRendererRenderTextFormat(float x, float y, const char* text, ...);

#ifdef __cplusplus
}
#endif

#endif /* !FONT_RENDERER_H */