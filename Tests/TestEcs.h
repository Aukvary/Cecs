#ifndef ECS_MANAGER_TESTS_H
#define ECS_MANAGER_TESTS_H

#include "Ecs/DtEcs.h"

struct TestEmptyComponent1 {};

struct TestEmptyComponent2 {};

struct TestDataComponent1 {
    int data;
};

struct TestDataComponent2 {
    char* data;
};

REGISTER_COMPONENT(TestEmptyComponent1);
REGISTER_COMPONENT(TestEmptyComponent2);
REGISTER_COMPONENT(TestDataComponent1);
REGISTER_COMPONENT(TestDataComponent2);

void test_create_remove_entity(void);
void test_parent_children_relations(void);

#endif
