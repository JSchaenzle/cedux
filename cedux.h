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
typedef void(*STORE_NAME##_SUBSCRIBER)(const TREE_TYPE * p_tree, void *data);                   \
struct STORE_NAME##_subscriber_container                                                        \
{                                                                                               \
    STORE_NAME##_SUBSCRIBER subscriber;                                                         \
    void *data;                                                                                 \
};                                                                                              \
QUEUE_DECLARATION(STORE_NAME##_action_queue, ACTION_TYPE, 16)                                   \
LIST_DECLARATION(STORE_NAME##_reducer_list, STORE_NAME##_REDUCER, 32)                           \
LIST_DECLARATION(STORE_NAME##_subscriber_list, struct STORE_NAME##_subscriber_container *, 32)  \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store,                \
                                           STORE_NAME##_REDUCER reducer);                       \
void cedux_register_##STORE_NAME##_subscriber(struct STORE_NAME##_handle * p_store,             \
                                              struct STORE_NAME##_subscriber_container * subscriber); \
void cedux_init_##STORE_NAME(struct STORE_NAME##_handle * p_store);                             \
void cedux_dispatch_##STORE_NAME(struct STORE_NAME##_handle * p_store, ACTION_TYPE action);     \
bool cedux_run_##STORE_NAME(struct STORE_NAME##_handle * p_store);                              \
struct STORE_NAME##_handle                                                                      \
{                                                                                               \
  TREE_TYPE tree;                                                                               \
  struct STORE_NAME##_action_queue action_queue;                                                \
  struct STORE_NAME##_reducer_list reducer_list;                                                \
  struct STORE_NAME##_subscriber_list subscriber_list;                                          \
} STORE_NAME;


#define CEDUX_DEFINE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)                                  \
                                                                                                \
QUEUE_DEFINITION(STORE_NAME##_action_queue, ACTION_TYPE)                                        \
LIST_DEFINITION(STORE_NAME##_reducer_list, STORE_NAME##_REDUCER)                                \
LIST_DEFINITION(STORE_NAME##_subscriber_list, struct STORE_NAME##_subscriber_container *)       \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store,                \
                                          STORE_NAME##_REDUCER reducer) {                       \
  STORE_NAME##_reducer_list_push(&p_store->reducer_list, &reducer);                             \
}                                                                                               \
                                                                                                \
void cedux_register_##STORE_NAME##_subscriber(struct STORE_NAME##_handle * p_store,             \
                                              struct STORE_NAME##_subscriber_container * subscriber) { \
  STORE_NAME##_subscriber_list_push(&p_store->subscriber_list, &subscriber);                    \
}                                                                                               \
                                                                                                \
void cedux_init_##STORE_NAME(struct STORE_NAME##_handle * p_store) {                            \
  STORE_NAME##_action_queue_init(&p_store->action_queue);                                       \
  STORE_NAME##_reducer_list_init(&p_store->reducer_list);                                       \
  STORE_NAME##_subscriber_list_init(&p_store->subscriber_list);                                 \
}                                                                                               \
                                                                                                \
void cedux_dispatch_##STORE_NAME(struct STORE_NAME##_handle * p_store, ACTION_TYPE action) {    \
  STORE_NAME##_action_queue_enqueue(&p_store->action_queue, &action);                           \
}                                                                                               \
                                                                                                \
bool cedux_run_##STORE_NAME(struct STORE_NAME##_handle * p_store) {                             \
  ACTION_TYPE action;                                                                           \
  STORE_NAME##_REDUCER reducer;                                                                 \
  struct STORE_NAME##_subscriber_container *subscriber_container;                               \
  bool did_work = false;                                                                        \
  while(STORE_NAME##_action_queue_dequeue(&p_store->action_queue, &action) == DEQUEUE_RESULT_SUCCESS) \
  {                                                                                             \
    LIST_FOR_EACH(p_store->reducer_list, reducer)                                               \
    {                                                                                           \
      reducer(&p_store->tree, action);                                                          \
    }                                                                                           \
    did_work = true;                                                                            \
  }                                                                                             \
  if (did_work)                                                                                 \
  {                                                                                             \
    LIST_FOR_EACH(p_store->subscriber_list, subscriber_container)                               \
    {                                                                                           \
      subscriber_container->subscriber(&p_store->tree, subscriber_container->data);             \
    }                                                                                           \
  }                                                                                             \
  return did_work;                                                                              \
}

#endif
