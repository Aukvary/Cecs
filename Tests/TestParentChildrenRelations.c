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

void test_parent_children_relations(void) {
    manager = dt_ecs_manager_new(&cfg);

    DtEntity e1;
    DtEntity e2;
    DtEntity e3;

    DtEntityInfo info1;

    e1 = dt_ecs_manager_new_entity(manager);
    e2 = dt_ecs_manager_new_entity(manager);
    e3 = dt_ecs_manager_new_entity(manager);

    dt_ecs_manager_set_parent(manager, e2, e1);
    info1 = *dt_ecs_manager_get_entity(manager, e1);
    DtEntity children_count = info1.children_count;

    DtEntity child = info1.children[0]->id;
    DtEntity parent = dt_ecs_manager_get_parent(manager, e2)->id;

    assert(children_count == 1);
    assert(child == e2);
    assert(parent == e1);

    dt_ecs_manager_set_parent(manager, e2, DT_ENTITY_NULL);
    info1 = *dt_ecs_manager_get_entity(manager, e1);

    children_count = info1.children_count;
    parent = dt_ecs_manager_get_parent(manager, e2)->id;

    assert(children_count == 0);
    assert(parent == DT_ENTITY_NULL);

    dt_ecs_manager_set_parent(manager, e2, e1);
    dt_ecs_manager_set_parent(manager, e3, e1);

    info1 = *dt_ecs_manager_get_entity(manager, e1);
    children_count = info1.children_count;

    DtEntity child_1 = info1.children[0]->id;
    DtEntity child_2 = info1.children[1]->id;
    DtEntity parent_e2 = dt_ecs_manager_get_parent(manager, e2)->id;
    DtEntity parent_e3 = dt_ecs_manager_get_parent(manager, e2)->id;

    assert(children_count == 2);
    assert(child_1 == e2);
    assert(child_2 == e3);

    assert(parent_e2 == e1);
    assert(parent_e3 == e1);

    dt_ecs_manager_set_parent(manager, e2, DT_ENTITY_NULL);
    dt_ecs_manager_set_parent(manager, e3, DT_ENTITY_NULL);

    info1 = *dt_ecs_manager_get_entity(manager, e1);
    children_count = info1.children_count;

    parent_e2 = dt_ecs_manager_get_parent(manager, e2)->id;
    parent_e3 = dt_ecs_manager_get_parent(manager, e2)->id;

    assert(children_count == 0);

    assert(parent_e2 == DT_ENTITY_NULL);
    assert(parent_e3 == DT_ENTITY_NULL);

    dt_ecs_manager_free(manager);
}