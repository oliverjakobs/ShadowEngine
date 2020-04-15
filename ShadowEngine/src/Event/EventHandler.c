#include "EventHandler.h"

#include "clib/vector.h"

CLIB_VECTOR_DEFINE_FUNCS(event_queue, Event)

typedef struct
{
	clib_vector queue;
	void (*callback)(Application*, const Event);
} EventHandler;

static EventHandler event_handler;

int EventHandlerInit()
{
	clib_vector_init(&event_handler.queue, EVENT_HANDLER_INITIAL_QUEUE_SIZE);

	return 0;
}

void EventHandlerDestroy()
{
	for (size_t i = 0; i < event_handler.queue.size; i++)
		free(event_queue_vector_get(&event_handler.queue, i));
	clib_vector_free(&event_handler.queue);
}

void EventHandlerSetEventCallback(void (*callback)(Application*, const Event))
{
	event_handler.callback = callback;
}

void EventHandlerThrowWindowEvent(EventType type, int width, int height)
{
	Event* e = (Event*)malloc(sizeof(Event));

	if (e)
	{
		e->type = type;
		e->window.width = width;
		e->window.height = height;
		event_queue_vector_push(&event_handler.queue, e);
	}
}

void EventHandlerThrowKeyEvent(EventType type, int keycode, int repeatcount)
{
	Event* e = (Event*)malloc(sizeof(Event));

	if (e)
	{
		e->type = type;
		e->key.keycode = keycode;
		e->key.repeatcount = repeatcount;
		event_queue_vector_push(&event_handler.queue, e);
	}
}

void EventHandlerThrowMouseButtonEvent(EventType type, int button)
{
	Event* e = (Event*)malloc(sizeof(Event));

	if (e)
	{
		e->type = type;
		e->mousebutton.buttoncode = button;
		event_queue_vector_push(&event_handler.queue, e);
	}
}

void EventHandlerThrowMouseEvent(EventType type, float x, float y)
{
	Event* e = (Event*)malloc(sizeof(Event));

	if (e)
	{
		e->type = EVENT_MOUSE_MOVED;
		e->mouse.x = x;
		e->mouse.y = y;
		event_queue_vector_push(&event_handler.queue, e);
	}
}

void EventHandlerPoll(Application* app)
{
	while (event_handler.queue.size > 0)
	{
		Event* e = event_queue_vector_get(&event_handler.queue, 0);

		event_handler.callback(app, *e);

		event_queue_vector_delete(&event_handler.queue, 0);

		free(e);
	}
}
