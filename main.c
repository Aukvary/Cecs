#include <stdio.h>
#include <stdlib.h>
#include "Ecs/Ecs.h"

typedef struct Component1 { } Component1;
typedef struct Component2 { } Component2;
typedef struct Component3 { } Component3;

typedef struct {
    EcsSystem system;
    EcsPool* pool;
    EcsFilter* filter;
} PositionSystem;

void init(const EcsManager* manager, void* data) {
    PositionSystem* system = data;

    EcsMask mask = GET_MASK(manager, Component1);
    MASK_INC(mask, Component2);
    MASK_EXC(mask, Component3);
    system->filter = mask_end(mask);
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

    Entity e1 = ecs_manager_new_entity(manager);
    Entity e2 = ecs_manager_new_entity(manager);
    Entity e3 = ecs_manager_new_entity(manager);

    EcsPool* pool1 = ECS_MANAGER_GET_POOL(manager, Component1);
    EcsPool* pool2 = ECS_MANAGER_GET_POOL(manager, Component2);
    EcsPool* pool3 = ECS_MANAGER_GET_POOL(manager, Component3);

    ecs_pool_add(pool1, e1, NULL);
    ecs_pool_add(pool2, e1, NULL);

    ecs_pool_add(pool1, e2, NULL);

    ecs_pool_add(pool1, e3, NULL);
    ecs_pool_add(pool2, e3, NULL);
    ecs_pool_add(pool3, e3, NULL);

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

    system_handler_update(handler);

    ecs_manager_free(manager);

    return 0;
}
