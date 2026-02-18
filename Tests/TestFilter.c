#include <assert.h>
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
static void test_filter_2(void);

static DtEcsFilter* filter_test_1;

void test_filter(void) {
    manager = dt_ecs_manager_new(&cfg);

    e1 = dt_ecs_manager_new_entity(manager);
    e2 = dt_ecs_manager_new_entity(manager);
    e3 = dt_ecs_manager_new_entity(manager);

    test_filter_1();


    test_filter_2();

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

    DtEcsMask mask = dt_mask_new(manager, 1, 1);
    DT_MASK_INC(mask, TestEmptyComponent1);
    DT_MASK_INC(mask, TestEmptyComponent2);
    DT_MASK_INC(mask, TestDataComponent1);
    DT_MASK_EXC(mask, TestDataComponent2);

    filter_test_1 = dt_mask_end(mask);

    FOREACH(DtEntity, e, &filter_test_1->entities.iterator, { assert(e == e1); });
}

static void test_filter_2(void) {
    DT_ECS_MANAGER_REMOVE_FROM_POOL(manager, TestEmptyComponent1, e1);
    DT_ECS_MANAGER_ADD_TO_POOL(manager, TestEmptyComponent1, e2, NULL);
    DT_ECS_MANAGER_REMOVE_FROM_POOL(manager, TestDataComponent2, e3);

    DtEcsMask mask = dt_mask_new(manager, 4, 1);
    DT_MASK_INC(mask, TestEmptyComponent1);
    DT_MASK_INC(mask, TestEmptyComponent2);
    DT_MASK_INC(mask, TestDataComponent1);
    DT_MASK_EXC(mask, TestDataComponent2);

    DtEcsFilter* filter = dt_mask_end(mask);

    assert(filter == filter_test_1);

    printf("%d\n", dt_entity_container_has(&filter->entities, e2));


    FOREACH(DtEntity, e, &filter->entities.iterator, {
        //assert(e == e2 || e == e3);
        printf("%d", e);
    });
}
