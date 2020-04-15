#ifndef RENDERER2D_H
#define RENDERER2D_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Ignis/Ignis.h"

void Renderer2DInit(const char* vert, const char* frag);
void Renderer2DDestroy();

void Renderer2DSetShader(IgnisShader* shader);

void Renderer2DRenderTexture(IgnisTexture* texture, float x, float y, float w, float h, const float* view_proj);
void Renderer2DRenderTextureC(IgnisTexture* texture, float x, float y, float w, float h, const float* view_proj, IgnisColorRGBA color);
void Renderer2DRenderTextureM(IgnisTexture* texture, const float* model, const float* view_proj, const float* color);

#ifdef __cplusplus
}
#endif

#endif /* !RENDERER2D_H */