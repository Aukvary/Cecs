#include <assert.h>
#include <string.h>
#include "TestEcs.h"

const DtComponentData** components;
DtComponentData func_components[5];
u16 count;

DtComponentData TestEmptyComponent1_data();
DtComponentData TestEmptyComponent2_data();
DtComponentData TestDataComponent1_data();
DtComponentData TestDataComponent2_data();

void test_component_register_1(void);
void test_component_register_2(void);
void test_component_register_3(void);

void test_component_register(void) {
    printf("\n\t===test_component_register===\n");

    components = dt_component_get_all(&count);

    printf("\n\t\t===test 1 start===\n");
    test_component_register_1();
    printf("\t\t===test 1 success===\n");

    printf("\n\t\t===test 2 start===\n");
    test_component_register_2();
    printf("\t\t===test 2 success===\n");

    printf("\n\t\t===test 3 start===\n");
    test_component_register_3();
    printf("\t\t===test 3 success===\n");

    printf("\n\t\t===SUCCESS===\n\n");
}

void test_component_register_1(void) {
    func_components[0] = TestEmptyComponent1_data();
    assert(func_components[0].name != NULL);

    func_components[1] = TestEmptyComponent2_data();
    assert(func_components[1].name != NULL);

    func_components[2] = TestDataComponent1_data();
    assert(func_components[2].name != NULL);

    func_components[3] = TestDataComponent2_data();
    assert(func_components[3].name != NULL);
}

void test_component_register_2(void) {
    for (int i = 0; i < 4; i++) {
        bool found = false;
        for (int j = 0; j < count; j++) {
            if (strcmp(func_components[i].name, components[j]->name) != 0) continue;
            found = true;
        }

        assert(found && func_components[i].name);
    }
}

void test_component_register_3(void) {
    assert(TestDataComponent1_data().attribute_count == 1);
    assert(strcmp(TestDataComponent1_data().attributes[0].attribute_name, "test_attr") == 0);
    assert(((TestAttribute*)TestDataComponent1_data().attributes[0].data)->test_value == 10);
}