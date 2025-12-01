#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdint.h>
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

#define VEC_NEW(T, capacity) (T*)vec_new(sizeof(T), capacity)

#define VEC_COUNT(data) vec_header(data)->count
#define VEC_CAPACITY(data) vec_header(data)->capacity

#define VEC_ADD(data, value) ({ \
typeof(value) tmp_var = value; \
*(data) = vec_add(*(data), &tmp_var); \
})
#define VEC_POP(data, idx) vec_pop(data, idx)
#define VEC_REMOVE(data, el) {\
typeof(el) e = el;\
vec_remove(data, &e);\
}

#define VEC_ITERATOR(data) vec_header(data)

void* vec_new(size_t, size_t);

VecHeader* vec_header(void*);

void* vec_add(void*, void*);
void vec_pop(void*, int);
void vec_remove(void*, void*);

VecHeader* vec_iterator(void*);
void vec_start(void*);
void* vec_current(void*);
int vec_next(void*);

#endif