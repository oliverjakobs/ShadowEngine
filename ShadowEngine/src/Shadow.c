#include "Shadow.h"

#include "Graphics/Renderer2D.h"
#include "Graphics/BatchRenderer2D.h"

#include "Camera/Camera.h"

void GenerateTexStorage2D(IgnisTexture2D* tex, int width, int height, GLenum internal_format)
{
	glGenTextures(1, &tex->name);
	glBindTexture(GL_TEXTURE_2D, tex->name);
	glTexStorage2D(GL_TEXTURE_2D, 8, internal_format, width, height);

	tex->width = width;
	tex->height = height;
	tex->rows = 1;
	tex->columns = 1;
}

void DeleteTexStorage2D(IgnisTexture2D* tex)
{
	glDeleteTextures(1, &tex->name);
}

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
	ignisCreateQuadTextured(&shadow->light_quad, GL_DYNAMIC_DRAW);
	ignisCreateQuadTextured(&shadow->full_quad, GL_STATIC_DRAW);

	ignisGenerateFrameBuffer(&shadow->scene_buffer, width, height);

	/* shaders */
	ignisCreateShadervf(&shadow->occlusion_shader, "res/shaders/pass.vert", "res/shaders/occlusion.frag");
	ignisCreateShadervf(&shadow->shadow_map_shader, "res/shaders/pass.vert", "res/shaders/shadow_map.frag");
	ignisCreateShadervf(&shadow->shadow_shader, "res/shaders/pass.vert", "res/shaders/shadow.frag");
}

void ShadowEngineDestroy(ShadowEngine* shadow)
{
	ignisDeleteQuad(&shadow->light_quad);
	ignisDeleteQuad(&shadow->full_quad);

	ignisDeleteFrameBuffer(&shadow->scene_buffer);

	ignisDeleteShader(&shadow->occlusion_shader);
	ignisDeleteShader(&shadow->shadow_map_shader);
	ignisDeleteShader(&shadow->shadow_shader);
}

void ShadowEngineStart(ShadowEngine* shadow)
{
	/* save previous gl state */
	glGetIntegerv(GL_VIEWPORT, shadow->backup_viewport);
	glGetFloatv(GL_COLOR_CLEAR_VALUE, &shadow->backup_clear_color.r);
}

void ShadowEngineProcess(ShadowEngine* shadow, Light* lights, size_t count, const float* view_proj)
{
	for (size_t i = 0; i < count; ++i)
	{
		ShadowEngineProcessLight(shadow, &lights[i], view_proj);
	}
}

void ShadowEngineRender(ShadowEngine* shadow, Light* lights, size_t count, const float* view_proj)
{
	/* RESET */
	ignisBindFrameBuffer(NULL);

	Renderer2DSetShader(&shadow->shadow_shader);

	for (size_t i = 0; i < count; ++i)
	{
		ShadowEngineRenderLight(shadow, &lights[i], view_proj);
	}
}

void ShadowEngineFinish(ShadowEngine* shadow)
{
	glViewport(shadow->backup_viewport[0], shadow->backup_viewport[1], shadow->backup_viewport[2], shadow->backup_viewport[3]);

	ignisClearColor(shadow->backup_clear_color);
	Renderer2DSetShader(NULL);
}

void ShadowEngineProcessLight(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	ignisBindFrameBuffer(&light->occlusion_map);
	glClear(GL_COLOR_BUFFER_BIT);

	float r = light->radius;
	float x = light->x - (r / 2.0f);
	float y = light->y - (r / 2.0f);

	/* set quad vertices */
	float s1 = x / shadow->scene_buffer.width;
	float t1 = y / shadow->scene_buffer.height;
	float s2 = s1 + (r / shadow->scene_buffer.width);
	float t2 = t1 + (r / shadow->scene_buffer.height);

	GLfloat vertices[] =
	{
		0.0f, 0.0f, s1, t1,
		1.0f, 0.0f, s2, t1,
		1.0f, 1.0f, s2, t2,
		0.0f, 1.0f, s1, t2
	};

	ignisBufferSubData(&shadow->light_quad.vao.array_buffers[0], 0, sizeof(vertices), vertices);

	/* creating a mat4 that scales to light radius */
	float model[16];
	model[0] = r;    model[4] = 0.0f; model[8] = 0.0f;  model[12] = 0.0f;
	model[1] = 0.0f; model[5] = r;    model[9] = 0.0f;  model[13] = 0.0f;
	model[2] = 0.0f; model[6] = 0.0f; model[10] = 1.0f; model[14] = 0.0f;
	model[3] = 0.0f; model[7] = 0.0f; model[11] = 0.0f; model[15] = 1.0f;

	Renderer2DSetShader(NULL);
	Renderer2DRenderTextureQuad(&shadow->scene_buffer.texture, &shadow->light_quad, model, view_proj, IGNIS_WHITE);

	/* create shadow map*/
	ignisBindFrameBuffer(&light->shadow_map);
	glClear(GL_COLOR_BUFFER_BIT);

	ignisUseShader(&shadow->shadow_map_shader);

	ignisSetUniform1f(&shadow->shadow_map_shader, "u_Resolution", r);
	ignisSetUniformMat4(&shadow->shadow_map_shader, "u_ViewProjection", view_proj);
	ignisSetUniformMat4(&shadow->shadow_map_shader, "u_Model", model);

	ignisBindTexture2D(&light->occlusion_map.texture, 0);

	ignisDrawQuadElements(&shadow->full_quad, GL_TRIANGLES);
}

void ShadowEngineRenderLight(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	/* draw centered on light position */
	float x = light->x - light->radius / 2.0f;
	float y = light->y - light->radius / 2.0f;
	float r = light->radius;

	ignisSetUniform1f(&shadow->shadow_shader, "u_SoftShadows", 1.0f);
	ignisSetUniform1f(&shadow->shadow_shader, "u_Resolution", r);

	Renderer2DRenderTextureColor(&light->shadow_map.texture, x, r + y, r, -r, view_proj, light->color);
}

void ShadowEngineStartOcclusion(ShadowEngine* shadow, const float* view_proj)
{
	/* bind occlusion map */
	ignisBindFrameBuffer(&shadow->scene_buffer);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	BatchRenderer2DStart(view_proj);
}

void ShadowEngineFlushOcclusion(ShadowEngine* shadow)
{
	BatchRenderer2DFlush();
}

void ShadowEngineRenderOccluder(ShadowEngine* shadow, IgnisTexture2D* texture, float x, float y, float w, float h)
{
	BatchRenderer2DRenderTexture(texture, x, y, w, h);
}
