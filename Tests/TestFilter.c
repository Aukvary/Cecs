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

static DtEntity e1;
static DtEntity e2;
static DtEntity e3;

static void test_filter_1(void);

void test_filter(void) {
    manager = dt_ecs_manager_new(&cfg);

    e1 = dt_ecs_manager_new_entity(manager);
    e2 = dt_ecs_manager_new_entity(manager);
    e3 = dt_ecs_manager_new_entity(manager);

    test_filter_1();

    dt_ecs_manager_free(manager);
}

static void test_filter_1(void) {
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent1, e1, NULL);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent2, e1, NULL);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestDataComponent1, e1, NULL);

    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent1, e2, NULL);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent2, e2, NULL);

    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent1, e3, NULL);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent2, e3, NULL);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestDataComponent1, e3, NULL);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestDataComponent2, e3, NULL);
}