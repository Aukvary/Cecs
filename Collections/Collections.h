#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdint.h>
#include <stdlib.h>
#include "Iterator.h"

typedef struct {
    size_t count;
    size_t capacity;

    size_t element_size;

    Iterator iterator;
    int iter_locked;
    size_t current;

    void* data;
} VecHeader;

#define VEC(T) T*

#define VEC_NEW(T, capacity) (T*) vec_new(sizeof(T), capacity)

#define VEC_ADD(data, value)                                                             \
    ({                                                                                   \
        typeof(value) tmp_var = (value);                                                 \
        data = vec_add(data, &tmp_var);                                                  \
    })

#define VEC_REMOVE(data, el)                                                             \
    {                                                                                    \
        typeof(el) e = el;                                                               \
        vec_remove(data, &e);                                                            \
    }

#define VEC_ITERATOR(data) vec_header(data)

void* vec_new(size_t, size_t);

VecHeader* vec_header(void*);

inline size_t vec_capacity(void* data) { return vec_header(data)->capacity; }

inline size_t vec_count(void* data) { return vec_header(data)->count; }

void* vec_add(void*, void*);
void vec_pop(void*, int);
void vec_remove(void*, void*);

VecHeader* vec_iterator(void*);
void vec_start(void*);
void* vec_current(void*);
int vec_next(void*);

void vec_free(void* data);

#endif
