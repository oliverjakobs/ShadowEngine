#ifndef CLIB_VECTOR_H
#define CLIB_VECTOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CLIB_VECTOR_DECLARE_FUNCS(name, value_type)                         \
    void name##_vector_push(clib_vector* v, value_type* item);              \
    void name##_vector_set(clib_vector* v, size_t index, value_type* item); \
    void name##_vector_delete(clib_vector* v, size_t index);                \
    value_type* name##_vector_get(const clib_vector* v, size_t index);

#define CLIB_VECTOR_DEFINE_FUNCS(name, value_type)                          \
    void name##_vector_push(clib_vector* v, value_type* item)               \
    {                                                                       \
        clib_vector_push(v, (void*)item);                                   \
    }                                                                       \
    void name##_vector_set(clib_vector* v, size_t index, value_type* item)  \
    {                                                                       \
        clib_vector_set(v, index, (void*)item);                             \
    }                                                                       \
    void name##_vector_delete(clib_vector* v, size_t index)                 \
    {                                                                       \
        clib_vector_delete(v, index);                                       \
    }                                                                       \
    value_type* name##_vector_get(const clib_vector* v, size_t index)       \
    {                                                                       \
        return (value_type*)clib_vector_get(v, index);                      \
    }


    typedef struct
    {
        void** items;
        size_t capacity;
        size_t size;
    } clib_vector;

    int clib_vector_init(clib_vector* v, size_t initial_capacity);
    void clib_vector_free(clib_vector* v);

    void clib_vector_push(clib_vector* v, void* item);
    void clib_vector_set(clib_vector* v, size_t index, void* item);
    void clib_vector_delete(clib_vector* v, size_t index);

    void* clib_vector_get(const clib_vector* v, size_t index);

#ifdef __cplusplus
}
#endif

#endif /* !CLIB_VECTOR_H */