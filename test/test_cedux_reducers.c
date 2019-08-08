#include "unity.h"
#include "cedux.h"

CEDUX_DECLARE_STORE(int, int, int_store);
CEDUX_DEFINE_STORE(int, int, int_store);

void setUp(void) {

}

void tearDown(void) {

}

bool reducer1(int * p_state, int action) {
    if (action == 1) {
        *p_state += 100;
        return true;
    }
    return false;
}

void test_cedux_reducer_will_run_and_can_update_state(void) {
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;
    cedux_register_int_store_reducer(&store, reducer1);
    cedux_dispatch_int_store(&store, 1);
    cedux_run_int_store(&store);

    TEST_ASSERT_EQUAL(100, store.tree);
}

void test_cedux_reducer_will_will_only_be_called_once_for_a_given_dispatched_action(void) {
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;
    cedux_register_int_store_reducer(&store, reducer1);
    cedux_dispatch_int_store(&store, 1);

    // Call run_store several times to ensure that the reducer doesn't get called many times.
    bool did_work = cedux_run_int_store(&store);
    TEST_ASSERT_TRUE(did_work);
    did_work = cedux_run_int_store(&store);
    TEST_ASSERT_FALSE(did_work);
    did_work = cedux_run_int_store(&store);
    TEST_ASSERT_FALSE(did_work);
    did_work = cedux_run_int_store(&store);
    TEST_ASSERT_FALSE(did_work);

    TEST_ASSERT_EQUAL(100, store.tree);
}