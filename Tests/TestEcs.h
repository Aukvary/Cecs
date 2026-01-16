#ifndef ECS_MANAGER_TESTS_H
#define ECS_MANAGER_TESTS_H

#include "Ecs/ComponentsHandler.h"
#include "Ecs/DtEcs.h"

#define TEST_EMPTY_COMPONENT(X, name)

DT_DEFINE_EMPTY_COMPONENT(TestEmptyComponent1, TEST_EMPTY_COMPONENT)
DT_DEFINE_EMPTY_COMPONENT(TestEmptyComponent2, TEST_EMPTY_COMPONENT)

#define TEST_DATA_COMPONENT_1(X, name) X(int, data, name)

DT_DEFINE_COMPONENT(TestDataComponent1, TEST_DATA_COMPONENT_1)

#define TEST_DATA_COMPONENT_2(X, name) X(char*, data, name)

DT_DEFINE_COMPONENT(TestDataComponent2, TEST_DATA_COMPONENT_2)


void test_create_remove_entity(void);
void test_parent_children_relations(void);
void test_filter(void);

#endif
