#ifndef CEDUX_H
#define CEDUX_H
#include <stddef.h>
#include <stdbool.h>
#include "queue.h"
#include "list.h"

#define CEDUX_DECLARE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)                                 \
                                                                                                \
struct STORE_NAME##_handle;                                                                     \
typedef bool(*STORE_NAME##_REDUCER)(TREE_TYPE * p_tree, ACTION_TYPE action);                    \
typedef void(*STORE_NAME##_SUBSCRIBER)(struct STORE_NAME##_handle * p_store,                    \
                                       TREE_TYPE const * const p_tree, void *data);             \
struct STORE_NAME##_subscriber_container                                                        \
{                                                                                               \
    STORE_NAME##_SUBSCRIBER subscriber;                                                         \
    void *data;                                                                                 \
    STORE_NAME##_REDUCER linked_reducer;                                                        \
};                                                                                              \
QUEUE_TYPE_DECLARATION(STORE_NAME##_action_queue, ACTION_TYPE, 16)                              \
QUEUE_DECLARATION(STORE_NAME##_action_queue, ACTION_TYPE)                                       \
LIST_DECLARATION(STORE_NAME##_reducer_list, STORE_NAME##_REDUCER, 32)                           \
LIST_DECLARATION(STORE_NAME##_subscriber_list, struct STORE_NAME##_subscriber_container, 32)    \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store,                \
                                           STORE_NAME##_REDUCER reducer);                       \
void cedux_register_##STORE_NAME##_subscriber(struct STORE_NAME##_handle * p_store,             \
                                              STORE_NAME##_SUBSCRIBER subscriber,               \
                                              void *data);                                      \
void cedux_register_##STORE_NAME##_linked_subscriber(struct STORE_NAME##_handle * p_store,      \
                                              STORE_NAME##_SUBSCRIBER subscriber,               \
                                              void *data,                                       \
                                              STORE_NAME##_REDUCER linked_reducer);             \
struct STORE_NAME##_handle cedux_init_##STORE_NAME(void);                                       \
void cedux_dispatch_##STORE_NAME(struct STORE_NAME##_handle * p_store, ACTION_TYPE action);     \
bool cedux_run_##STORE_NAME(struct STORE_NAME##_handle * p_store);                              \
void cedux_set_threadsafe_##STORE_NAME(struct STORE_NAME##_handle * p_store, void * lock,       \
                                       cedux_lock_func_t lock_get, cedux_lock_func_t lock_release); \
struct STORE_NAME##_handle                                                                      \
{                                                                                               \
  TREE_TYPE tree;                                                                               \
  struct STORE_NAME##_action_queue action_queue;                                                \
  struct STORE_NAME##_reducer_list reducer_list;                                                \
  struct STORE_NAME##_subscriber_list subscriber_list;                                          \
  void *lock;                                                                                   \
  cedux_lock_func_t lock_get;                                                                   \
  cedux_lock_func_t lock_release;                                                               \
};


#define CEDUX_DEFINE_STORE(TREE_TYPE, ACTION_TYPE, STORE_NAME)                                  \
                                                                                                \
QUEUE_DEFINITION(STORE_NAME##_action_queue, ACTION_TYPE)                                        \
LIST_DEFINITION(STORE_NAME##_reducer_list, STORE_NAME##_REDUCER)                                \
LIST_DEFINITION(STORE_NAME##_subscriber_list, struct STORE_NAME##_subscriber_container)         \
                                                                                                \
void cedux_register_##STORE_NAME##_reducer(struct STORE_NAME##_handle * p_store,                \
                                          STORE_NAME##_REDUCER reducer) {                       \
  STORE_NAME##_reducer_list_push(&p_store->reducer_list, &reducer);                             \
}                                                                                               \
                                                                                                \
void cedux_register_##STORE_NAME##_subscriber(struct STORE_NAME##_handle * p_store,             \
                                              STORE_NAME##_SUBSCRIBER subscriber,               \
                                              void * p_data) {                                  \
  cedux_register_##STORE_NAME##_linked_subscriber(p_store, subscriber, p_data, NULL);           \
}                                                                                               \
                                                                                                \
void cedux_register_##STORE_NAME##_linked_subscriber(struct STORE_NAME##_handle * p_store,      \
                                              STORE_NAME##_SUBSCRIBER subscriber,               \
                                              void * p_data,                                    \
                                              STORE_NAME##_REDUCER linked_reducer) {            \
  struct STORE_NAME##_subscriber_container container;                                           \
  container.subscriber = subscriber;                                                            \
  container.data = p_data;                                                                      \
  container.linked_reducer = linked_reducer;                                                    \
  STORE_NAME##_subscriber_list_push(&p_store->subscriber_list, &container);                     \
}                                                                                               \
                                                                                                \
struct STORE_NAME##_handle cedux_init_##STORE_NAME(void) {                                      \
  struct STORE_NAME##_handle new_store;                                                         \
  STORE_NAME##_action_queue_init(&new_store.action_queue);                                      \
  STORE_NAME##_reducer_list_init(&new_store.reducer_list);                                      \
  STORE_NAME##_subscriber_list_init(&new_store.subscriber_list);                                \
  new_store.lock = NULL;                                                                        \
  new_store.lock_get = NULL;                                                                    \
  new_store.lock_release = NULL;                                                                \
  return new_store;                                                                             \
}                                                                                               \
                                                                                                \
void cedux_dispatch_##STORE_NAME(struct STORE_NAME##_handle * p_store, ACTION_TYPE action) {    \
  if (p_store->lock_get) p_store->lock_get(p_store->lock);                                      \
  STORE_NAME##_action_queue_enqueue(&p_store->action_queue, &action);                           \
  if (p_store->lock_release) p_store->lock_release(p_store->lock);                              \
}                                                                                               \
                                                                                                \
bool cedux_run_##STORE_NAME(struct STORE_NAME##_handle * p_store) {                             \
  ACTION_TYPE action;                                                                           \
  STORE_NAME##_REDUCER reducer;                                                                 \
  struct STORE_NAME##_subscriber_container subscriber_container;                                \
  bool did_work = false;                                                                        \
  if (p_store->lock_get) p_store->lock_get(p_store->lock);                                      \
  while(STORE_NAME##_action_queue_dequeue(&p_store->action_queue, &action) == DEQUEUE_RESULT_SUCCESS) \
  {                                                                                             \
    LIST_FOR_EACH(p_store->reducer_list, reducer)                                               \
    {                                                                                           \
      bool reducer_did_work = reducer(&p_store->tree, action);                                  \
      if (reducer_did_work)                                                                     \
      {                                                                                         \
        did_work = true;                                                                        \
        LIST_FOR_EACH(p_store->subscriber_list, subscriber_container)                           \
        {                                                                                       \
          if (subscriber_container.linked_reducer == reducer)                                   \
          {                                                                                     \
            subscriber_container.subscriber(p_store, &p_store->tree, subscriber_container.data);\
          }                                                                                     \
        }                                                                                       \
      }                                                                                         \
    }                                                                                           \
  }                                                                                             \
  if (p_store->lock_release) p_store->lock_release(p_store->lock);                              \
  if (did_work)                                                                                 \
  {                                                                                             \
    LIST_FOR_EACH(p_store->subscriber_list, subscriber_container)                               \
    {                                                                                           \
      if (subscriber_container.linked_reducer == NULL) {                                        \
        subscriber_container.subscriber(p_store, &p_store->tree, subscriber_container.data);    \
      }                                                                                         \
    }                                                                                           \
  }                                                                                             \
  return did_work;                                                                              \
}                                                                                               \
                                                                                                \
void cedux_set_threadsafe_##STORE_NAME(struct STORE_NAME##_handle * p_store, void * lock,       \
                                       cedux_lock_func_t lock_get, cedux_lock_func_t lock_release) \
{                                                                                               \
  p_store->lock = lock;                                                                         \
  p_store->lock_get = lock_get;                                                                 \
  p_store->lock_release = lock_release;                                                         \
}

#define STORE_TREE(STORE) STORE.tree

typedef void(*cedux_lock_func_t)(void *lock);

#endif
