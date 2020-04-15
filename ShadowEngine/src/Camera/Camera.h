#ifndef CAMERA_H
#define CAMERA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "math/math.h"

typedef struct
{
	mat4 view;
	mat4 projection;
	mat4 viewProjection;

	vec3 position;
	vec2 size;
} Camera;

int CameraCreate(Camera* camera, const vec3 pos, const vec2 size);
int CameraCreateOrtho(Camera* camera, const vec3 center, const vec2 size);

/*
 * Call after changing size or position
 */
void CameraUpdateViewOrtho(Camera* camera);

void CameraSetProjectionOrtho(Camera* camera, float w, float h);
void CameraSetProjectionOrthoVec2(Camera* camera, const vec2 size);

const vec2 CameraGetMousePos(Camera* camera, const vec2 mouse);

const float* CameraGetViewPtr(Camera* camera);
const float* CameraGetProjectionPtr(Camera* camera);
const float* CameraGetViewProjectionPtr(Camera* camera);

#ifdef __cplusplus
}
#endif

#endif /* !CAMERA_H */