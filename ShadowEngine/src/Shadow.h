#ifndef SHADOW_H
#define SHADOW_H

#include "Ignis/Ignis.h"
#include "Ignis/Framebuffer.h"
#include "Camera/Camera.h"

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

	/* opengl state backup */
	GLint backup_viewport[4];
	IgnisColorRGBA backup_clear_color;

	Camera light_camera;
} ShadowEngine;

void ShadowEngineInit(ShadowEngine* shadow);
void ShadowEngineDestroy(ShadowEngine* shadow);

void ShadowEngineStartLight(ShadowEngine* shadow, Light* light);
void ShadowEngineProcessLight(ShadowEngine* shadow, Light* light, const float* view_proj);
void ShadowEngineRenderLight(ShadowEngine* shadow, Light* light, const float* view_proj);

#endif /* !SHADOW_H */
