#include "TestEcs.h"

static DtEcsManager* manager;

static const DtEcsManagerConfig cfg = {
    .dense_size = 3,
    .sparce_size = 3,
    .recycle_size = 3,
    .components_count = 0,
    .pools_size = 1,
    .masks_size = 0,

    .children_size = 1,

    .include_mask_count = 0,
    .exclude_mask_count = 0,
    .filters_size = 0,
};

void test_filter(void) {
    manager = dt_ecs_manager_new(&cfg);
}