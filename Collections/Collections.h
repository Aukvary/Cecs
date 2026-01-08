#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdint.h>
#include <stdlib.h>
#include "Iterator.h"

/**
 * @brief vector data
 */
typedef struct {
    size_t count;
    size_t capacity;

    size_t element_size;

    Iterator iterator;
    int iter_locked;
    size_t current;

    void* data;
} VecHeader;

/**
 * @brief wrapper
 */
#define VEC(T) T*

/**
* @brief return pointer to vector data array with type T
 *
 * @param T type for vector
 * @param capacity base capacity for vector
 */
#define VEC_NEW(T, capacity) (T*) vec_new(sizeof(T), capacity)

/**
 * @brief add value to vector
 *
 * @param data pointer to data array
 */
#define VEC_ADD(data, value)                                                             \
    ({                                                                                   \
        typeof(value) tmp_var = (value);                                                 \
        data = vec_add(data, &tmp_var);                                                  \
    })

/**
 * @brief remove value to vector
 *
 * @param data pointer to data array
 */
#define VEC_REMOVE(data, el)                                                             \
    {                                                                                    \
        typeof(el) e = el;                                                               \
        vec_remove(data, &e);                                                            \
    }

/**
 * @brief return header for using in FOREACH-macros
 *
 * @param data pointer to data array
 */
#define VEC_ITERATOR(data) vec_header(data)

/**
 * @brief return pointer to vector data array
 *
 * @param item_size size of type
 * @param capacity base capacity
 *
 * @warning use VEC_NEW macros-wrapper
 */
void* vec_new(size_t item_size, size_t capacity);

/**
 * @brief return header of vector info
 *
 * @param data pointer to data array
 */
VecHeader* vec_header(void* data);

/**
 * @brief return capacity of data array
 *
 * @param data pointer to data array
 */
inline size_t vec_capacity(void* data) { return vec_header(data)->capacity; }

/**
 * @brief return count of data array
 *
 * @param data pointer to data array
 */
inline size_t vec_count(void* data) { return vec_header(data)->count; }

/**
 * @brief add value to vector data array
 *
 * @param data pointer to data array
 * @param  value pointer to values
 *
 * @warning use VEC_ADD macros-wrapper
 */
void* vec_add(void* data, void* value);

/**
 * @brief remove value from data array with this index
 *
 * @param data pointer to data array
 * @param  idx value index
 */
void vec_pop(void* data, int idx);

/**
 * @brief remove value from data array
 *
 * @param data pointer to data array
 * @param  value pointer to values
 *
 * @warning use VEC_REMOVE macros-wrapper
 */
void vec_remove(void* data, void* value);

/**
 * @brief return header for using in FOREACH macros
 *
 * @param data pointer to data array
 *
 * @warning use VEC_ITERATOR macros-wrapper
 */
VecHeader* vec_iterator(void* data);

/**
 * @brief set interator pointer to start
 *
 * @param data pointer to vector header
 *
 * @warning func must be called by FOREACH macros else it can throw exceptions
 */
void vec_start(void* data);

/**
 * @brief return pointer to value
 *
 * @param data pointer to vector header
 *
 * @warning func must be called by FOREACH macros else it can throw exceptions
 */
void* vec_current(void* data);

/**
 * @brief move iterator pointer and return 1 if current != NULL else 0
 *
 * @param data pointer to vector header
 *
 * @warning func must be called by FOREACH macros else it can throw exceptions
 */
int vec_next(void* data);

/**
 * @brief free vector memory
 *
 * @param data pointer to vector header
 */
void vec_free(void* data);

#endif
