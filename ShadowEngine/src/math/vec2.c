#include "vec2.h"

vec2 vec2_mult(vec2 vec, float f)
{
	vec2 v;
	v.x = vec.x * f;
	v.y = vec.y * f;

	return v;
}

vec2 vec2_add(vec2 a, vec2 b)
{
	vec2 v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;

	return v;
}

vec2 vec2_sub(vec2 a, vec2 b)
{
	vec2 v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;

	return v;
}

int vec2_inside(vec2 point, vec2 min, vec2 max)
{
	if (point.x < min.x || point.y < min.y) return 0;
	if (point.x > max.x || point.y > max.y) return 0;

	return 1;
}
