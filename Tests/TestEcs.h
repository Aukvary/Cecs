#ifndef ECS_MANAGER_TESTS_H
#define ECS_MANAGER_TESTS_H

#include "DtAllocators.h"
#include "Ecs/DtEcs.h"
#include "Ecs/RegisterHandler.h"

#define TEST_DATA_COMPONENT_1(X, name) X(int, data, name)
DT_DEFINE_COMPONENT(TestDataComponent1, TEST_DATA_COMPONENT_1);

#define TEST_DATA_COMPONENT_2(X, name) X(char*, data, name)
DT_DEFINE_COMPONENT(TestDataComponent2, TEST_DATA_COMPONENT_2)

typedef struct {
    UpdateSystem system;
    DtEcsFilter* filter;

    DtEcsPool* data1_pool;
    DtEcsPool* tag1_pool;
} TestUpdate1;

typedef struct {
    DrawSystem system;
    DtEcsFilter* filter;

    DtEcsPool* data2_pool;
    DtEcsPool* tag2_pool;
} TestDraw1;

void test_create_remove_entity(void);
void test_parent_children_relations(void);
void test_pools(void);
void test_filter(void);

#endif
