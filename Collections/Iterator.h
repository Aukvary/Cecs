#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdint.h>

typedef struct {
    void(*start)(void*);
    void*(*current)(void*);
    int(*next)(void*);
} Iterator;

#define FOREACH(T, var, container, block_code) ({ \
    Iterator iter = (container).iterator; \
    typeof(container) tmp = container; \
    iter.start(&tmp); \
    T var; \
    while (iter.next(&tmp)) { \
        var = *(T*)iter.current(&tmp); \
        block_code \
    }\
})

typedef struct {
    Iterator iterator;
    int64_t current;

    int64_t start;
    int64_t end;
    int64_t step;
} Range;

Range range(int64_t, int64_t, int64_t);

void range_start(void*);
void* range_current(void*);
int range_next(void*);

#endif