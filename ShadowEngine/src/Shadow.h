#ifndef SHADOW_H
#define SHADOW_H

#include "Ignis/Ignis.h"
#include "Ignis/Framebuffer.h"
#include "Ignis/Quad.h"

typedef struct
{
	float x;
	float y;
	float radius;

	IgnisColorRGBA color;

	IgnisFrameBuffer occlusion_map;
	IgnisFrameBuffer shadow_map;
} Light;

void ShadowEngineCreateLight(Light* light, float x, float y, float radius, IgnisColorRGBA color);
void ShadowEngineDeleteLight(Light* light);

typedef struct
{
	IgnisShader occlusion_shader;
	IgnisShader shadow_map_shader;
	IgnisShader shadow_shader;

	IgnisFrameBuffer occlusion_map;

	IgnisQuad quad;

	/* opengl state backup */
	GLint backup_viewport[4];
	IgnisColorRGBA backup_clear_color;
} ShadowEngine;

void ShadowEngineInit(ShadowEngine* shadow, int width, int height);
void ShadowEngineDestroy(ShadowEngine* shadow);

void ShadowEngineStart(ShadowEngine* shadow);

void ShadowEngineProcessLight(ShadowEngine* shadow, Light* light, const float* view_proj);

void ShadowEngineRenderStart(ShadowEngine* shadow);
void ShadowEngineRenderFlush(ShadowEngine* shadow);

void ShadowEngineRenderLight(ShadowEngine* shadow, Light* light, const float* view_proj);

#endif /* !SHADOW_H */
