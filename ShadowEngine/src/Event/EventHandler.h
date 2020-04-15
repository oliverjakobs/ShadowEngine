#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Event.h"
#include "Application/Application.h"

#define EVENT_HANDLER_INITIAL_QUEUE_SIZE	4

int EventHandlerInit();
void EventHandlerDestroy();

void EventHandlerSetEventCallback(void (*callback)(Application*, const Event));

void EventHandlerThrowWindowEvent(EventType type, int width, int height);
void EventHandlerThrowKeyEvent(EventType type, int keycode, int repeatcount);
void EventHandlerThrowMouseButtonEvent(EventType type, int button);
void EventHandlerThrowMouseEvent(EventType type, float x, float y);

void EventHandlerPoll(Application* app);

#ifdef __cplusplus
}
#endif

#endif /* !EVENT_HANDLER_H */