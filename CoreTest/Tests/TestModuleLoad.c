#include <assert.h>


#include "TestEcs.h"
#include "scheduler/RuntimeScheduler.h"

static ModuleInfo* info;

void test_module_load1(void);
void test_module_load2(void);
void test_module_load3(void);
void test_module_load4(void);
void test_module_load5(void);
void test_module_load6(void);

void test_module_load(void) {
    printf("\n\t===test_component_register===\n");

    info = dt_module_load(dt_environment_instance(), DT_LIB_NAME("./libModuleTest"));

    printf("\n\t\t===test 1 start===\n");
    test_module_load1();
    printf("\t\t===test 1 success===\n");

    printf("\n\t\t===test 2 start===\n");
    test_module_load2();
    printf("\t\t===test 2 success===\n");

    printf("\n\t\t===test 3 start===\n");
    test_module_load3();
    printf("\t\t===test 3 success===\n");

    printf("\n\t\t===test 4 start===\n");
    test_module_load4();
    printf("\t\t===test 4 success===\n");

    printf("\n\t\t===test 5 start===\n");
    test_module_load5();
    printf("\t\t===test 5 success===\n");

    printf("\n\t\t===SUCCESS===\n\n");

    dt_module_unload(dt_environment_instance(), info);
}

void test_module_load1(void) {
    assert(info != NULL);
    assert(info->environment != NULL);
}

void test_module_load2(void) {
    const DtComponentData** data = info->environment->components;
    assert(data != NULL);

    u16 count = info->environment->components_count;
    assert(count == 3);

    for (u16 i = 0; i < count; i++) {
        if (strcmp(data[i]->name, "HierarchyDirty") == 0) {
            continue;
        }

        if (strcmp(data[i]->name, "ModuleTestTag") == 0) {
            continue;
        }

        if (strcmp(data[i]->name, "ModuleTestComponent") == 0) {
            continue;
        }

        assert(0);
    }
}

void test_module_load3(void) {
    const DtComponentData* data = info->environment->get_component("ModuleTestComponent");
    assert(data != NULL);
    assert(strcmp(data->name, "ModuleTestComponent") == 0);
    assert(data->attribute_count == 1);
    assert(strcmp(data->attributes[0].attribute_name, "TestComponent") == 0);
    assert(strcmp(data->attributes[0].data, "test") );

    assert(data->field_count == 1);
    assert(data->filed_attributes_count[0] == 1);
    assert(strcmp(data->filed_attributes[0][0].attribute_name, "module_test_attr") == 0);
    assert(*(int*)data->filed_attributes[0][0].data == 10);
}

void test_module_load4(void) {
    const DtUpdateData** data = info->environment->updaters;
    assert(data != NULL);
    assert(strcmp(data[0]->name, "TestUpdate1") == 0);
    assert(info->environment->get_update("TestUpdate1") == data[0]);

    u16 count = info->environment->updaters_count;
    assert(count == 1);

    UpdateSystem* s = data[0]->new();
    assert(s != NULL);
    assert(s->init != NULL);
    assert(s->update != NULL);
    assert(s->destroy != NULL);
    assert(s->priority == 0);

    assert(strcmp(data[0]->attributes[0].attribute_name, "update_attribute") == 0);
}

void test_module_load5(void) {
    const DtDrawData** data = info->environment->drawers;
    assert(data != NULL);
    assert(strcmp(data[0]->name, "TestDraw1") == 0);
    assert(info->environment->get_draw("TestDraw1") == data[0]);

    u16 count = info->environment->drawers_count;
    assert(count == 1);

    DrawSystem* s = data[0]->new();
    assert(s != NULL);
    assert(s->init != NULL);
    assert(s->draw != NULL);
    assert(s->destroy != NULL);
    assert(s->priority == 0);

    assert(strcmp(data[0]->attributes[0].attribute_name, "draw_attribute") == 0);
}