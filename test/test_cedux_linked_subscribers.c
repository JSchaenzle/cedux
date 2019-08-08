#include "unity.h"
#include "cedux.h"
#include "mock_cedux_test_helpers.h"

CEDUX_DECLARE_STORE(int, int, int_store);
CEDUX_DEFINE_STORE(int, int, int_store);

bool reducer1(int * p_state, int action) {
    if (action == 1) {
        *p_state += 100;
        return true;
    }
    return false;
}

bool reducer2(int * p_state, int action) {
    if (action == 2) {
        *p_state += 200;
        return true;
    }
    return false;
}

void test_cedux_linked_subscriber_will_run_only_when_corresponding_reducer_runs(void) {
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;

    cedux_register_int_store_reducer(&store, reducer1);
    cedux_register_int_store_reducer(&store, reducer2);
    cedux_register_int_store_linked_subscriber(&store, (void (*)(struct int_store_handle *, const int * const, void *))mockable_subscriber1, 
                                                NULL, reducer1);
    cedux_register_int_store_linked_subscriber(&store, (void (*)(struct int_store_handle *, const int * const, void *))mockable_subscriber2, 
                                                NULL, reducer2);

    // Expect only subscriber1 to be called
    mockable_subscriber1_Expect(&store, &store.tree, NULL);

    cedux_dispatch_int_store(&store, 1); // Dispatch 1 which will result in reducer1 running
    cedux_run_int_store(&store);

    TEST_ASSERT_EQUAL(100, store.tree);
}