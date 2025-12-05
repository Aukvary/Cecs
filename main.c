#include <stdio.h>
#include <stdlib.h>
#include "Ecs/Ecs.h"

typedef struct Position {
    float x, y;
} Position;

typedef struct Player {

} Player;

typedef struct Health {
    int value;
} Health;

typedef struct {
    EcsSystem system;
    EcsFilter* filter;
} PositionSystem;

void init(EcsManager* manager, void* data) {
    PositionSystem* system = data;

    EcsMask mask = GET_MASK(manager, Player);
    // MASK_INC(mask, Position);
    // MASK_INC(mask, Health);
    system->filter = mask_end(mask);
}

void update(void* data) {
    PositionSystem* system = data;

    FOREACH(Entity, e, system->filter->entities, { printf("%d ", e); });

    printf("was called\n");
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
    // Entity e2 = ecs_manager_new_entity(manager);
    // Entity e3 = ecs_manager_new_entity(manager);

    EcsPool* pool1 = ECS_MANAGER_GET_POOL(manager, Player);
    EcsPool* pool2 = ECS_MANAGER_GET_POOL(manager, Position);
    EcsPool* pool3 = ECS_MANAGER_GET_POOL(manager, Health);

    ECS_POOL_ADD(pool1, e1, (Player) {});
    // ECS_POOL_ADD(pool2, e1, ((Position) {1.0, 2.0}));
    // ECS_POOL_ADD(pool3, e1, (Health) {100});

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

    ecs_pool_remove(pool1, e1);

    system_handler_update(handler);

    ecs_manager_free(manager);

    return 0;
}
