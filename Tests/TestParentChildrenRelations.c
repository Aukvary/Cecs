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
static DtEntityInfo info1;

static DtEntity e2;
static DtEntityInfo info2;

static DtEntity e3;
static DtEntityInfo info2;

static void test_parent_children_relations_1();

void test_parent_children_relations(void) {
    manager = dt_ecs_manager_new(&cfg);

    e1 = dt_ecs_manager_new_entity(manager);
    e2 = dt_ecs_manager_new_entity(manager);
    e3 = dt_ecs_manager_new_entity(manager);

    test_parent_children_relations_1();
}

static void test_parent_children_relations_1() {
    dt_ecs_manager_set_parent(manager, e2, e1);
    info1 = dt_ecs_manager_get_entity(manager, e1);
    DtEntity children_count = info1.children_count;
    info2 = dt_ecs_manager_get_entity(manager, e2);

    DtEntity child = info1.children[0]->id;
    DtEntity parent = dt_ecs_manager_get_parent(manager, e2);

    assert(children_count == 1);
    assert(child == e2);
    assert(parent == e1);

    dt_ecs_manager_set_parent(manager, e2, DT_ENTITY_NULL);
    info1 = dt_ecs_manager_get_entity(manager, e1);
    info2 = dt_ecs_manager_get_entity(manager, e2);

    parent = dt_ecs_manager_get_parent(manager, e2);

    assert(children_count == 0);
    assert(parent == ENTITY_NULL);
}