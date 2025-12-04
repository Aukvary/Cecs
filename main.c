#include <stdio.h>
#include <stdlib.h>
#include "Ecs/Ecs.h"

typedef struct Position Position;

typedef struct {
    EcsSystem system;
    EcsPool* pool;
    EcsFilter* filter;
} PositionSystem;

void init(const EcsManager* manager, void* data) {
    PositionSystem* system = data;

    EcsMask mask = GET_MASK(manager, Position);
    system->filter = mask_end(mask);

    system->pool = GET_TAG_POOL(manager, Position);
}

void update(void* data) {
    PositionSystem* system = data;

    FOREACH(Entity, e, system->filter->entities, { printf("%d ", e); });
}

EcsSystem* new_position_system() {
    EcsSystem* system = malloc(sizeof(EcsSystem));

    *system = (EcsSystem) {
        .data = malloc(sizeof(PositionSystem)),

        .init = init,
        .update = update,

        .destroy = NULL,
        .pre_update = NULL,
        .post_update = NULL,
    };

    return system;
}

int main() {
    const EcsConfig config = {
        .dense_size = 50,
        .sparce_size = 500,
        .recycle_size = 50,
        .components_count = 5,
        .pools_size = 25,
        .masks_size = 10,
        .include_mask_count = 5,
        .exclude_mask_count = 5,
        .filters_size = 10,
    };

    EcsManager* manager = ecs_manager_new(&config);

    Entity e = ecs_manager_new_entity(manager);

    EcsPool* pool = GET_TAG_POOL(manager, Position);

    ecs_pool_add_item(pool, e, NULL);

    printf("%d\n", ecs_pool_has(pool, e));

    SystemHandlerConfig sys_cfg = (SystemHandlerConfig) {
        .init_size = 1,
        .pre_update_size = 0,
        .update_size = 1,
        .post_update_size = 0,
        .destroy_size = 0,
    };

    SystemHandler* handler = system_handler_new(manager, &sys_cfg);
    system_handler_add(handler, new_position_system());


    system_handler_init(handler);

    for (int i = 0; i < 10; i++) {
        system_handler_update(handler);
    }

    ecs_manager_free(manager);

    return 0;
}
