#ifndef CEDUX_H
#define CEDUX_H
#include <stddef.h>
#include "queue.h"


#define CEDUX_DEFINE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)                                  \
                                                                                                \
QUEUE_DECLARATION(STORE_NAME##_action_queue, ACTION_TYPE, 10)                                   \
QUEUE_DEFINITION(STORE_NAME##_action_queue, ACTION_TYPE)                                        \
                                                                                                \
struct STORE_NAME##_handle;                                                                     \
typedef void(*STORE_NAME##_REDUCER)(TREE_TYPE * p_tree, ACTION_TYPE action);                    \
                                                                                                \
QUEUE_DECLARATION(STORE_NAME##_reducer_queue, STORE_NAME##_REDUCER, 10)                         \
QUEUE_DEFINITION(STORE_NAME##_reducer_queue, STORE_NAME##_REDUCER)                              \
struct STORE_NAME##_handle                                                                      \
{                                                                                               \
  TREE_TYPE tree;                                                                               \
  struct STORE_NAME##_action_queue action_queue;                                                \
  struct STORE_NAME##_reducer_queue reducer_queue;                                              \
};                                                                                              \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store, STORE_NAME##_REDUCER reducer) {  \
                                                                                                \
}                                                                                               \
                                                                                                \
struct STORE_NAME##_handle store;                                                               \
                                                                                                \
void cedux_init(struct STORE_NAME##_handle * p_store) {                                         \                                    \
  STORE_NAME##_action_queue_init(&p_store->action_queue);                                       \
  STORE_NAME##_reducer_queue_init(&p_store->reducer_queue);                                     \
}                                                                                               \
                                                                                                \
                                                                                                \
void cedux_dispatch(struct STORE_NAME##_handle * p_store, ACTION_TYPE action) {                 \
}

// // Register a handler to receive actions
// void tree_register_reducer(struct cedux_store * store, );

// // Dispatch an action to the tree
// void tree_dispatch(struct cedux_store store);


// // Register a callback to be called when a portion of the tree's state is updated
// void tree_register_listener();

#endif