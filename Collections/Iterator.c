#include <stdint.h>
#include "Iterator.h"

Range range(int64_t start, int64_t end, int64_t step) {
    return (Range) {
        .iterator = (Iterator) {
            .start = range_start,
            .current = range_current,
            .next = range_next
        },

        .start = start,
        .end = end,
        .step = step
    };
}

void range_start(void* ptr) {
    Range* rng = (Range*)ptr;

    rng->current = rng->start - rng->step;
}

void* range_current(void* ptr) {
    Range* rng = (Range*)ptr;
    return &rng->current;
}
int range_next(void* ptr) {
    Range* rng = (Range*)ptr;

    rng->current += rng->step;

    return rng->start < rng->end ?
                rng->current < rng->end :
                rng->current > rng->end;
}