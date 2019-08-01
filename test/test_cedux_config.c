#include "unity.h"
#include "mock_cedux_test_helpers.h"

// Define max values _before_ including cedux. Otherwise, you need to #undef and #define the values.
#define CEDUX_MAX_ACTIONS 256 
#define CEDUX_MAX_REDUCERS 128 
#define CEDUX_MAX_SUBSCRIBERS 256

#include "cedux.h"

CEDUX_DECLARE_STORE(int, int, int_store);
CEDUX_DEFINE_STORE(int, int, int_store);

void test_cedux_can_configure_max_actions(void) {

    //  Initialize a store
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;

    // Setup an expectation that the reducer will be called CEDUX_MAX_ACTIONS times
    for (int i = 0; i < CEDUX_MAX_ACTIONS; i++) {
        mockable_reducer1_ExpectAndReturn(&store.tree, i, true);
    }

    // Register the reducer
    cedux_register_int_store_reducer(&store, mockable_reducer1);

    // Dispatch CEDUX_MAX_ACTIONS. This should result in the reducer being called that many times.
    for (int i = 0; i < CEDUX_MAX_ACTIONS; i++) {
        cedux_dispatch_int_store(&store, i);
    }

    // Run the store
    cedux_run_int_store(&store);
}

void test_cedux_can_configure_max_reducers(void) {

    //  Initialize a store
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;

    // Setup an expectation that the reducer will be called CEDUX_MAX_REDUCERS times
    for (int i = 0; i < CEDUX_MAX_REDUCERS; i++) {
        mockable_reducer1_ExpectAndReturn(&store.tree, 1, true);
    }

    // Register the same reducer CEDUX_MAX_REDUCERS times 
    for (int i = 0; i < CEDUX_MAX_REDUCERS; i++) {
        cedux_register_int_store_reducer(&store, mockable_reducer1);
    }

    // Dispatch 1 action. This should result in the reducer being called CEDUX_MAX_REDUCERS times.
    cedux_dispatch_int_store(&store, 1);

    // Run the store
    cedux_run_int_store(&store);
}

void test_cedux_can_configure_max_subscribers(void) {

    //  Initialize a store
    struct int_store_handle store = cedux_init_int_store();
    store.tree = 0;

    // Expect one reducer to be called
    mockable_reducer1_ExpectAndReturn(&store.tree, 1, true);

    // Expect max subscrber calls
    for (int i = 0; i < CEDUX_MAX_SUBSCRIBERS; i++) {
        mockable_subscriber1_Expect(&store, &store.tree, &i);
    }

    // Register the reducer 
    cedux_register_int_store_reducer(&store, mockable_reducer1);

    // Register max number of subscribers
    for (int i = 0; i < CEDUX_MAX_SUBSCRIBERS; i++) {
        cedux_register_int_store_subscriber(&store, (void (*)(struct int_store_handle *, const int * const, void *))mockable_subscriber1, &i);
    }

    // Dispatch 1 action. This should result in the subscriber being called CEDUX_MAX_SUBSCRIBERS times.
    cedux_dispatch_int_store(&store, 1);

    // Run the store
    cedux_run_int_store(&store);
}