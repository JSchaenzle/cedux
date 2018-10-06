#ifndef CEDUX_H
#define CEDUX_H
#include <stddef.h>
#include <stdbool.h>
#include "queue.h"
#include "list.h"

#define CEDUX_DECLARE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)                                 \
                                                                                                \
struct STORE_NAME##_handle;                                                                     \
typedef void(*STORE_NAME##_REDUCER)(TREE_TYPE * p_tree, ACTION_TYPE action);                    \
QUEUE_DECLARATION(STORE_NAME##_action_queue, ACTION_TYPE, 16)                                   \
LIST_DECLARATION(STORE_NAME##_reducer_list, STORE_NAME##_REDUCER, 32)                           \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store,                \
                                           STORE_NAME##_REDUCER reducer);                       \
struct STORE_NAME##_handle cedux_init_##STORE_NAME(void);                                       \
void cedux_dispatch_##STORE_NAME(struct STORE_NAME##_handle * p_store, ACTION_TYPE action);     \
bool cedux_run_##STORE_NAME(struct STORE_NAME##_handle * p_store);                              \
struct STORE_NAME##_handle                                                                      \
{                                                                                               \
  TREE_TYPE tree;                                                                               \
  struct STORE_NAME##_action_queue action_queue;                                                \
  struct STORE_NAME##_reducer_list reducer_list;                                                \
};


#define CEDUX_DEFINE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)                                  \
                                                                                                \
QUEUE_DEFINITION(STORE_NAME##_action_queue, ACTION_TYPE)                                        \
LIST_DEFINITION(STORE_NAME##_reducer_list, STORE_NAME##_REDUCER)                                \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store,                \
                                          STORE_NAME##_REDUCER reducer) {                       \
  STORE_NAME##_reducer_list_push(&p_store->reducer_list, &reducer);                             \
}                                                                                               \
                                                                                                \
struct STORE_NAME##_handle cedux_init_##STORE_NAME(void) {                                      \
  struct STORE_NAME##_handle new_store;                                                         \
  STORE_NAME##_action_queue_init(&new_store.action_queue);                                      \
  STORE_NAME##_reducer_list_init(&new_store.reducer_list);                                      \
  return new_store;                                                                             \
}                                                                                               \
                                                                                                \
void cedux_dispatch_##STORE_NAME(struct STORE_NAME##_handle * p_store, ACTION_TYPE action) {    \
  STORE_NAME##_action_queue_enqueue(&p_store->action_queue, &action);                           \
}                                                                                               \
                                                                                                \
bool cedux_run_##STORE_NAME(struct STORE_NAME##_handle * p_store) {                             \
  ACTION_TYPE action;                                                                           \
  STORE_NAME##_REDUCER reducer;                                                                 \
  bool did_work = false;                                                                        \
  while(STORE_NAME##_action_queue_dequeue(&p_store->action_queue, &action) == DEQUEUE_RESULT_SUCCESS) \
  {                                                                                             \
    LIST_FOR_EACH(p_store->reducer_list, reducer)                                               \
    {                                                                                           \
      reducer(&p_store->tree, action);                                                          \
    }                                                                                           \
    did_work = true;                                                                            \
  }                                                                                             \
  return did_work;                                                                              \
}

#endif