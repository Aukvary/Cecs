#include <assert.h>
#include <string.h>


#include "TestEcs.h"
#include "scheduler/RuntimeScheduler.h"

#define SCENE_PATH "./test_scene_1.dt.scene"
#define SCENE_NAME "test_scene_1"

static void test_scene_parse1(void);
static void test_scene_parse2(void);
static void test_scene_parse3(void);

static const DtScene* scene;

void test_scene_parse(void) {
    printf("\n\t===test_scene_parse===\n");

    printf("\n\t\t===test 1 start===\n");
    test_scene_parse1();
    printf("\t\t===test 1 success===\n");

    printf("\n\t\t===test 2 start===\n");
    test_scene_parse2();
    printf("\t\t===test 2 success===\n");

    printf("\n\t\t===test 3 start===\n");
    test_scene_parse3();
    printf("\t\t===test 3 success===\n");

    printf("\n\t\t===SUCCESS===\n\n");
}

static void test_scene_parse1(void) {
    dt_add_scene(SCENE_PATH);
    dt_scenes_set_active(SCENE_NAME);
    scene = dt_scenes_get_active();
    assert(scene != NULL);
}

static void test_scene_parse2(void) {
    DtEcsPool* tag_pool1 = DT_ECS_MANAGER_GET_POOL(scene->manager, TestEmptyComponent1);
    DtEcsPool* tag_pool2 = DT_ECS_MANAGER_GET_POOL(scene->manager, TestEmptyComponent2);

    DtEcsPool* data_pool1 = DT_ECS_MANAGER_GET_POOL(scene->manager, TestDataComponent1);
    DtEcsPool* data_pool2 = DT_ECS_MANAGER_GET_POOL(scene->manager, TestDataComponent2);

    assert(((TestDataComponent1*)dt_ecs_pool_get(data_pool1, 0))->data == 10);
    assert(dt_ecs_pool_has(tag_pool1, 0));

    assert(strcmp(((TestDataComponent2*)dt_ecs_pool_get(data_pool2, 1))->data, "text data") == 0);
    assert(dt_ecs_pool_has(tag_pool1, 1));
    assert(dt_ecs_pool_has(tag_pool2, 1));

    assert(dt_ecs_pool_has(tag_pool1, 2));
    assert(dt_ecs_pool_has(tag_pool2, 2));
}

static void test_scene_parse3(void) {
    assert(dt_ecs_manager_get_entity(scene->manager, 0).parent == 1);
    assert(dt_ecs_manager_get_entity(scene->manager, 1).parent == DT_ENTITY_NULL);
    assert(dt_ecs_manager_get_entity(scene->manager, 2).parent == 0);
    assert(dt_ecs_manager_get_entity(scene->manager, 3).parent == 0);
}
