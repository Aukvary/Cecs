#include "../GameComponents.h"
#include "Ecs/RegisterHandler.h"

void collider_grid_reset(void* data) {
    ColliderGrid* grid = data;

    grid->show = false;
    grid->cell_count = (Vector2) {100, 100};
    grid->cell_size = 20;
    grid->grid_color = GREEN;
}

DT_REGISTER_COMPONENT(ColliderGrid, COLLIDER_GRID, DT_INIT_ATTR(collider_grid_reset))
