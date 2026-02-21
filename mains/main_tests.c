#include "../Tests/TestEcs.h"


int main() {
    test_create_remove_entity();
    test_parent_children_relations();
    test_pools();
    test_filter();
    return 0;
}