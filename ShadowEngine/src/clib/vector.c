#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

int clib_vector_init(clib_vector* v, size_t initial_capacity)
{
    v->capacity = initial_capacity;
    v->size = 0;
    v->items = malloc(sizeof(void*) * v->capacity);

    if (v->items) return 1;
    return 0;
}

void clib_vector_free(clib_vector* v)
{
    v->capacity = 0;
    v->size = 0;

    if (v->items)
    {
        free(v->items);
        v->items = NULL;
    }
}

static void _clib_vector_resize(clib_vector *v, size_t capacity)
{
    void** items = realloc(v->items, sizeof(void*) * capacity);
    if (items) 
    {
        v->items = items;
        v->capacity = capacity;
    }
}

void clib_vector_push(clib_vector* v, void* item)
{
    if (v->capacity == v->size)
        _clib_vector_resize(v, v->capacity * 2);

    v->items[v->size++] = item;
}

void clib_vector_set(clib_vector* v, size_t index, void* item)
{
    if (index >= 0 && index < v->size)
        v->items[index] = item;
}

void clib_vector_delete(clib_vector* v, size_t index)
{
    if (index < 0 || index >= v->size)
        return;

    v->items[index] = NULL;

    for (int i = index; i < v->size - 1; i++)
    {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->size--;

    if (v->size > 0 && v->size == v->capacity / 4)
        _clib_vector_resize(v, v->capacity / 2);
}

void* clib_vector_get(const clib_vector* v, size_t index)
{
    if (index >= 0 && index < v->size)
        return v->items[index];

    return NULL;
}
