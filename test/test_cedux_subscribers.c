#include "unity.h"
#include "cedux.h"
#include "mock_cedux_test_helpers.h"

CEDUX_DECLARE_STORE(int, int, int_store);
CEDUX_DEFINE_STORE(int, int, int_store);

bool reducer1(int * p_state, int action) {
    if (action == 1) {
        *p_state = 1;
        return true;
    }
    return false;
}

bool reducer2(int * p_state, int action) {
    if (action == 2) {
        *p_state = 2;
        return true;
    }
    return false;
}

void subscriber1(struct int_store_handle * p_store, int const * const state, void *data) {
  if(*state == 1) {
    cedux_dispatch_int_store(p_store, 2);
  }
}

void test_cedux_subscriber_dispatches_actions_that_will_be_run(void) {
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;

    cedux_register_int_store_reducer(&store, reducer1);
    cedux_register_int_store_reducer(&store, reducer2);
    cedux_register_int_store_subscriber(&store, subscriber1, NULL);


    cedux_dispatch_int_store(&store, 1); // Dispatch 1 which will result in reducer1 running
    cedux_run_int_store(&store);

    TEST_ASSERT_EQUAL(2, store.tree);
}