#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdint.h>

/**
 * @brief struct for handle iteration with FOREACH macros

 * @warning filed in struct must be named "iterator" to use with FOREACH macros
 */
typedef struct {
    void (*start)(void*);
    void* (*current)(void*);
    int (*next)(void*);
} Iterator;

/**
 * @brief iterate for collection
 *
 * @param T pointer to value will be cast to T
 * @param var name of local variable
 * @param container pointer to struct with field with name iterator
 * @param block_code body of loop
 *
 * @note use {}-block in loop body
 */
#define FOREACH(T, var, container, block_code)                                           \
    ({                                                                                   \
        Iterator iter = (container)->iterator;                                           \
        iter.start(container);                                                           \
        T var;                                                                           \
        while (iter.next(container)) {                                                   \
            var = *(T*) iter.current(container);                                         \
            block_code                                                                   \
        }                                                                                \
    })

/**
 * @brief struct for
 */
typedef struct {
    Iterator iterator;
    int64_t current;

    int64_t start;
    int64_t end;
    int64_t step;
} Range;

#define RANGE(_start, _end, ...)                                                       \
    &(Range) { .start = _start, .end = _end, __VA_ARGS__ }

void range_start(void*);
void* range_current(void*);
int range_next(void*);

#endif
