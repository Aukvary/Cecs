#include <stdint.h>
#include "Collections.h"

DtRange range(int64_t start, int64_t end, int64_t step) {
    return (DtRange) {
        .iterator = (DtIterator) {
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
    DtRange* rng = (DtRange*)ptr;

    rng->current = rng->start - rng->step;
}

void* range_current(void* ptr) {
    DtRange* rng = (DtRange*)ptr;
    return &rng->current;
}
int range_next(void* ptr) {
    DtRange* rng = (DtRange*)ptr;

    rng->current += rng->step;

    return rng->start < rng->end ?
                rng->current < rng->end :
                rng->current > rng->end;
}