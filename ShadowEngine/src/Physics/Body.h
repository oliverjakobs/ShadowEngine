#ifndef BODY_H
#define BODY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "math/math.h"

struct World;

typedef enum
{
	BODY_TYPE_STATIC,
	BODY_TYPE_DYNAMIC
} BodyType;

typedef struct
{
	vec2 position;
	vec2 halfSize;

	vec2 velocity;

	BodyType type;

	// CollisionState
	int collidesBottom;
	int collidesTop;
	int collidesLeft;
	int collidesRight;

	struct World* world;
} Body;

void BodyLoad(Body* body, float x, float y, float hw, float hh, BodyType type);

void BodyTick(Body* body, float deltatime);

void BodyResolveCollision(Body* body, const Body* other, const vec2 oldpos);

/* get the edges of the body */
const float BodyGetX(const Body* body);
const float BodyGetX2(const Body* body);
const float BodyGetY(const Body* body);
const float BodyGetY2(const Body* body);

rect BodyGetRect(Body* body);

#ifdef __cplusplus
}
#endif

#endif /* !BODY_H */