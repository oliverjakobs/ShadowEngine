#include "Shadow.h"

#include "Graphics/Renderer2D.h"

static GLint backup_viewport[4];
static GLfloat backup_clear_color[4];

void ShadowEngineCreateLight(Light* light, float x, float y, float radius)
{
	light->x = x;
	light->y = y;
	light->radius = radius;

	/* framebuffers */
	ignisGenerateFrameBuffer(&light->occlusion_map, GL_TEXTURE_2D, radius, radius);
	ignisGenerateFrameBuffer(&light->shadow_map, GL_TEXTURE_2D, radius, 1);
}

void ShadowEngineDeleteLight(Light* light)
{
	ignisDeleteFrameBuffer(&light->occlusion_map);
	ignisDeleteFrameBuffer(&light->shadow_map);
}

void ShadowEngineInit(ShadowEngine* shadow)
{
	/* shaders */
	ignisShadervf(&shadow->occlusion_shader, "res/shaders/pass.vert", "res/shaders/occlusion.frag");
	ignisShadervf(&shadow->shadow_map_shader, "res/shaders/pass.vert", "res/shaders/shadow_map.frag");
	ignisShadervf(&shadow->shadow_shader, "res/shaders/pass.vert", "res/shaders/shadow.frag");
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
	glGetIntegerv(GL_VIEWPORT, backup_viewport);
	glGetFloatv(GL_COLOR_CLEAR_VALUE, backup_clear_color);

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
	glViewport(backup_viewport[0], backup_viewport[1], backup_viewport[2], backup_viewport[3]);

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
	glClearColor(backup_clear_color[0], backup_clear_color[1], backup_clear_color[2], backup_clear_color[3]);
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
	Renderer2DRenderTexture(&light->shadow_map.texture, x, h + y, w, -h, view_proj);
}
