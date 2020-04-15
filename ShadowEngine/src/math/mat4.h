#ifndef MAT4_H
#define MAT4_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "vec3.h"

typedef struct
{
    float v[4 * 4];
} mat4;

float mat4_determinant(mat4 mat);

mat4 mat4_indentity();

mat4 mat4_transpose(mat4 mat);
mat4 mat4_inverse(mat4 mat);

mat4 mat4_translate(mat4 mat, vec3 v);
mat4 mat4_rotate(mat4 mat, vec3 axis, float angle); /* Angle should be provided in radians */
mat4 mat4_scale(mat4 mat, vec3 v);

mat4 mat4_multiply(mat4 left, mat4 right);

mat4 mat4_frustum(float left, float right, float bottom, float top, float near, float far);
mat4 mat4_perspective(float fov_y, float aspect, float near, float far);
mat4 mat4_ortho(float left, float right, float bottom, float top, float near, float far);
mat4 mat4_look_at(vec3 position, vec3 target, vec3 up);

#ifdef __cplusplus
}
#endif

#endif /* !MAT4_H */
