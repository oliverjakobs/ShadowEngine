#include "Camera.h"

int CameraCreate(Camera* camera, const vec3 pos, const vec2 size)
{
	camera->view = mat4_indentity();
	camera->projection = mat4_indentity();
	camera->viewProjection = mat4_indentity();

	camera->position = pos;
	camera->size = size;

	return 1;
}

int CameraCreateOrtho(Camera* camera, const vec3 center, const vec2 size)
{
	int result = CameraCreate(camera, center, size);
	CameraSetProjectionOrtho(camera, size.x, size.y);

	return result;
}

void CameraUpdateViewOrtho(Camera* camera)
{
	mat4 transform = mat4_indentity();
	transform = mat4_translate(transform, camera->position);

	camera->view = mat4_inverse(transform);
	camera->viewProjection = mat4_multiply(camera->projection, camera->view);
}

void CameraSetProjectionOrtho(Camera* camera, float w, float h)
{
	camera->projection = mat4_ortho(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, -1.0f, 1.0f);

	CameraUpdateViewOrtho(camera);
}

void CameraSetProjectionOrthoVec2(Camera* camera, const vec2 size)
{
	CameraSetProjectionOrtho(camera, size.x, size.y);
}

const vec2 CameraGetMousePos(Camera* camera, const vec2 mouse)
{
	float x = mouse.x + (camera->position.x - (camera->size.x / 2.0f));
	float y = (camera->size.y - mouse.y) + (camera->position.y - (camera->size.y / 2.0f));

	return (vec2){ x, y };
}

const float* CameraGetViewPtr(Camera* camera)
{
	return camera->view.v;
}

const float* CameraGetProjectionPtr(Camera* camera)
{
	return camera->projection.v;
}

const float* CameraGetViewProjectionPtr(Camera* camera)
{
	return camera->viewProjection.v;
}
