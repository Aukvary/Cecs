#include <string.h>
#include "RegisterHandler.h"

#ifndef DRAW_TABLE_SIZE
#define DRAW_TABLE_SIZE 107ULL
#endif

static int id_counter = 0;
static const DtDrawData* draw_data_by_id[DRAW_TABLE_SIZE] = {NULL};
static const DtDrawData* draw_data_by_name[DRAW_TABLE_SIZE] = {NULL};

static int dt_draw_get_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

void dt_draw_register(DtDrawData* data) {
    data->id = id_counter++;
    draw_data_by_id[data->id] = data;

    u64 idx = dt_draw_get_hash(data->name) % DRAW_TABLE_SIZE;
    const u64 start = idx;
    while (draw_data_by_name[idx] != NULL) {
        if (strcmp(data->name, draw_data_by_name[idx]->name) == 0)
            return;

        idx = (idx + 1) % DRAW_TABLE_SIZE;
        if (idx == start) {
            printf("[DEBUG]draw count out of range");
            exit(1);
        }
    }

    draw_data_by_name[idx] = data;
    printf("[DEBUD]%s draw system was registered with id %d\n", (char*) data->name, data->id);
}

const DtDrawData* dt_draw_get_data_by_name(const char* name) {
    u64 idx = dt_draw_get_hash(name) % DRAW_TABLE_SIZE;
    const u64 start = idx;

    while (draw_data_by_name[idx] != NULL && strcmp(draw_data_by_name[idx]->name, name) != 0) {
        idx = (idx + 1) % DRAW_TABLE_SIZE;
        if (idx == start) {
            return NULL;
        }
    }

    return draw_data_by_name[idx];
}

const DtDrawData* dt_draw_get_data_by_id(u16 id) {
    if (id >= DRAW_TABLE_SIZE) {
        return NULL;
    }

    return draw_data_by_id[id];
}

const DtDrawData** dt_draw_get_all(u16* size) {
    *size = id_counter;
    return draw_data_by_id;
}
