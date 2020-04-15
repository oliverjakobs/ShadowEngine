#include "Body.h"

#include "World.h"

#include "clib/clib.h"

void BodyLoad(Body* body, float x, float y, float hw, float hh, BodyType type)
{
	body->position = (vec2){ x, y };
	body->halfSize = (vec2){ hw, hh };
	body->velocity = (vec2){ 0.0f, 0.0f };
	
	body->type = type;
	
	body->world = NULL;

	body->collidesBottom = 0;
	body->collidesTop = 0;
	body->collidesLeft = 0;
	body->collidesRight = 0;
}

void BodyTick(Body* body, float deltatime)
{
	body->velocity = vec2_add(body->velocity, vec2_mult(body->world->gravity, deltatime));

	body->collidesBottom = 0;
	body->collidesTop = 0;
	body->collidesLeft = 0;
	body->collidesRight = 0;

	body->position = vec2_add(body->position, vec2_mult(body->velocity, deltatime));
}

void BodyResolveCollision(Body* body, const Body* other, const vec2 oldpos)
{
	float overlap_x = MIN(BodyGetX2(body) - BodyGetX(other), BodyGetX2(other) - BodyGetX(body));
	float overlap_y = MIN((oldpos.y + body->halfSize.y) - BodyGetY(other), BodyGetY2(other) - (oldpos.y - body->halfSize.y));

	// horizontal resolve
	if (overlap_x > 0.0f && overlap_y > 0.0f)
	{
		if (body->velocity.x < 0.0f)
		{
			body->position.x += MAX(overlap_x, 0.0f);
			body->velocity.x = 0.0f;
			body->collidesLeft = 1;
		}
		else if (body->velocity.x > 0.0f)
		{
			body->position.x -= MAX(overlap_x, 0.0f);
			body->velocity.x = 0.0f;
			body->collidesRight = 1;
		}
	}

	overlap_x = MIN((oldpos.x + body->halfSize.x) - BodyGetX(other), BodyGetX2(other) - (oldpos.x - body->halfSize.x));
	overlap_y = MIN(BodyGetY2(body) - BodyGetY(other), BodyGetY2(other) - BodyGetY(body));

	// vertical resolve
	if (overlap_x > 0.0f && overlap_y > 0.0f)
	{
		if (body->velocity.y < 0.0f)
		{
			body->position.y += MAX(overlap_y, 0.0f);
			body->velocity.y = 0.0f;
			body->collidesBottom = 1;
		}
		else if (body->velocity.y > 0.0f)
		{
			body->position.y -= MAX(overlap_y, 0.0f);
			body->velocity.y = 0.0f;
			body->collidesTop = 1;
		}
	}
}

const float BodyGetX(const Body* body)
{
	return body->position.x - body->halfSize.x;
}

const float BodyGetX2(const Body* body)
{
	return body->position.x + body->halfSize.x;
}

const float BodyGetY(const Body* body)
{
	return body->position.y - body->halfSize.y;
}

const float BodyGetY2(const Body* body)
{
	return body->position.y + body->halfSize.y;
}

rect BodyGetRect(Body* body)
{
	return (rect) { body->position.x - body->halfSize.x, body->position.y - body->halfSize.y, body->halfSize.x * 2.0f, body->halfSize.y * 2.0f };
}
