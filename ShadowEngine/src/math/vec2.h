#ifndef VEC2_H
#define VEC2_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	float x;
	float y;
} vec2;

vec2 vec2_mult(vec2 v, float f);
vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);

int vec2_inside(vec2 point, vec2 min, vec2 max);

#ifdef __cplusplus
}
#endif

#endif /* !VEC2_H */
