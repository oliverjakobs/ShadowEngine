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

	IgnisFrameBuffer occlusion_map;
	IgnisFrameBuffer shadow_map;
} Light;

void ShadowEngineCreateLight(Light* light, float x, float y, float radius);
void ShadowEngineDeleteLight(Light* light);

typedef struct
{
	IgnisShader occlusion_shader;
	IgnisShader shadow_map_shader;
	IgnisShader shadow_shader;

	Camera light_camera;
} ShadowEngine;

void ShadowEngineInit(ShadowEngine* shadow);
void ShadowEngineDestroy(ShadowEngine* shadow);

void ShadowEngineStart(ShadowEngine* shadow, Light* light);
void ShadowEngineProcess(ShadowEngine* shadow, Light* light, const float* view_proj);
void ShadowEngineRender(ShadowEngine* shadow, Light* light, const float* view_proj);

#endif /* !SHADOW_H */
