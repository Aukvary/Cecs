#include <stdio.h>
#include <stdlib.h>


#include "Ecs/EcsManager/EcsManager.h"
#include "Ecs/Systems/Systems.h"

typedef struct {
    float x, y;
} Position;

typedef struct {
    EcsSystem system;
    EcsPool* pool;
    EcsFilter* filter;
} PositionSystem;

void init(EcsManager* manager, void* data) {
    PositionSystem* system = data;

    EcsMask mask = GET_MASK(manager, Position);
    system->filter = mask_end(mask);

    system->pool = GET_POOL(manager, Position);
}

void update(void* data) {
    PositionSystem* system = data;

    FOREACH(Entity, e, system->filter->entities, {
        Position* pos = ecs_pool_get_item(system->pool, e);

        pos->x += 0.006;
    });
}

UpdateSystem* new_position_system() {
    UpdateSystem* system = malloc(sizeof(EcsSystem));

    return system;
}

int main() {
    EcsConfig config = {
        .dense_size = 50,
        .sparce_size = 500,
        .recycle_size = 50,
        .components_count = 5,
        .pools_size = 25,
        .masks_size = 10,
        .include_mask_count = 5,
        .exclude_mask_count = 5,
        .filters_size = 10
    };

    EcsManager* manager = ecs_manager_new(&config);

    Entity e = ecs_manager_new_entity(manager);

    EcsPool* pool = GET_POOL(manager, Position);

    printf("%zu\n", pool->data.id);

    ecs_pool_add_item(pool, e, NULL);




    return 0;
}
