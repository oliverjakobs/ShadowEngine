#include "Shadow.h"

#include "Graphics/Renderer2D.h"

void ShadowEngineCreateLight(Light* light, float x, float y, float radius, IgnisColorRGBA color)
{
	light->x = x;
	light->y = y;
	light->radius = radius;
	light->color = color;

	/* framebuffers */
	ignisGenerateFrameBuffer(&light->occlusion_map, radius, radius);
	ignisGenerateFrameBuffer(&light->shadow_map, radius, 1);
}

void ShadowEngineDeleteLight(Light* light)
{
	ignisDeleteFrameBuffer(&light->occlusion_map);
	ignisDeleteFrameBuffer(&light->shadow_map);
}

void ShadowEngineInit(ShadowEngine* shadow)
{
	/* shaders */
	ignisCreateShadervf(&shadow->occlusion_shader, "res/shaders/pass.vert", "res/shaders/occlusion.frag");
	ignisCreateShadervf(&shadow->shadow_map_shader, "res/shaders/pass.vert", "res/shaders/shadow_map.frag");
	ignisCreateShadervf(&shadow->shadow_shader, "res/shaders/pass.vert", "res/shaders/shadow.frag");
}

void ShadowEngineDestroy(ShadowEngine* shadow)
{
	ignisDeleteShader(&shadow->occlusion_shader);
	ignisDeleteShader(&shadow->shadow_map_shader);
	ignisDeleteShader(&shadow->shadow_shader);
}

void ShadowEngineStart(ShadowEngine* shadow, Light* light)
{
	/* save previous gl state */
	glGetIntegerv(GL_VIEWPORT, shadow->backup_viewport);
	glGetFloatv(GL_COLOR_CLEAR_VALUE, &shadow->backup_clear_color.r);

	/* create occlusion map */
	ignisBindFrameBuffer(&light->occlusion_map);
	shadow->light_camera.position = (vec3){ light->x, light->y, 0.0f };

	CameraSetProjectionOrtho(&shadow->light_camera, (float)light->occlusion_map.width, (float)light->occlusion_map.height);
	glViewport(0, 0, light->occlusion_map.width, light->occlusion_map.height);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void ShadowEngineProcess(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	glViewport(shadow->backup_viewport[0], shadow->backup_viewport[1], shadow->backup_viewport[2], shadow->backup_viewport[3]);

	/* create shadow map*/
	ignisBindFrameBuffer(&light->shadow_map);
	glClear(GL_COLOR_BUFFER_BIT);

	ignisSetUniform1f(&shadow->shadow_map_shader, "u_Resolution", light->radius);

	float w = (float)light->shadow_map.width;
	float h = (float)light->shadow_map.height;

	Renderer2DSetShader(&shadow->shadow_map_shader);
	Renderer2DRenderTexture(&light->occlusion_map.texture, 0.0f, h, w, -h, view_proj);

	/* RESET */
	ignisBindFrameBuffer(NULL);
	ignisClearColor(shadow->backup_clear_color);
}

void ShadowEngineRender(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	ignisSetUniform1f(&shadow->shadow_shader, "u_SoftShadows", 1.0f);
	ignisSetUniform1f(&shadow->shadow_shader, "u_Resolution", light->radius);

	//draw centered on light position
	float x = light->x - light->radius / 2.0f;
	float y = light->y - light->radius / 2.0f;
	float w = light->radius;
	float h = light->radius;

	Renderer2DSetShader(&shadow->shadow_shader);
	Renderer2DRenderTextureC(&light->shadow_map.texture, x, h + y, w, -h, view_proj, light->color);
}
