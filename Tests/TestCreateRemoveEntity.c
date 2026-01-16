#include "TestEcs.h"
#include <assert.h>
#include <stdio.h>

static DtEcsManager* manager;

static const DtEcsManagerConfig cfg = {
    .dense_size = 1,
    .sparce_size = 1,
    .recycle_size = 0,
    .components_count = 3,
    .pools_size = 3,
    .masks_size = 1,

    .children_size = 3,

    .include_mask_count = 2,
    .exclude_mask_count = 0,
    .filters_size = 1,
};

static void test_create_remove_entity_1(DtEntity* entity, DtEntityInfo* info);
static void test_create_remove_entity_add_to_pools(DtEntity* entity, DtEntityInfo* info,
                                                   DtEcsPool* empty_pool,
                                                   DtEcsPool* data_pool);
static void test_create_remove_entity_2(DtEntity* entity1, DtEntityInfo* info1,
                                        DtEntity* entity2, DtEntityInfo* info2);
static void test_create_remove_entity_set_parent(DtEntity entity1, DtEntity entity2);

void test_create_remove_entity(void) {
    printf("\n==test_create_remove_entity===\n");

    manager = dt_ecs_manager_new(&cfg);
    assert(manager != NULL && "Manager should be created");

    DtEcsPool* empty_1_pool = DT_ECS_MANAGER_GET_POOL(manager, TestEmptyComponent1);
    DtEcsPool* data_1_pool = DT_ECS_MANAGER_GET_POOL(manager, TestDataComponent1);

    DtEntity entity1;
    DtEntityInfo info1;

    DtEntity entity2;
    DtEntityInfo info2;

    DtEntity recycled;
    DtEntityInfo recycled_info;

    test_create_remove_entity_1(&entity1, &info1);
    test_create_remove_entity_add_to_pools(&entity1, &info1, empty_1_pool, data_1_pool);
    test_create_remove_entity_2(&entity1, &info1, &entity2, &info2);
    test_create_remove_entity_set_parent(entity1, entity2);

    dt_ecs_manager_kill_entity(manager, entity1);
    info1 = dt_ecs_manager_get_entity(manager, entity1);
    DtEntity parent = dt_ecs_manager_get_parent(manager, entity2);

    assert(info1.gen == -1 && "Killed entity should have gen < 0");
    assert(info1.component_count == 0 && "Killed entity should have no components");
    assert(!dt_ecs_pool_has(empty_1_pool, entity1) &&
           "Killed entity should be removed from empty_1_pool");
    assert(!dt_ecs_pool_has(data_1_pool, entity1) &&
           "Killed entity should be removed from data_1_pool");

    assert(parent == DT_ENTITY_NULL &&
           "entity2 have parent yet");

    recycled = dt_ecs_manager_new_entity(manager);
    recycled_info = dt_ecs_manager_get_entity(manager, recycled);

    assert(recycled == 0 && "Recycled entity should reuse id 0");
    assert(recycled_info.gen == 2 &&
           "Recycled entity should have incremented generation (2)");

    dt_ecs_manager_free(manager);
    printf("===SUCCESS===\n\n");
}

static void test_create_remove_entity_1(DtEntity* entity, DtEntityInfo* info) {
    *entity = dt_ecs_manager_new_entity(manager);
    *info = dt_ecs_manager_get_entity(manager, *entity);

    assert(info->id == 0 && "First entity should have id 0");
    assert(info->gen == 1 && "Generation should start from 1");
    assert(info->component_count == 0 && "New entity should have no components");
}

static void test_create_remove_entity_add_to_pools(DtEntity* entity, DtEntityInfo* info,
                                                   DtEcsPool* empty_pool,
                                                   DtEcsPool* data_pool) {
    dt_ecs_pool_add(empty_pool, *entity, NULL);
    dt_ecs_pool_add(data_pool, *entity, &(TestDataComponent1) {10});

    EcsMask mask = dt_mask_new(manager, 1, 0);
    DT_MASK_INC(mask, TestEmptyComponent1);
    const DtEcsFilter* filter = dt_mask_end(mask);

    *info = dt_ecs_manager_get_entity(manager, *entity);
    assert(info->component_count == 2 && "Entity should have 2 components after adding");
    assert(dt_ecs_pool_has(empty_pool, *entity) &&
           "Killed entity should add to empty_1_pool");
    assert(dt_ecs_pool_has(data_pool, *entity) && "Killed entity should add to data_1_pool");
    assert(dt_entity_container_has(&filter->entities, *entity) &&
           "Killed entity should add to data_1_pool");
}

static void test_create_remove_entity_2(DtEntity* entity1, DtEntityInfo* info1,
                                        DtEntity* entity2, DtEntityInfo* info2) {
    *entity2 = dt_ecs_manager_new_entity(manager);
    *info2 = dt_ecs_manager_get_entity(manager, *entity2);

    assert(info2->id == 1 && "Second entity should have id 1");
    assert(*entity1 != *entity2 && "Entities should have different ids");
}

static void test_create_remove_entity_set_parent(DtEntity entity1, DtEntity entity2) {
    dt_ecs_manager_set_parent(manager, entity1, entity2);
    DtEntity parent = dt_ecs_manager_get_parent(manager, entity2);
}