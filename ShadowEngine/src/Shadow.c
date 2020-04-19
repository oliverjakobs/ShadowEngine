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

void ShadowEngineInit(ShadowEngine* shadow, int width, int height)
{
	ignisCreateQuadTextured(&shadow->quad, GL_DYNAMIC_DRAW);

	ignisGenerateFrameBuffer(&shadow->occlusion_map, width, height);

	/* shaders */
	ignisCreateShadervf(&shadow->occlusion_shader, "res/shaders/pass.vert", "res/shaders/occlusion.frag");
	ignisCreateShadervf(&shadow->shadow_map_shader, "res/shaders/pass.vert", "res/shaders/shadow_map.frag");
	ignisCreateShadervf(&shadow->shadow_shader, "res/shaders/pass.vert", "res/shaders/shadow.frag");
}

void ShadowEngineDestroy(ShadowEngine* shadow)
{
	ignisDeleteQuad(&shadow->quad);

	ignisDeleteFrameBuffer(&shadow->occlusion_map);

	ignisDeleteShader(&shadow->occlusion_shader);
	ignisDeleteShader(&shadow->shadow_map_shader);
	ignisDeleteShader(&shadow->shadow_shader);
}

void ShadowEngineStart(ShadowEngine* shadow)
{
	/* save previous gl state */
	glGetIntegerv(GL_VIEWPORT, shadow->backup_viewport);
	glGetFloatv(GL_COLOR_CLEAR_VALUE, &shadow->backup_clear_color.r);

	/* create occlusion map */
	ignisBindFrameBuffer(&shadow->occlusion_map);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void ShadowEngineSetQuadVertices(ShadowEngine* shadow, float x, float y, float radius)
{
	float s1 = x / shadow->occlusion_map.width;
	float t1 = y / shadow->occlusion_map.height;
	float s2 = s1 + (radius / shadow->occlusion_map.width);
	float t2 = t1 + (radius / shadow->occlusion_map.height);

	GLfloat vertices[] =
	{
		0.0f, 0.0f, s1, t1,
		1.0f, 0.0f, s2, t1,
		1.0f, 1.0f, s2, t2,
		0.0f, 1.0f, s1, t2
	};

	ignisBufferSubData(&shadow->quad.vao.array_buffers[0], 0, sizeof(vertices), vertices);
}

void ShadowEngineProcessLight(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	ignisBindFrameBuffer(&light->occlusion_map);
	glClear(GL_COLOR_BUFFER_BIT);

	float radius = light->radius;
	float x = light->x - (radius / 2.0f);
	float y = light->y - (radius / 2.0f);

	ShadowEngineSetQuadVertices(shadow, x, y, radius);

	/* creating a mat4 that translates and scales */
	float model[16];
	model[0] = radius;	model[4] = 0.0f;	model[8] = 0.0f;	model[12] = 0.0f;
	model[1] = 0.0f;	model[5] = radius;	model[9] = 0.0f;	model[13] = 0.0f;
	model[2] = 0.0f;	model[6] = 0.0f;	model[10] = 1.0f;	model[14] = 0.0f;
	model[3] = 0.0f;	model[7] = 0.0f;	model[11] = 0.0f;	model[15] = 1.0f;

	Renderer2DSetShader(NULL);
	Renderer2DRenderTextureQuad(&shadow->occlusion_map.texture, &shadow->quad, model, view_proj, &IGNIS_WHITE.r);

	/* create shadow map*/
	ignisBindFrameBuffer(&light->shadow_map);
	glClear(GL_COLOR_BUFFER_BIT);

	ignisSetUniform1f(&shadow->shadow_map_shader, "u_Resolution", radius);

	Renderer2DSetShader(&shadow->shadow_map_shader);
	Renderer2DRenderTexture(&light->occlusion_map.texture, 0.0f, 0.0f, radius, radius, view_proj);
}

void ShadowEngineRenderStart(ShadowEngine* shadow)
{
	/* RESET */
	ignisBindFrameBuffer(NULL);

	Renderer2DSetShader(&shadow->shadow_shader);
}

void ShadowEngineRenderFlush(ShadowEngine* shadow)
{
	glViewport(shadow->backup_viewport[0], shadow->backup_viewport[1], shadow->backup_viewport[2], shadow->backup_viewport[3]);

	ignisClearColor(shadow->backup_clear_color);
}

void ShadowEngineRenderLight(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	ignisSetUniform1f(&shadow->shadow_shader, "u_SoftShadows", 1.0f);
	ignisSetUniform1f(&shadow->shadow_shader, "u_Resolution", light->radius);

	//draw centered on light position
	float x = light->x - light->radius / 2.0f;
	float y = light->y - light->radius / 2.0f;
	float w = light->radius;
	float h = light->radius;

	Renderer2DRenderTextureC(&light->shadow_map.texture, x, h + y, w, -h, view_proj, light->color);
}
