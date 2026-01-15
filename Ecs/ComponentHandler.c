#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ComponentsHandler.h"


#ifndef COMPONENT_TABLE_SIZE
#define COMPONENT_TABLE_SIZE 107
#endif

static int id_counter = 0;
static const DtComponentData* component_data_by_id[COMPONENT_TABLE_SIZE] = {NULL};
static const DtComponentData* component_data_by_name[COMPONENT_TABLE_SIZE] = {NULL};

static int component_get_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

void register_component(DtComponentData* data) {
    if (component_get_data_by_name(data->name) != NULL) return;

    data->id = id_counter++;

    component_data_by_id[data->id] = data;

    int idx = component_get_hash(data->name) % COMPONENT_TABLE_SIZE;
    const int start = idx;
    while (component_data_by_name[idx] != NULL) {
        idx = (idx + 1) % COMPONENT_TABLE_SIZE;
        if (idx == start) {
            printf("[DEBUG]component count out of range");
            exit(1);
        }
    }

    component_data_by_name[idx] = data;
    printf("[DEBUD]%s component was registred with id %d\n", data->name, data->id);
}

const DtComponentData* component_get_data_by_id(const u16 id) {
    if (id < 0 || id >= COMPONENT_TABLE_SIZE) {
        return NULL;
    }

    if (component_data_by_id[id] == NULL) {
        return NULL;
    }

    return component_data_by_id[id];
}

const DtComponentData* component_get_data_by_name(const char* name) {
    int idx = component_get_hash(name) % COMPONENT_TABLE_SIZE;
    const int start = idx;

    while (component_data_by_name[idx] != NULL &&
           strcmp(component_data_by_name[idx]->name, name) != 0) {
        idx = (idx + 1) % COMPONENT_TABLE_SIZE;
        if (idx == start) {
            return NULL;
        }
           }

    if (component_data_by_name[idx] == NULL) {
        return NULL;
    }

    return component_data_by_name[idx];
}