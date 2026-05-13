#include "DtAllocators.h"
#include "DtComponents/Components.h"
#include "GameComponents.h"

static DrawSystem* collider_grid_new();
static void collider_grind_init(DtEcsManager* manager, void* data);
static void collider_grid_draw(void* data);
DT_REGISTER_DRAW(ColliderDrawSystem, collider_grid_new, DTE_EDIT_DRAW(collider_grid_new))

typedef struct {
    DrawSystem system;

    DtEcsPool* transforms;
    DtEcsPool* grids;
} ColliderDrawSystem;

static DrawSystem* collider_grid_new() {
    ColliderDrawSystem* sys = DT_MALLOC(sizeof(ColliderDrawSystem));

    *sys = (ColliderDrawSystem) {
        .system =
            (DrawSystem) {
                .data = sys,
                .init = collider_grind_init,
                .draw = collider_grid_draw,
                .destroy = NULL,
            },
    };

    return &sys->system;
}

static void collider_grind_init(DtEcsManager* manager, void* data) {
    ColliderDrawSystem* sys = data;

    sys->transforms = DT_ECS_MANAGER_GET_POOL(manager, DtTransform2D);
    sys->grids = DT_ECS_MANAGER_GET_POOL(manager, ColliderGrid);
}

static void collider_grid_draw(void* data) {
    ColliderDrawSystem* sys = data;

    FOREACH(DtEntity, e, &sys->grids->iterator, ({
                DtTransform2D* transform = dt_ecs_pool_get(sys->transforms, e);
                ColliderGrid* grid = dt_ecs_pool_get(sys->grids, e);

                if (!grid->show)
                    continue;

                for (int i = 0; i <= grid->cell_count.x; i++) {
                    Vector2 start = {i * grid->cell_size, 0.0f};
                    Vector2 end = {i * grid->cell_size, grid->cell_count.y * grid->cell_size};
                    DrawLineV(start, end, grid->grid_color);
                }

                for (int i = 0; i <= grid->cell_count.y; i++) {
                    Vector2 start = {0.0f, i * grid->cell_size};
                    Vector2 end = {grid->cell_count.x * grid->cell_size, i * grid->cell_size};
                    DrawLineV(start, end, grid->grid_color);
                }
            }));
}
