#include "Collections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* dt_vec_new(const size_t item_size, const size_t capacity) {
    DtVecHeader* header = malloc(sizeof(DtVecHeader) + item_size * capacity);

    *header = (DtVecHeader) {
        .element_size = item_size,
        .capacity = capacity,
        .count = 0,

        .data = header + 1,

        .iterator =
            (DtIterator) {
                .start = dt_vec_start,
                .current = dt_vec_current,
                .next = dt_vec_next,
                .enumerable = header
            },
        .iter_locked = 0,
    };

    return header->data;
}

void* dt_vec_add(void* data, void* value) {
    DtVecHeader* header = dt_vec_header(data);

    if (header->count == header->capacity) {
        size_t new_capacity = header->capacity == 0 ? 10 : header->capacity * 2;
        DtVecHeader* temp =
            realloc(header, sizeof(DtVecHeader) + new_capacity * header->element_size);

        if (temp == NULL) {
            printf("vector memory allocation exception\n");
            exit(1);
        }

        header = temp;
        header->data = header + 1;
        header->capacity = new_capacity;
        header->iterator.enumerable = header;

        data = header->data;
    }

    uint8_t* data_ptr = header->data;
    size_t offset = header->count * header->element_size;

    memcpy(data_ptr + offset, value, header->element_size);
    header->count++;

    return header->data;
}

void dt_vec_pop(void* data, int idx) {
    DtVecHeader* header = dt_vec_header(data);

    if (header->iter_locked) {
        printf("cant change vector where it's locked\n");
        exit(1);
    }
    if (idx >= header->count)
        exit(1);

    for (int i = idx; i < header->count - 1; i++) {
        memcpy((uint8_t*) data + header->element_size * i,
               (uint8_t*) data + header->element_size * (i + 1), header->element_size);
    }

    header->count--;
}

void dt_vec_remove(void* data, void* value) {
    DtVecHeader* header = dt_vec_header(data);

    if (header->iter_locked) {
        printf("cant change vector where it's locked\n");
        exit(1);
    }

    int idx;

    for (int i = 0; i < header->count; i++) {
        if (memcmp(value, (uint8_t*) data + header->element_size * i,
                   header->element_size) == 0) {
            idx = i;
            header->count--;
            break;
        }
    }

    for (int i = idx; i < header->count; i++) {
        memcpy((uint8_t*) data + header->element_size * i,
               (uint8_t*) data + header->element_size * (i + 1), header->element_size);
    }
}

void dt_vec_start(void* data) {
    DtVecHeader* header = data;
    header->current = -1;
}

void* dt_vec_current(void* data) {
    DtVecHeader* header = data;

    if (header->current >= header->count) {
        return NULL;
    }

    void* element = (char*) header->data + (header->current * header->element_size);
    return element;
}

bool dt_vec_next(void* data) {
    DtVecHeader* header = data;

    header->current++;

    return header->current < header->count;
}

inline void dt_vec_free(void* data) { free(dt_vec_header(data)); }
