#include <string.h>
#include "RegisterHandler.h"

#ifndef UPDATE_TABLE_SIZE
#define UPDATE_TABLE_SIZE 107ULL
#endif

static int id_counter = 0;
static const DtUpdateData* update_data_by_id[UPDATE_TABLE_SIZE] = {NULL};
static const DtUpdateData* update_data_by_name[UPDATE_TABLE_SIZE] = {NULL};

static int dt_update_get_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

void dt_update_register(DtUpdateData* data) {
    data->id = id_counter++;
    update_data_by_id[data->id] = data;

    u64 idx = dt_update_get_hash(data->name) % UPDATE_TABLE_SIZE;
    const u64 start = idx;
    while (update_data_by_name[idx] != NULL) {
        if (strcmp(data->name, update_data_by_name[idx]->name) == 0)
            return;

        idx = (idx + 1) % UPDATE_TABLE_SIZE;
        if (idx == start) {
            printf("[DEBUG]update count out of range");
            exit(1);
        }
    }

    update_data_by_name[idx] = data;
    printf("[DEBUD]%s update system was registered with id %d\n", (char*) data->name, data->id);
}

const DtUpdateData* dt_update_get_data_by_name(const char* name) {
    u64 idx = dt_update_get_hash(name) % UPDATE_TABLE_SIZE;
    const u64 start = idx;

    while (update_data_by_name[idx] != NULL && strcmp(update_data_by_name[idx]->name, name) != 0) {
        idx = (idx + 1) % UPDATE_TABLE_SIZE;
        if (idx == start) {
            return NULL;
        }
    }

    return update_data_by_name[idx];
}

const DtUpdateData* dt_update_get_data_by_id(u16 id) {
    if (id >= UPDATE_TABLE_SIZE) {
        return NULL;
    }

    return update_data_by_id[id];
}

const DtUpdateData** dt_update_get_all(u16* size) {
    *size = id_counter;
    return update_data_by_id;
}
