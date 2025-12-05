#include "Collections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* vec_new(size_t el_size, size_t capacity) {
    VecHeader* header = malloc(sizeof(VecHeader) + el_size * capacity);
    ;

    *header = (VecHeader) {
        .element_size = el_size,
        .capacity = capacity,
        .count = 0,

        .data = header + 1,

        .iterator =
            (Iterator) {
                .start = vec_start,
                .current = vec_current,
                .next = vec_next,
            },
        .iter_locked = 0,
    };

    return header->data;
}

inline VecHeader* vec_header(void* data) {
    return (VecHeader*) ((uint8_t*) data - sizeof(VecHeader));
}

void* vec_add(void* data, void* el) {
    VecHeader* header = vec_header(data);

    if (header->count == header->capacity) {
        size_t new_capacity = header->capacity == 0 ? 1 : header->capacity * 2;
        VecHeader* temp =
            realloc(header, sizeof(VecHeader) + new_capacity * header->element_size);

        if (temp == NULL) {
            printf("vector memory allocation exception\n");
            exit(1);
        }

        header = temp;
        header->data = header + 1;
        header->capacity = new_capacity;

        data = header->data;
    }

    uint8_t* data_ptr = header->data;
    size_t offset = header->count * header->element_size;

    memcpy(data_ptr + offset, el, header->element_size);
    header->count++;

    return header->data;
}

void vec_pop(void* data, int idx) {
    VecHeader* header = vec_header(data);

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

void vec_remove(void* data, void* el) {
    VecHeader* header = vec_header(data);

    if (header->iter_locked) {
        printf("cant change vector where it's locked\n");
        exit(1);
    }

    int idx;

    for (int i = 0; i < header->count; i++) {
        if (memcmp(el, (uint8_t*) data + header->element_size * i,
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

void vec_start(void* data) {
    VecHeader* header = data;
    header->current = -1;
}

void* vec_current(void* data) {
    VecHeader* header = data;

    if (header->current >= header->count) {
        return NULL;
    }

    void* element = (char*) header->data + (header->current * header->element_size);
    return element;
}

int vec_next(void* data) {
    VecHeader* header = data;

    header->current++;

    return header->current < header->count;
}

inline void vec_free(void* data) { free(vec_header(data)); }
