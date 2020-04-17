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
	shadow->resolution = 512.0f;

	ignisCreateQuadTextured(&shadow->quad, GL_DYNAMIC_DRAW);

	/* shaders */
	ignisCreateShadervf(&shadow->occlusion_shader, "res/shaders/pass.vert", "res/shaders/occlusion.frag");
	ignisCreateShadervf(&shadow->shadow_map_shader, "res/shaders/pass.vert", "res/shaders/shadow_map.frag");
	ignisCreateShadervf(&shadow->shadow_shader, "res/shaders/pass.vert", "res/shaders/shadow.frag");
}

void ShadowEngineDestroy(ShadowEngine* shadow)
{
	ignisDeleteQuad(&shadow->quad);

	ignisDeleteShader(&shadow->occlusion_shader);
	ignisDeleteShader(&shadow->shadow_map_shader);
	ignisDeleteShader(&shadow->shadow_shader);
}

void ShadowEngineStartLight(ShadowEngine* shadow, Light* light)
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

static void ShadowEngineSetQuadVertices(ShadowEngine* shadow, float x, float y, float radius)
{
	GLfloat vertices[] =
	{
		0.0f, 0.0f, x, y,
		1.0f, 0.0f, x + radius, y,
		1.0f, 1.0f, x + radius, y + radius,
		0.0f, 1.0f, x, y + radius
	};

	ignisBufferSubData(&shadow->quad.vao.array_buffers[0], 0, sizeof(vertices), vertices);
}

void ShadowEngineProcessLight(ShadowEngine* shadow, Light* light, const float* view_proj)
{
	/* create shadow map*/
	ignisBindFrameBuffer(&light->shadow_map);
	glClear(GL_COLOR_BUFFER_BIT);

	float resolution = shadow->resolution;
	float radius = light->radius;

	glViewport(0, 0, 2 * resolution, radius);

	/* creating a mat4 that translates and scales */
	float model[16];
	model[0] = radius;	model[4] = 0.0f;	model[8] = 0.0f;	model[12] = 0.0f;
	model[1] = 0.0f;	model[5] = radius;	model[9] = 0.0f;	model[13] = 0.0f;
	model[2] = 0.0f;	model[6] = 0.0f;	model[10] = 1.0f;	model[14] = 0.0f;
	model[3] = 0.0f;	model[7] = 0.0f;	model[11] = 0.0f;	model[15] = 1.0f;

	ignisUseShader(&shadow->shadow_map_shader);

	ignisSetUniformMat4(&shadow->shadow_map_shader, "u_Model", model);
	ignisSetUniformMat4(&shadow->shadow_map_shader, "u_ViewProjection", view_proj);

	ignisSetUniform1f(&shadow->shadow_map_shader, "u_Resolution", resolution);

	ignisBindTexture2D(&light->occlusion_map.texture, 0);

	// ShadowEngineSetQuadVertices(shadow, light->x, light->y, radius);
	ignisDrawQuadElements(&shadow->quad, GL_TRIANGLES);

	/* RESET */
	ignisBindFrameBuffer(NULL);
	ignisClearColor(shadow->backup_clear_color);
	glViewport(shadow->backup_viewport[0], shadow->backup_viewport[1], shadow->backup_viewport[2], shadow->backup_viewport[3]);
}

void ShadowEngineRenderStart(ShadowEngine* shadow)
{
	Renderer2DSetShader(&shadow->shadow_shader);
}

void ShadowEngineRenderFlush(ShadowEngine* shadow)
{
	glViewport(shadow->backup_viewport[0], shadow->backup_viewport[1], shadow->backup_viewport[2], shadow->backup_viewport[3]);
}

void ShadowEngineRenderLight(ShadowEngine* shadow, Light* light)
{
	ignisSetUniform1f(&shadow->shadow_shader, "u_SoftShadows", 1.0f);
	ignisSetUniform1f(&shadow->shadow_shader, "u_Resolution", light->radius);

	//draw centered on light position
	float x = light->x - light->radius / 2.0f;
	float y = light->y - light->radius / 2.0f;
	float w = light->radius;
	float h = light->radius;

	glViewport(x, y, w, h);

	shadow->light_camera.position = (vec3){ light->x, light->y, 0.0f };
	CameraSetProjectionOrtho(&shadow->light_camera, w, -h);

	Renderer2DRenderTextureC(&light->shadow_map.texture, x, y, w, h, CameraGetViewProjectionPtr(&shadow->light_camera), light->color);
}
