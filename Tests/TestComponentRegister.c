#include <stddef.h>
#include "Ecs/RegisterHandler.h"
#include "TestEcs.h"

DT_REGISTER_COMPONENT(TestEmptyComponent1, TEST_EMPTY_COMPONENT);
DT_REGISTER_COMPONENT(TestEmptyComponent2, TEST_EMPTY_COMPONENT);
DT_REGISTER_COMPONENT(TestDataComponent1, TEST_DATA_COMPONENT_1);
DT_REGISTER_COMPONENT(TestDataComponent2, TEST_DATA_COMPONENT_2);

static void test_update_system_init(DtEcsManager* manager, void* data);
static void test_update_system_update(void* data, DtUpdateContext* ctx);
static void test_update_system_destroy(void* data);

static UpdateSystem test_update_template = (UpdateSystem) {
    .init = test_update_system_init,
    .update = test_update_system_update,
    .destroy = test_update_system_destroy,
    .priority = 0
};

UpdateSystem* test_update_system_new() {
    TestUpdate1* update = DT_MALLOC(sizeof(TestUpdate1));

    *update = (TestUpdate1) {
        .system = test_update_template
    };

    update->system.data = update;

    return &update->system;
}

static void test_update_system_init(DtEcsManager* manager, void* data) {
    TestUpdate1* update = data;

    update->tag1_pool = DT_ECS_MANAGER_GET_POOL(manager, TestEmptyComponent1);
    update->data1_pool = DT_ECS_MANAGER_GET_POOL(manager, TestDataComponent1);

    DtEcsMask mask = dt_mask_new(manager, 2, 0);
    DT_MASK_INC(mask, TestEmptyComponent1);
    DT_MASK_INC(mask, TestDataComponent1);

    update->filter = dt_mask_end(mask);
}

static void test_update_system_update(void* data, DtUpdateContext* ctx) {
    TestUpdate1* update = data;

    FOREACH(DtEntity, e, &update->filter->entities.entities_iterator, {

    });
}

static void test_update_system_destroy(void* data) {}

DT_REGISTER_UPDATE(TestUpdate1, test_update_system_new, test_update_template);


static void test_draw_system_init(DtEcsManager* manager, void* data);
static void test_draw_system_draw(void* data);
static void test_draw_system_destroy(void* data);

static DrawSystem test_draw_template = (DrawSystem) {
    .init = test_draw_system_init,
    .draw = test_draw_system_draw,
    .destroy = test_draw_system_destroy,
    .priority = 0
};

DrawSystem* test_draw_system_new() {
    TestDraw1* draw = DT_MALLOC(sizeof(TestDraw1));

    *draw = (TestDraw1) {
        .system = test_draw_template,
    };

    draw->system.data = draw;

    return &draw->system;
}

static void test_draw_system_init(DtEcsManager* manager, void* data) {
    TestDraw1* draw = data;

    draw->tag2_pool = DT_ECS_MANAGER_GET_POOL(manager, TestEmptyComponent2);
    draw->data2_pool = DT_ECS_MANAGER_GET_POOL(manager, TestDataComponent2);

    DtEcsMask mask = dt_mask_new(manager, 2, 0);
    DT_MASK_INC(mask, TestEmptyComponent2);
    DT_MASK_INC(mask, TestDataComponent2);

    draw->filter = dt_mask_end(mask);
}

static void test_draw_system_draw(void* data) {
    TestDraw1* draw = data;

    FOREACH(DtEntity, e, &draw->filter->entities.entities_iterator, {

    });
}

static void test_draw_system_destroy(void* data) {}

DT_REGISTER_DRAW(TestDraw1, test_draw_system_new, test_draw_template);