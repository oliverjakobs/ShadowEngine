#ifndef WORLD_H
#define WORLD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Body.h"

#include "clib/vector.h"

#define WORLD_INITIAL_SIZE	8

CLIB_VECTOR_DECLARE_FUNCS(body, Body)

typedef struct World
{
	clib_vector bodies;
	vec2 gravity;
} World;

void WorldLoad(World* world, const vec2 gravity);
void WorldDestroy(World* world);

void WorldAddBody(World* world, Body* body);
void WorldRemoveBody(World* world, Body* body);

void WorldTick(World* world, float deltatime);

Body* WorldGetBody(World* world, size_t index);

#ifdef __cplusplus
}
#endif

#endif /* !WORLD_H */